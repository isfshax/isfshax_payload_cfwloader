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
#include <string.h>

typedef struct {
    char name[0x100];
    void* data;
    u32 size;
    u32 flags;
    u8 hash[0x14];
    u8 pad[0x0C];
} PACKED prsh_section;

typedef struct {
    u32 checksum;
    u32 magic;
    u32 version;
    u32 size;
    u32 flags;
    u32 max_sections;
    u32 num_sections;
    prsh_section sections[];
} PACKED prsh_header;

static prsh_header* header = NULL;
static bool initialized = false;

void prsh_init(void)
{
    if(initialized) return;

    void* buffer = (void*)0x10000400;
    u32 size = 0x7C00;
    while(size) {
        if(!memcmp(buffer, "PRSH", sizeof(u32))) break;
        buffer += sizeof(u32);
        size -= sizeof(u32);
    }

    header = buffer - sizeof(u32);
    initialized = true;
}

int prsh_get_section(const char* name, void** data, u32* size)
{
    prsh_init();
    if(!name) return -1;

    for(int i = 0; i < header->num_sections; i++) {
        prsh_section* section = &header->sections[i];

        if(!strncmp(name, section->name, sizeof(section->name))) {
            if(data) *data = section->data;
            if(size) *size = section->size;
            return 0;
        }
    }

    return -2;
}
