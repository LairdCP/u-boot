/*
 * Copyright (c) 2010-2020 Laird Connectivity
 * All rights reserved.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <linux/sizes.h>

/*
 * SoC must be defined first, before hardware.h is included.
 * In this case SoC is defined in boards.cfg.
 */
#include <asm/hardware.h>

/*
 * Warning: changing CONFIG_SYS_TEXT_BASE requires
 * adapting the initial boot program.
 * Since the linker has to swallow that define, we must use a pure
 * hex number here!
 */
#define CONFIG_SYS_TEXT_BASE		0x23f00000

/* ARM asynchronous clock */
#define CONFIG_SYS_AT91_SLOW_CLOCK      32768
#define CONFIG_SYS_AT91_MAIN_CLOCK      12000000 /* from 12 MHz crystal */

#define CONFIG_ARCH_CPU_INIT

#define CONFIG_CMDLINE_TAG              /* enable passing of ATAGs */
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
#define CONFIG_USART_BASE		ATMEL_BASE_DBGU
#define CONFIG_USART_ID 		ATMEL_ID_SYS

#define CONFIG_BOOTDELAY		3

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME

/* NOR flash - no real flash on this board */
#define CONFIG_SYS_NO_FLASH

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_IMI
#undef CONFIG_CMD_LOADS
#undef CONFIG_CMD_SF
#undef CONFIG_CMD_NFS
#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_NAND
//#define CONFIG_CMD_MEMTEST
//#define CONFIG_CMD_MMC
//#define CONFIG_CMD_USB
//#define CONFIG_CMD_MII

#if defined(CONFIG_CMD_USB) || defined(CONFIG_CMD_MMC)
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#endif

#undef CONFIG_SYS_BOOT_RAMDISK_HIGH

/* SDRAM */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_SDRAM_BASE		0x20000000
#define CONFIG_SYS_SDRAM_SIZE		SZ_64M

#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_SDRAM_BASE + SZ_4K - GENERATED_GBL_DATA_SIZE)

/* NAND flash */
#ifdef CONFIG_CMD_NAND

#define CONFIG_NAND_ATMEL
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0x40000000 /* ATMEL_BASE_CS3 */
#define CONFIG_SYS_NAND_DBW_8
/* our ALE is AD21 */
#define CONFIG_SYS_NAND_MASK_ALE	(1 << 21)
/* our CLE is AD22 */
#define CONFIG_SYS_NAND_MASK_CLE	(1 << 22)
#define CONFIG_SYS_NAND_ENABLE_PIN	AT91_PIN_PD4   /* Chip select */
#define CONFIG_SYS_NAND_READY_PIN	AT91_PIN_PD5   /* NAND_WAIT */
#define CONFIG_SYS_NAND_ONFI_DETECTION

/* PMECC & PMERRLOC */
#define CONFIG_ATMEL_NAND_HWECC
#define CONFIG_ATMEL_NAND_HW_PMECC
#define CONFIG_PMECC_CAP		4
#define CONFIG_PMECC_SECTOR_SIZE	512

#define CONFIG_CMD_NAND_TRIMFFS

#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_PARTITIONS
#define CONFIG_MTD_DEVICE
#define CONFIG_RBTREE
#endif

/* MMC */
#ifdef CONFIG_CMD_MMC
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_GENERIC_ATMEL_MCI
#endif

/* Ethernet */
#define CONFIG_MACB
#define CONFIG_RMII
#define CONFIG_NET_RETRY_COUNT		20
#define CONFIG_MACB_SEARCH_PHY
#define CONFIG_PHYLIB
#define CONFIG_ETHADDR			00:17:23:00:00:00

#define CONFIG_ENV_OVERWRITE

#define CONFIG_HW_WATCHDOG
#define CONFIG_AT91SAM9_WATCHDOG
#define CONFIG_AT91_HW_WDT_TIMEOUT	15

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
#define CONFIG_SYS_USB_OHCI_SLOT_NAME		"wb45n"
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS	3
#endif
#define CONFIG_USB_STORAGE
#endif

#define CONFIG_SYS_LOAD_ADDR		0x20001000

#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		0x23e00000
#define CONFIG_SYS_ALT_MEMTEST

#ifdef CONFIG_SYS_USE_NANDFLASH
/* bootstrap + u-boot + env + linux in nandflash */
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OFFSET		0xa0000
#define CONFIG_ENV_OFFSET_REDUND	0xc0000
#define CONFIG_ENV_SIZE 		0x20000		/* 1 sector = 128 kB */

#define CONFIG_BOOTCOMMAND		"nand read 0x22000000 0x000e0000 0x280000; " \
					"bootm"
#else
#error No boot method selected, please select 'CONFIG_SYS_USE_NANDFLASH'
#endif

#define CONFIG_BOOTARGS			"console=ttyS0,115200 loglevel=4 rw " \
					"rootfstype=ubifs root=ubi0:rootfs " \
					"ubi.mtd=6"

#define CONFIG_BAUDRATE			115200

#define CONFIG_SYS_PROMPT		"U-Boot> "
#define CONFIG_SYS_CBSIZE		1024
#define CONFIG_SYS_MAXARGS		16
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_LONGHELP
#define CONFIG_CMDLINE_EDITING
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_HUSH_PARSER

#define MTDIDS_DEFAULT			"nand0=atmel_nand"
#define MTDPARTS_DEFAULT		"mtdparts=atmel_nand:" \
					"128K(at91bs)," \
					"512K(u-boot)," \
					"128K(u-boot-env-a)," \
					"128K(u-boot-env-b)," \
					"2560K(kernel-a)," \
					"2560K(kernel-b)," \
					"38912K(rootfs-a)," \
					"38912K(rootfs-b)," \
					"46208K(user)," \
					"512K(logs)"

#define CONFIG_EXTRA_ENV_SETTINGS       \
		"autoload=no\0" \
		"autostart=no\0" \
		"mtdids=" MTDIDS_DEFAULT "\0" \
		"mtdparts=" MTDPARTS_DEFAULT "\0" \
		"\0"

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN		(SZ_512K + SZ_4K)

#endif
