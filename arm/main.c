/*
 *  minute - a port of the "mini" IOS replacement for the Wii U.
 *
 *  Copyright (C) 2021          rw-r-r-0644 <rwrr0644@gmail.com>
 *  Copyright (C) 2017          Ash Logan <quarktheawesome@gmail.com>
 *
 *  Copyright (C) 2016          SALT
 *  Copyright (C) 2016          Daz Jones <daz@dazzozo.com>
 *
 *  Copyright (C) 2008, 2009    Haxx Enterprises <bushing@gmail.com>
 *  Copyright (C) 2008, 2009    Sven Peter <svenpeter@gmail.com>
 *  Copyright (C) 2008, 2009    Hector Martin "marcan" <marcan@marcansoft.com>
 *  Copyright (C) 2009          Andre Heider "dhewg" <dhewg@wiibrew.org>
 *  Copyright (C) 2009          John Kelley <wiidev@kelley.ca>
 *
 *  This code is licensed to you under the terms of the GNU GPL, version 2;
 *  see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include <stdlib.h>
#include <malloc.h>
#include "video/gfx.h"
#include "system/exception.h"
#include "system/memory.h"
#include "system/irq.h"
#include "storage/sd/sdcard.h"
#include "storage/sd/fatfs/elm.h"
#include "storage/nand/nand.h"
#include "crypto/crypto.h"
#include "system/smc.h"
#include "system/boot_info.h"
#include "system/latte.h"
#include "system/gpio.h"
#include "system/rtc.h"
#include "system/exi.h"
#include "system/ancast.h"
#include "common/utils.h"
#include "lolserial.h"
#include "stdio.h"

extern int _lolserial_enable;

u32 _main(void* base)
{
    int res;
	u32 vector = 0;

	exception_initialize();
    exi_initialize();
	lolserial_init();
	irq_initialize();
	crypto_initialize();

    lolserial_suspend();

	printf("Initializing SD card...\n");
	sdcard_init();

	printf("Mounting SD card...\n");
	res = ELM_Mount();

    lolserial_resume();

	if(res) {
		printf("Error while mounting SD card (%d).\n", res);
		panic(0);
	}

	printf("Writing fake boot_info data\n");
    boot_info_boot1_fill();

    printf("Loading ancast image\n");
	vector = ancast_iop_load("fw.img");
	printf("vector: %08X\n", vector);

    printf("Writing SMC\n");
    smc_write_raw(SMC_CMD_ODDPOWER_ON);
    smc_write_raw(SMC_CMD_ONINDICATOR_ON);

    /*
     * boot1 normally shuts down the fan until IOS reenables it.
     * However this might lead to overheating if fw fails to load,
     * probably best not to do this here
    clear32(LT_GPIO2_OWNER, GP2_DCDC2);
    write32(LT_GPIO2_OUT, GP2_DCDC2);
    set32(LT_GPIO2_ENABLE, GP2_DCDC2);
    set32(LT_GPIO2_DIR, GP2_DCDC2);
    write32(LT_GPIO2_OUT, GP2_DCDC2);*/

	printf("Shutting down SD card...\n");
	ELM_Unmount();
	sdcard_exit();

	printf("Waiting power button input...\n");
	smc_wait_events(SMC_POWER_BUTTON);

	irq_shutdown();
	if (!vector)
		smc_shutdown(false);

	return vector;
}
