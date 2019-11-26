/*
 * Watchdog driver for Atmel SAMA5D3x processors series.
 *
 * Copyright (C) 2018
 * Laird  <support@lairdconnect.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * The Watchdog Timer Mode Register can be only written to once. If the
 * timeout need to be set from U-Boot, be sure that the bootstrap doesn't
 * write to this register.
 *
 * Also, check if Watchdog Timer is configured as enabled in bootstrap
 */

 #include <common.h>
 #include <watchdog.h>
 #include <asm/arch/hardware.h>
 #include <asm/io.h>
 #include <asm/arch/at91_wdt.h>

/*
 * SAMA5D3x watchdog runs a 12bit counter @ 256Hz,
 * use this to convert a watchdog
 * value from/to milliseconds.
 */
 #define ms_to_ticks(t)	(((t << 8) / 1000) - 1)
 #define ticks_to_ms(t)	(((t + 1) * 1000) >> 8)

/*
 * Default timeout to 16 seconds
 */
 #define DEFAULT_TIMEOUT 16

/*
 * Set the watchdog time interval in 1/256Hz (write-once)
 * Counter is 12 bit.
 */
static int sama5d3_wdt_settimeout(unsigned int timeout)
{
    /* 16 seconds timer, resets enabled */
    unsigned int reg;
    at91_wdt_t *wd = (at91_wdt_t *) ATMEL_BASE_WDT;

	/* Check if disabled */
    if (readl(&wd->mr) & AT91_WDT_MR_WDDIS)
    {
        printf("sorry, Watchdog is disabled\n");

        return -1;
    }

    /* Check timeout value if valid*/
    if((timeout<=0) || (timeout>16))
    {
        printf("Watchdog: timeout value invalid\n");
        printf("Watchdog: reseting timeout value to default\n");
        timeout = DEFAULT_TIMEOUT;
    }

	/*
	 * All counting occurs at SLOW_CLOCK / 128 = 256 Hz
	 *
	 * Since WDV is a 12-bit counter, the maximum period is
	 * 4096 / 256 = 16 seconds.
	 */
    reg =AT91_WDT_MR_WDRSTEN /* reset enabled*/
        | AT91_WDT_MR_WDDBGHLT /* disabled in debug mode */
        | AT91_WDT_MR_WDD(0xfff) /* restart at any time */
        | AT91_WDT_MR_WDV(ms_to_ticks(timeout * 1000));	/* timeout value */

    printf("Watchdog: timeout %d secs",timeout);
    writel(reg, &wd->mr);

    return 0;
}

/*
 * Disable Watchdog
 *
 */
static void sama5d3_wdt_disable(void)
{
    at91_wdt_t *wd = (at91_wdt_t *) ATMEL_BASE_WDT;
    printf("Watchdog: Disabled\n");
    writel(AT91_WDT_MR_WDDIS,&wd->mr);
}

/*
 * Reset watchdog
 *
 */
void hw_watchdog_reset(void)
{
    at91_wdt_t *wd = (at91_wdt_t *) ATMEL_BASE_WDT;
    printf("Watchdog: Reset\n");
    writel(AT91_WDT_CR_WDRSTT | AT91_WDT_CR_KEY, &wd->cr);
}

/*
 * Initialize watchdog
 *
 */
void hw_watchdog_init(void)
{
    const char *watchdog= getenv("watchdog");
    unsigned int timeout= (unsigned int) getenv_ulong("timeout",10,DEFAULT_TIMEOUT);

    if(watchdog)
    {
        if((strcmp("off",watchdog) == 0))
        {
            sama5d3_wdt_disable();
        }

        else if(strcmp("on",watchdog) == 0)
        {
            sama5d3_wdt_settimeout(timeout);
        }

        else
        {
            sama5d3_wdt_disable(); // to disable by default
        }

    }

    else
    {
        sama5d3_wdt_disable();
    }
}
