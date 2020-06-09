/*
 * Copyright (C) 2012 Atmel Corporation
 *
 * Configuation settings for the AT91SAM9X5EK board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <asm/hardware.h>

#define CONFIG_SYS_TEXT_BASE		0x23f00000

/* ARM asynchronous clock */
#define CONFIG_SYS_AT91_SLOW_CLOCK	32768
#define CONFIG_SYS_AT91_MAIN_CLOCK	12000000	/* 12 MHz crystal */

#define CONFIG_CMDLINE_TAG		/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_SKIP_LOWLEVEL_INIT
#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_DISPLAY_CPUINFO

#define CONFIG_CMD_BOOTZ
#define CONFIG_OF_LIBFDT

#define CONFIG_SYS_GENERIC_BOARD

/* general purpose I/O */
#define CONFIG_ATMEL_LEGACY		/* required until (g)pio is fixed */
#define CONFIG_AT91_GPIO

/* serial console */
#define CONFIG_ATMEL_USART
#define CONFIG_USART_BASE	ATMEL_BASE_DBGU
#define CONFIG_USART_ID		ATMEL_ID_SYS

/* LCD */
#undef CONFIG_LCD

#define CONFIG_BOOTDELAY	3

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME

/* no NOR flash */
#define CONFIG_SYS_NO_FLASH

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>
#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_IMI

#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_NAND
#undef CONFIG_CMD_SF
#define CONFIG_CMD_MMC
//#define CONFIG_CMD_FAT
//#define CONFIG_CMD_USB
#define CONFIG_CMD_MII

/*
 * define CONFIG_USB_EHCI to enable USB Hi-Speed (aka 2.0)
 * NB: in this case, USB 1.1 devices won't be recognized.
 */


/* SDRAM */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_SDRAM_BASE		0x20000000
#define CONFIG_SYS_SDRAM_SIZE		0x04000000	/* 64 megs */

#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_SDRAM_BASE + 4 * 1024 - GENERATED_GBL_DATA_SIZE)

/* DataFlash */
#ifdef CONFIG_CMD_SF
#define CONFIG_ATMEL_SPI
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_ATMEL
#define CONFIG_SF_DEFAULT_SPEED		30000000
#endif

/* NAND flash */
#ifdef CONFIG_CMD_NAND

#define CONFIG_NAND_ATMEL
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0x40000000 /* ATMEL_BASE_CS3 */
#define CONFIG_SYS_NAND_DBW_8		1
/* our ALE is AD21 */
#define CONFIG_SYS_NAND_MASK_ALE	(1 << 21)
/* our CLE is AD22 */
#define CONFIG_SYS_NAND_MASK_CLE	(1 << 22)
#define CONFIG_SYS_NAND_ENABLE_PIN	AT91_PIN_PD4   /* Chip select */
#define CONFIG_SYS_NAND_READY_PIN	AT91_PIN_PD5   /* NAND_WAIT */
#define CONFIG_SYS_NAND_ONFI_DETECTION

/* PMECC & PMERRLOC */
#define CONFIG_ATMEL_NAND_HWECC		1
#define CONFIG_ATMEL_NAND_HW_PMECC	1
#define CONFIG_PMECC_CAP		4
#define CONFIG_PMECC_SECTOR_SIZE	512
#define CONFIG_PMECC_INDEX_TABLE_OFFSET 0x8000

#define CONFIG_CMD_NAND_TRIMFFS

#define CONFIG_MTD_DEVICE
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_PARTITIONS
#define CONFIG_RBTREE
#define CONFIG_LZO
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#endif

/* MMC */
#ifdef CONFIG_CMD_MMC
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_GENERIC_ATMEL_MCI
#endif

/* FAT */
#ifdef CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#endif

/* Ethernet */
#define CONFIG_MACB
#define CONFIG_RMII
#define CONFIG_NET_RETRY_COUNT		20
#define CONFIG_MACB_SEARCH_PHY
#define CONFIG_ETHADDR                  00:17:23:00:00:00

#define CONFIG_ENV_OVERWRITE     1

/* USB */
#ifdef CONFIG_CMD_USB
#ifdef CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_ATMEL
#define CONFIG_SYS_USB_EHCI_MAX_ROOT_PORTS	2
#else
#define CONFIG_USB_ATMEL
#define CONFIG_USB_ATMEL_CLK_SEL_UPLL
#define CONFIG_USB_OHCI_NEW
#define CONFIG_SYS_USB_OHCI_CPU_INIT
#define CONFIG_SYS_USB_OHCI_REGS_BASE		ATMEL_BASE_OHCI
#define CONFIG_SYS_USB_OHCI_SLOT_NAME		"at91sam9x5"
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS	3
#endif
#define CONFIG_USB_STORAGE
#endif

#define CONFIG_SYS_LOAD_ADDR		0x22000000	/* load address */

#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		0x23e00000

#ifdef CONFIG_SYS_USE_NANDFLASH
/* bootstrap + u-boot + env + linux in nandflash */
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OFFSET			0xa0000
#define CONFIG_ENV_OFFSET_REDUND	0xc0000
#define CONFIG_ENV_SIZE		0x20000		/* 1 sector = 128 kB */
#define CONFIG_BOOTCOMMAND              "nand read 0x22000000 0x000e0000 0x280000; "\
                                        "run _mtd; bootm"

#else
#error No boot method selected, please select 'CONFIG_SYS_USE_NANDFLASH'
#endif

#define CONFIG_BOOTARGS                 "console=ttyS0,115200 loglevel=4 " \
                                        "rw noinitrd mem=64M " \
                                        "rootfstype=ubifs root=ubi0:rootfs ubi.mtd=6"

#define CONFIG_BAUDRATE		115200

#define CONFIG_SYS_PROMPT	"U-Boot> "
#define CONFIG_SYS_CBSIZE	1024
#define CONFIG_SYS_MAXARGS	16
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_LONGHELP
#define CONFIG_CMDLINE_EDITING
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_HUSH_PARSER

#define MTDIDS_DEFAULT                  "nand0=atmel_nand"
#define MTDPARTS_DEFAULT                "mtdparts=atmel_nand:" \
                                        "128K(at91bs)," \
                                        "512K(u-boot)," \
                                        "128K(u-boot-env)," \
                                        "128K(redund-env)," \
                                        "2560K(kernel-a)," \
                                        "2560K(kernel-b)," \
                                        "38912K(rootfs-a)," \
                                        "38912K(rootfs-b)," \
                                        "46208K(user)," \
                                        "512K(logs)"

#define CONFIG_EXTRA_ENV_SETTINGS       \
        "_mtd=mtdparts default; set bootargs ${bootargs} ${mtdparts}\0" \
        "autoload=no\0" \
		"autostart=no\0" \
		"\0"

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN		(1024 * 1024)

#endif
