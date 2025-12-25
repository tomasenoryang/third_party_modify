/*
 * DMI Table Decode Extension - Callback-based API
 * 在不修改原 dmidecode 文件的情况下，提供获取 dmi_header 的能力
 *
 * Copyright (C) 2025
 * This is an extension module for dmidecode.
 */

#ifndef DMI_DECODE_EXT_H
#define DMI_DECODE_EXT_H

/* 引用原有 dmidecode 头文件 */
#include "types.h"
#include "dmidecode.h"

/* 标志位定义 */
#define DMI_EXT_FLAG_STOP_AT_EOT (1 << 1)

/*
 * 回调函数类型定义
 * 参数:
 *   h         - 指向当前解析到的 dmi_header
 *   data      - 指向该结构体的原始数据
 *   user_data - 用户传入的上下文指针
 * 返回值:
 *   0  - 继续解析
 *   非0 - 停止解析
 */
typedef int (*dmi_header_callback_t)(const struct dmi_header *h,
                                      const u8 *data,
                                      void *user_data);

/*
 * 带回调的 DMI 表解码函数
 * 参数:
 *   buf       - DMI 表数据缓冲区
 *   len       - 缓冲区长度
 *   num       - 预期的结构体数量 (0 表示未知)
 *   ver       - SMBIOS 版本
 *   flags     - 标志位 (DMI_EXT_FLAG_*)
 *   callback  - 每解析到一个 header 时调用的回调函数
 *   user_data - 传递给回调函数的用户数据
 * 返回值:
 *   实际解析的结构体数量
 */
int dmi_table_decode_ext(u8 *buf, u32 len, u16 num, u16 ver, u32 flags,
                         dmi_header_callback_t callback, void *user_data);

/*
 * 查找指定类型的第一个 DMI header
 * 参数:
 *   buf         - DMI 表数据缓冲区
 *   len         - 缓冲区长度
 *   num         - 预期的结构体数量 (0 表示未知)
 *   ver         - SMBIOS 版本
 *   target_type - 要查找的 DMI 类型
 *   out_header  - 输出参数，找到的 header 将复制到这里
 * 返回值:
 *   1 - 找到
 *   0 - 未找到
 */
int dmi_table_find_by_type(u8 *buf, u32 len, u16 num, u16 ver,
                           u8 target_type, struct dmi_header *out_header);

/*
 * 查找指定 handle 的 DMI header
 * 参数:
 *   buf         - DMI 表数据缓冲区
 *   len         - 缓冲区长度
 *   num         - 预期的结构体数量 (0 表示未知)
 *   ver         - SMBIOS 版本
 *   handle      - 要查找的 handle
 *   out_header  - 输出参数，找到的 header 将复制到这里
 * 返回值:
 *   1 - 找到
 *   0 - 未找到
 */
int dmi_table_find_by_handle(u8 *buf, u32 len, u16 num, u16 ver,
                             u16 handle, struct dmi_header *out_header);

#endif /* DMI_DECODE_EXT_H */

