/*
 * DMI 系统路径定义
 * 复制自 dmidecode.c 中的宏定义，以便在扩展模块中使用
 */

#ifndef DMI_PATHS_H
#define DMI_PATHS_H

#define FLAG_NO_FILE_OFFSET     (1 << 0)
#define FLAG_STOP_AT_EOT        (1 << 1)
#define SYS_FIRMWARE_DIR "/sys/firmware/dmi/tables"
#define SYS_ENTRY_FILE   SYS_FIRMWARE_DIR "/smbios_entry_point"
#define SYS_TABLE_FILE   SYS_FIRMWARE_DIR "/DMI"

#endif /* DMI_PATHS_H */

