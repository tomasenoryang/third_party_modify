/*
 * DMI Decode Extension - 使用示例
 *
 * 演示如何使用 dmi_decode_ext 模块获取 DMI header 信息
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dmi_decode_ext.h"
#include "dmi_paths.h"   /* 使用 SYS_ENTRY_FILE, SYS_TABLE_FILE */
#include "util.h"        /* 使用 read_file */
#include "dmioem.h"

/* DMI 类型名称（部分常用类型） */
static const char *dmi_type_name(u8 type)
{
    switch (type)
    {
        case 0:   return "BIOS Information";
        case 1:   return "System Information";
        case 2:   return "Baseboard Information";
        case 3:   return "Chassis Information";
        case 4:   return "Processor Information";
        case 7:   return "Cache Information";
        case 9:   return "System Slots";
        case 16:  return "Physical Memory Array";
        case 17:  return "Memory Device";
        case 127: return "End Of Table";
        default:  return "Other";
    }
}


/* 示例回调：打印每个 header 的信息 */
static int print_callback(const struct dmi_header *h, const u8 *data, void *user_data)
{
    // int *count = (int *)user_data;
    // (*count)++;

    // printf("[%3d] Type: %3d (%-25s) Handle: 0x%04X Length: %3d\n",
    //        *count, h->type, dmi_type_name(h->type), h->handle, h->length);
    /* assign vendor for vendor-specific decodes later */
    if (h->type == 1 && h->length >= 5){
        // 这里只是一个小例子具体字段如何详细解析可参照dmi_parse_controller_structure
        dmi_set_vendor(dmi_string(h, data[0x04]));
        printf("vendor: %s\n", dmi_string(h, data[0x04]));
    }


    return 0; /* 继续解析 */
}

/* 示例：收集特定类型的所有 header */
struct collector {
    struct dmi_header *headers;
    int count;
    int capacity;
    u8 target_type;
};

static int collect_type_callback(const struct dmi_header *h, const u8 *data, void *user_data)
{
    struct collector *col = (struct collector *)user_data;

    if (h->type == col->target_type)
    {
        if (col->count < col->capacity)
        {
            col->headers[col->count++] = *h;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    u8 *buf;
    size_t size;
    int count = 0;
    struct dmi_header found_header;

    (void)argc;
    (void)argv;

    printf("DMI Decode Extension Example\n");
    printf("============================\n\n");

    /* 使用 read_file 读取 DMI 表 */
    size = 0x10000;  /* 最大 64KB，read_file 会自动调整 */
    buf = read_file(0, &size, SYS_ENTRY_FILE);
    if (buf == NULL)
    {
        fprintf(stderr, "Cannot read DMI table from %s\n", SYS_TABLE_FILE);
        printf("Note: You may need root privileges to access DMI tables.\n");
        return 1;
    }

    printf("DMI table size: %zu bytes\n\n", size);
    if (!(size >= 31 && memcmp(buf, "_SM_", 4) == 0))
    {
        fprintf(stderr, "DMI table is not SMBIOS\n");
        return -1;
    }
    u16 ver;
    /* Don't let checksum run beyond the buffer */
	if (buf[0x05] > 0x20)
	{
		fprintf(stderr,
			"Entry point length too large (%u bytes, expected %u).\n",
			(unsigned int)buf[0x05], 0x1FU);
		return 0;
	}

    if (!checksum(buf, buf[0x05])
    || memcmp(buf + 0x10, "_DMI_", 5) != 0
    || !checksum(buf + 0x10, 0x0F))
       return 0;
    ver = (buf[0x06] << 8) + buf[0x07];
    /* Some BIOS report weird SMBIOS version, fix that up */
    switch (ver)
    {
        case 0x021F:
        case 0x0221:
            ver = 0x0203;
            break;
        case 0x0233:
            ver = 0x0206;
            break;
    }

    u32 len = WORD(buf + 0x16);
    off_t base = DWORD(buf + 0x18);
    u16 num = WORD(buf + 0x1C);
    u32 ver32 = (ver << 8);
    u32 flags = FLAG_NO_FILE_OFFSET;
    char devmem[] = SYS_TABLE_FILE;

    if (!flags & FLAG_NO_FILE_OFFSET){
        fprintf(stderr, "Cannot read DMI table from %s\n", SYS_TABLE_FILE);
        return -1;
    }
    /*
    * When reading from sysfs or from a dump file, the file may be
    * shorter than announced. For SMBIOS v3 this is expcted, as we
    * only know the maximum table size, not the actual table size.
    * For older implementations (and for SMBIOS v3 too), this
    * would be the result of the kernel truncating the table on
    * parse error.
    */
    size = len;
    buf = read_file(flags & FLAG_NO_FILE_OFFSET ? 0 : base,
        &size, devmem);
    len = size;
    if (buf == NULL){
        fprintf(stderr, "Failed to read DMI table from %s\n", SYS_TABLE_FILE);
        return -1;
    }
    /* 示例 1: 使用回调打印所有 header */
    printf("=== All DMI Structures ===\n");
    int total = dmi_table_decode_ext(buf, len, num, ver >> 8, flags, print_callback, &count);
    printf("\nTotal structures decoded: %d\n\n", total);

    // /* 示例 2: 查找特定类型 (Type 1 - System Information) */
    // printf("=== Find Type 1 (System Information) ===\n");
    // if (dmi_table_find_by_type(buf, (u32)size, 0, 0, 1, &found_header))
    // {
    //     printf("Found! Handle: 0x%04X, Length: %d bytes\n", 
    //            found_header.handle, found_header.length);
    // }
    // else
    // {
    //     printf("Not found.\n");
    // }
    // printf("\n");

    // /* 示例 3: 收集所有内存设备 (Type 17) */
    // printf("=== Collect All Memory Devices (Type 17) ===\n");
    // struct collector mem_collector = {
    //     .headers = malloc(64 * sizeof(struct dmi_header)),
    //     .count = 0,
    //     .capacity = 64,
    //     .target_type = 17
    // };
    
    // if (mem_collector.headers)
    // {
    //     dmi_table_decode_ext(buf, (u32)size, 0, 0, 0, 
    //                          collect_type_callback, &mem_collector);
    //     printf("Found %d memory device entries:\n", mem_collector.count);
    //     for (int i = 0; i < mem_collector.count; i++)
    //     {
    //         printf("  [%d] Handle: 0x%04X, Length: %d\n",
    //                i + 1, mem_collector.headers[i].handle, 
    //                mem_collector.headers[i].length);
    //     }
    //     free(mem_collector.headers);
    // }

    /* 清理 - read_file 分配的内存用 free 释放 */
    free(buf);

    return 0;
}
