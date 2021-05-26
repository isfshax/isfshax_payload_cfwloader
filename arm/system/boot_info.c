/*
 *  minute - a port of the "mini" IOS replacement for the Wii U.
 *
 *  Copyright (C) 2016          SALT
 *  Copyright (C) 2016          Daz Jones <daz@dazzozo.com>
 *
 *  This code is licensed to you under the terms of the GNU GPL, version 2;
 *  see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include "common/types.h"
#include "common/utils.h"
#include "system/prsh.h"
#include <string.h>

typedef struct {
    u32 valid;
    u32 boot_flags;
#define FL_NDEV_MODE        2
#define FL_BOOT1_DONE       4
    u32 boot_state;
    u32 boot_count;
    u32 field_0x10;
    u32 field_0x14;
    u32 field_0x18;
    u32 field_0x1c;
    u32 field_0x20;
    u32 field_0x24;
    u64 preloaded_fw_tid;
    u32 preloaded_fw_addr;
    u32 preloaded_fw_size;
    u32 boot_times[8];
#define TIME_BOOT1_MAIN     0
#define TIME_BOOT1_READ     1
#define TIME_BOOT1_VERIFY   2
#define TIME_BOOT1_DECRYPT  3
#define TIME_BOOT0_MAIN     4
#define TIME_BOOT0_READ     5
#define TIME_BOOT0_VERIFY   6
#define TIME_BOOT0_DECRYPT  7
} PACKED boot_info;

int boot_info_boot1_fill(void)
{
    boot_info* info = NULL;
    u32 size = 0;

    if(prsh_get_section("boot_info", (void *)&info, &size))
        return -1;
    if(!info || size != sizeof(boot_info))
        return -2;

    info->boot_times[TIME_BOOT1_MAIN] = 0x49ee7;
    info->boot_times[TIME_BOOT1_READ] = 0x269d5;
    info->boot_times[TIME_BOOT1_VERIFY] = 0xbeaa;
    info->boot_times[TIME_BOOT1_DECRYPT] = 0xa26c;
    info->boot_times[TIME_BOOT0_MAIN] = read32(0x0d417fe0);
    info->boot_times[TIME_BOOT0_READ] = read32(0x0d417fe4);
    info->boot_times[TIME_BOOT0_VERIFY] = read32(0x0d417fe8);
    info->boot_times[TIME_BOOT0_DECRYPT] = read32(0x0d417fec);
    info->boot_flags |= FL_NDEV_MODE | FL_BOOT1_DONE;
    info->boot_count++;

    return 0;
}
