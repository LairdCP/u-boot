/*
 * Copyright (C) 2012-2020 Laird Connectivity
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <mmc.h>
#include <asm/io.h>
#include <asm/arch/sama5d3_smc.h>
#include <asm/arch/at91_common.h>
#include <asm/arch/at91_pmc.h>
#include <asm/arch/at91_rstc.h>
#include <asm/arch/gpio.h>
#include <asm/arch/clk.h>
#include <lcd.h>
#include <atmel_hlcdc.h>
#include <atmel_mci.h>
#include <micrel.h>
#include <net.h>
#include <netdev.h>
#include <spl.h>
#include <asm/arch/atmel_mpddrc.h>
#include <asm/arch/at91_wdt.h>

#ifdef CONFIG_USB_GADGET_ATMEL_USBA
#include <asm/arch/atmel_usba_udc.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_NAND_ATMEL
void wb50n_nand_hw_init(void)
{
	struct at91_smc *smc = (struct at91_smc *)ATMEL_BASE_SMC;

	at91_periph_clk_enable(ATMEL_ID_SMC);

	/* Configure SMC CS3 for NAND to ONFI Timing Mode 3 */
	writel(AT91_SMC_SETUP_NWE(1) | AT91_SMC_SETUP_NCS_WR(0) |
	       AT91_SMC_SETUP_NRD(0) | AT91_SMC_SETUP_NCS_RD(0),
	       &smc->cs[3].setup);
	writel(AT91_SMC_PULSE_NWE(3) | AT91_SMC_PULSE_NCS_WR(6) |
	       AT91_SMC_PULSE_NRD(3) | AT91_SMC_PULSE_NCS_RD(6),
	       &smc->cs[3].pulse);
	writel(AT91_SMC_CYCLE_NWE(6) | AT91_SMC_CYCLE_NRD(6),
	       &smc->cs[3].cycle);
	writel(AT91_SMC_TIMINGS_TCLR(2) | AT91_SMC_TIMINGS_TADL(15) |
	       AT91_SMC_TIMINGS_TAR(2)  | AT91_SMC_TIMINGS_TRR(3)   |
	       AT91_SMC_TIMINGS_TWB(15)  | AT91_SMC_TIMINGS_RBNSEL(3)|
	       AT91_SMC_TIMINGS_NFSEL(1), &smc->cs[3].timings);
	writel(AT91_SMC_MODE_RM_NRD | AT91_SMC_MODE_WM_NWE |
	       AT91_SMC_MODE_EXNW_DISABLE |
#ifdef CONFIG_SYS_NAND_DBW_16
	       AT91_SMC_MODE_DBW_16 |
#else /* CONFIG_SYS_NAND_DBW_8 */
	       AT91_SMC_MODE_DBW_8 |
#endif
	       AT91_SMC_MODE_TDF |
	       AT91_SMC_MODE_TDF_CYCLE(12),
	       &smc->cs[3].mode);

	/* Disable Flash Write Protect Line */
	at91_set_pio_output(AT91_PIO_PORTE, 14, 1);
}
#endif

#ifdef CONFIG_CMD_USB
static void wb50n_usb_hw_init(void)
{
	at91_set_pio_output(AT91_PIO_PORTD, 25, 0);
	at91_set_pio_output(AT91_PIO_PORTD, 26, 0);
	at91_set_pio_output(AT91_PIO_PORTD, 27, 0);
}
#endif

#ifdef CONFIG_GENERIC_ATMEL_MCI
static void wb50n_mci_hw_init(void)
{
	at91_mci_hw_init();

	at91_set_pio_output(AT91_PIO_PORTB, 10, 0);	/* MCI0 Power */
}
#endif

int board_early_init_f(void)
{
	at91_periph_clk_enable(ATMEL_ID_PIOA);
	at91_periph_clk_enable(ATMEL_ID_PIOB);
	at91_periph_clk_enable(ATMEL_ID_PIOC);
	at91_periph_clk_enable(ATMEL_ID_PIOD);
	at91_periph_clk_enable(ATMEL_ID_PIOE);

	at91_seriald_hw_init();

	return 0;
}

int board_init(void)
{
	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;

#ifdef CONFIG_NAND_ATMEL
	wb50n_nand_hw_init();
#endif
#ifdef CONFIG_CMD_USB
	wb50n_usb_hw_init();
#endif
#ifdef CONFIG_USB_GADGET_ATMEL_USBA
	at91_udp_hw_init();
#endif
#ifdef CONFIG_GENERIC_ATMEL_MCI
	wb50n_mci_hw_init();
#endif
#ifdef CONFIG_MACB
	if (has_emac())
		at91_macb_hw_init();
	if (has_gmac())
		at91_gmac_hw_init();
#endif
	return 0;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size((void *)CONFIG_SYS_SDRAM_BASE,
				    CONFIG_SYS_SDRAM_SIZE);
	return 0;
}

int board_phy_config(struct phy_device *phydev)
{
	return 0;
}

int board_eth_init(bd_t *bis)
{
	int rc = 0;

#ifdef CONFIG_MACB
	if (has_emac())
		rc = macb_eth_initialize(0, (void *)ATMEL_BASE_EMAC, 0x00);
	if (has_gmac())
		rc = macb_eth_initialize(0, (void *)ATMEL_BASE_GMAC, 0x00);
#endif
#ifdef CONFIG_USB_GADGET_ATMEL_USBA
	usba_udc_probe(&pdata);
#ifdef CONFIG_USB_ETH_RNDIS
	usb_eth_initialize(bis);
#endif
#endif

	return rc;
}

#ifdef CONFIG_GENERIC_ATMEL_MCI
int board_mmc_init(bd_t *bis)
{
	int rc = 0;

	rc = atmel_mci_init((void *)ATMEL_BASE_MCI0);

	return rc;
}
#endif


#ifdef CONFIG_BOARD_LATE_INIT
#include <linux/ctype.h>
int board_late_init(void)
{
#ifdef CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG
	const char *SAMA5D3_BOARD_EK_NAME = "lrd_name";
	char name[32], *p;

	strcpy(name, get_cpu_name());
	for (p = name; *p != '\0'; *p = tolower(*p), p++);
	strcat(name, "-wb50n");
	setenv(SAMA5D3_BOARD_EK_NAME, name);

#endif

	return 0;
}
#endif
