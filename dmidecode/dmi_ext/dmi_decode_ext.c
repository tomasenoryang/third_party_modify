/*
 * DMI Table Decode Extension - Callback-based API Implementation
 *
 * Copyright (C) 2025
 * This is an extension module for dmidecode.
 */

#include <stdio.h>
#include <string.h>
#include "dmi_decode_ext.h"

static const char *bad_index = "<BAD INDEX>";

/*
 * 实现 is_printable（原版在 dmidecode.c 中，与 main 一起无法单独链接）
 * 检查缓冲区是否只包含可打印 ASCII 字符
 */
int is_printable(const u8 *data, int len)
{
    int i;

    for (i = 0; i < len; i++)
        if (data[i] < 32 || data[i] >= 127)
            return 0;

    return 1;
}

/*
 * 实现 dmi_string（原版在 dmidecode.c 中，与 main 一起无法单独链接）
 * 从 SMBIOS 结构体中提取字符串
 */
const char *dmi_string(const struct dmi_header *dm, u8 s)
{
    char *bp = (char *)dm->data;
    size_t i, len;

    if (s == 0)
        return "Not Specified";

    bp += dm->length;
    while (s > 1 && *bp)
    {
        bp += strlen(bp);
        bp++;
        s--;
    }

    if (!*bp)
        return bad_index;

    /* ASCII filtering - 替换不可打印字符 */
    len = strlen(bp);
    for (i = 0; i < len; i++)
        if (bp[i] < 32 || bp[i] == 127)
            bp[i] = '.';

    return bp;
}

/*
 * 重新实现 to_dmi_header（原版是 static 函数，无法直接使用）
 * 逻辑与 dmidecode.c 中的 to_dmi_header 完全一致
 */
static void to_dmi_header_ext(struct dmi_header *h, u8 *data)
{
    h->type = data[0];
    h->length = data[1];
    h->handle = WORD(data + 2);
    h->data = data;
}

int dmi_table_decode_ext(u8 *buf, u32 len, u16 num, u16 ver, u32 flags,
                         dmi_header_callback_t callback, void *user_data)
{
    u8 *data;
    int i = 0;

    data = buf;
    while ((i < num || !num)
        && data + 4 <= buf + len) /* 4 is the length of an SMBIOS structure header */
    {
        u8 *next;
        struct dmi_header h;

        to_dmi_header_ext(&h, data);

        /*
         * If a short entry is found (less than 4 bytes), not only it
         * is invalid, but we cannot reliably locate the next entry.
         * Better stop at this point.
         */
        if (h.length < 4)
        {
            break;
        }
        i++;

        /* Look for the next handle */
        next = data + h.length;
        while ((unsigned long)(next - buf + 1) < len
            && (next[0] != 0 || next[1] != 0))
            next++;
        next += 2;

        /* Make sure the whole structure fits in the table */
        if ((unsigned long)(next - buf) > len)
        {
            break;
        }

        /* 调用用户回调函数 */
        if (callback)
        {
            int ret = callback(&h, data, user_data);
            if (ret != 0)
            {
                /* 用户请求停止解析 */
                break;
            }
        }

        data = next;

        /* SMBIOS v3 requires stopping at this marker */
        if (h.type == 127 && (flags & DMI_EXT_FLAG_STOP_AT_EOT))
            break;
    }

    return i;
}

/* 用于 find_by_type 的内部回调上下文 */
struct find_type_context {
    u8 target_type;
    struct dmi_header *out_header;
    int found;
};

static int find_type_callback(const struct dmi_header *h, const u8 *data, void *user_data)
{
    struct find_type_context *ctx = (struct find_type_context *)user_data;

    if (h->type == ctx->target_type)
    {
        *(ctx->out_header) = *h;
        ctx->found = 1;
        return 1; /* 停止解析 */
    }
    return 0;
}

int dmi_table_find_by_type(u8 *buf, u32 len, u16 num, u16 ver,
                           u8 target_type, struct dmi_header *out_header)
{
    struct find_type_context ctx;

    if (!out_header)
        return 0;

    ctx.target_type = target_type;
    ctx.out_header = out_header;
    ctx.found = 0;

    dmi_table_decode_ext(buf, len, num, ver, 0, find_type_callback, &ctx);

    return ctx.found;
}

/* 用于 find_by_handle 的内部回调上下文 */
struct find_handle_context {
    u16 target_handle;
    struct dmi_header *out_header;
    int found;
};

static int find_handle_callback(const struct dmi_header *h, const u8 *data, void *user_data)
{
    struct find_handle_context *ctx = (struct find_handle_context *)user_data;

    if (h->handle == ctx->target_handle)
    {
        *(ctx->out_header) = *h;
        ctx->found = 1;
        return 1; /* 停止解析 */
    }
    return 0;
}

int dmi_table_find_by_handle(u8 *buf, u32 len, u16 num, u16 ver,
                             u16 handle, struct dmi_header *out_header)
{
    struct find_handle_context ctx;

    if (!out_header)
        return 0;

    ctx.target_handle = handle;
    ctx.out_header = out_header;
    ctx.found = 0;

    dmi_table_decode_ext(buf, len, num, ver, 0, find_handle_callback, &ctx);

    return ctx.found;
}

