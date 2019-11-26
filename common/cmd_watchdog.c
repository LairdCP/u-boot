/*
 * Watchdog driver command for Atmel SAMA5D3x processors series.
 *
 * (C) Copyright 2018
 * Laird <support@lairdconnect.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 *
 * Watchdog commands to drive Atmel SAMA5D3x processor drivers in
 * U-Boot
 *
 */
 #include <common.h>
 #include <command.h>
 #include <linux/ctype.h> /* isdigit */
 #ifndef CONFIG_HW_WATCHDOG
 #define CONFIG_HW_WATCHDOG
 #include <watchdog.h>
 #endif

/*
 * To check if input argument is only a numeric value
 *
 */
static bool is_number(const char* temp_arg)
{
    int pos = 0 ;

    if(temp_arg[pos]== '-')
    {
        pos=1;
    }

    while(temp_arg[pos]!= 0)
    {
        if(!isdigit(temp_arg[pos]))
        {
            return false;
        }
        pos++;
    }

    return true;
}


/*
 * Set watchdog environment variable
 *
 */
static int do_env_set(const char* env_var, const char* env_value)
{
    setenv(env_var,env_value);
    return 0;
}


static int do_watchdog(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int i = 1; /* Skip argv[0] (program name). */
    int status = 1;
    int input = -1;
    if(argc < 2)
    {
        return CMD_RET_USAGE;
    }

    while (i < argc)
    {
        if (strcmp(argv[i], "-t") == 0)  /* Process optional arguments. */
        {
            if (i + 1 <= argc - 1)  /* There are enough arguments in argv. */
            {
                i++;
                /*check if input arg is valid */
                if(!(is_number(argv[i])))
                {
                    printf("\nERROR: Timeout value invalid\n");
                    return CMD_RET_USAGE;
                }
                input = simple_strtol(argv[i],NULL,10);

                /* Check if input is a number and in range */
                if(!((input > 0) && (input <= 16)))
                {
                    printf("\nERROR: Out of range\n");
                    return CMD_RET_USAGE;
                }
                status = do_env_set("timeout", argv[i]);
            }

            else
            {
                return CMD_RET_USAGE;
            }
        }

        else
        {
            if(i == argc-1)
            {
                if(strcmp(argv[i], "on") == 0)
                {
                    status = do_env_set("watchdog", "on");
                }

                else if(strcmp(argv[i], "off") == 0)
                {
                    status = do_env_set("watchdog", "off");
                }

                else
                {
                    return CMD_RET_USAGE;
                }
            }

            else
            {
                return CMD_RET_USAGE;
            }

        }
        i++;
    }

    printf("Setting watchdog values... DONE\n");
    printf("Writing watchdog values... \n");

    if(!status)
    {
        saveenv();
    }

    return 0;
}

U_BOOT_CMD(watchdog, 4, 0,do_watchdog,
	"Enable/Disable/Set <timeout> for Watchdog timer\n",
	"[-t] <timeout> [on|off]\n"
	"       - timeout in seconds (timeout is decimal and 0 < timeout <=16 !!!)"
);
