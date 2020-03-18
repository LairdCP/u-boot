/*
 * Copyright (c) 2010-2020, Laird Connectivity
 * Christopher Cole <chris.cole@lairdconnect.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/at91sam9260_matrix.h>
#include <asm/arch/at91sam9_smc.h>
#include <asm/arch/at91_common.h>
#include <asm/arch/at91_pmc.h>
#include <asm/arch/at91_rstc.h>
#include <asm/arch/gpio.h>

#include <netdev.h>

DECLARE_GLOBAL_DATA_PTR;

/* ------------------------------------------------------------------------- */
/*
 * Miscelaneous platform dependent initialisations
 */

#ifdef CONFIG_CMD_NAND
static void wb40n_nand_hw_init(void)
{
	struct at91_smc *smc = (struct at91_smc *)ATMEL_BASE_SMC;
	struct at91_matrix *matrix = (struct at91_matrix *)ATMEL_BASE_MATRIX;
	unsigned long csa;

	/* Assign CS3 to NAND/SmartMedia Interface */
	csa = readl(&matrix->ebicsa);
	csa |= AT91_MATRIX_CS3A_SMC_SMARTMEDIA;
	writel(csa, &matrix->ebicsa);

	/* Configure SMC CS3 for NAND/SmartMedia */
	writel(AT91_SMC_SETUP_NWE(1) | AT91_SMC_SETUP_NCS_WR(0) |
		AT91_SMC_SETUP_NRD(1) | AT91_SMC_SETUP_NCS_RD(0),
		&smc->cs[3].setup);
	writel(AT91_SMC_PULSE_NWE(3) | AT91_SMC_PULSE_NCS_WR(6) |
		AT91_SMC_PULSE_NRD(3) | AT91_SMC_PULSE_NCS_RD(6),
		&smc->cs[3].pulse);
	writel(AT91_SMC_CYCLE_NWE(6) | AT91_SMC_CYCLE_NRD(6),
		&smc->cs[3].cycle);
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

	/* Configure RDY/BSY */
	at91_set_gpio_input(CONFIG_SYS_NAND_READY_PIN, 1);

	/* Enable NandFlash */
	at91_set_gpio_output(CONFIG_SYS_NAND_ENABLE_PIN, 1);

}
#endif

#ifdef CONFIG_MACB
static void wb40n_macb_hw_init(void)
{
	struct at91_pmc *pmc = (struct at91_pmc *)ATMEL_BASE_PMC;
	struct at91_port *pioa = (struct at91_port *)ATMEL_BASE_PIOA;
	struct at91_rstc *rstc = (struct at91_rstc *)ATMEL_BASE_RSTC;
	unsigned long erstl;

	/* Enable EMAC clock */
	writel(1 << ATMEL_ID_EMAC0, &pmc->pcer);

	/*
	 * Disable pull-up on:
	 *	RXDV (PA17) => PHY normal mode (not Test mode)
	 *	ERX0 (PA14) => PHY ADDR0
	 *	ERX1 (PA15) => PHY ADDR1
	 *
	 * PHY has internal pull-down
	 */
	writel(pin_to_mask(AT91_PIN_PA14) |
		pin_to_mask(AT91_PIN_PA15) |
		pin_to_mask(AT91_PIN_PA17),
		&pioa->pudr);

	/* Ensure that PHY ADR bits are zero during PHY reset */
	at91_set_gpio_output(AT91_PIN_PA14, 0);
	at91_set_gpio_output(AT91_PIN_PA15, 0);
	at91_set_gpio_output(AT91_PIN_PA17, 0);

	erstl = readl(&rstc->mr) & AT91_RSTC_MR_ERSTL_MASK;

	/* Need to reset PHY -> 500ms reset */
	writel(AT91_RSTC_KEY | AT91_RSTC_MR_ERSTL(13) |
		AT91_RSTC_MR_URSTEN, &rstc->mr);

	writel(AT91_RSTC_KEY | AT91_RSTC_CR_EXTRST, &rstc->cr);

	/* Wait for end hardware reset */
	while (!(readl(&rstc->sr) & AT91_RSTC_SR_NRSTL))
		;

	/* Restore NRST value */
	writel(AT91_RSTC_KEY | erstl | AT91_RSTC_MR_URSTEN,
		&rstc->mr);

	/* Re-enable pull-up */
	writel(pin_to_mask(AT91_PIN_PA14) |
		pin_to_mask(AT91_PIN_PA15) |
		pin_to_mask(AT91_PIN_PA17),
		&pioa->puer);

	/* Initialize EMAC=MACB hardware */
	at91_macb_hw_init();
}
#endif

int board_early_init_f(void)
{
	struct at91_pmc *pmc = (struct at91_pmc *)ATMEL_BASE_PMC;

	/* Enable clocks for all PIOs */
	writel((1 << ATMEL_ID_PIOA) | (1 << ATMEL_ID_PIOB) |
		(1 << ATMEL_ID_PIOC),
		&pmc->pcer);

	return 0;
}

int board_init(void)
{
	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;

	at91_seriald_hw_init();
#ifdef CONFIG_CMD_NAND
	wb40n_nand_hw_init();
#endif
#ifdef CONFIG_MACB
	wb40n_macb_hw_init();
#endif

	/* Ensure that Wifi module is kept powered off and held in reset during boot */
	/* Set SYS_RST_L LOW */
	at91_set_gpio_output(AT91_PIN_PB13, 0);
	/* Set CHIP_PWD_L LOW */
	at91_set_gpio_output(AT91_PIN_PB31, 0);
	/* Set BT_RST_L LOW */
	at91_set_gpio_output(AT91_PIN_PC11, 0);

#ifdef CONFIG_USB_ATMEL
	/* Enable USB host ports */
	at91_set_gpio_output(AT91_PIN_PC0, 0);
#endif

	return 0;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size(
		(void *)CONFIG_SYS_SDRAM_BASE,
		CONFIG_SYS_SDRAM_SIZE);
	return 0;
}

int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_MACB
	rc = macb_eth_initialize(0, (void *)ATMEL_BASE_EMAC0, 0x00);
#endif
	return rc;
}
