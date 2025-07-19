# libnmcli - NetworkManager CLI Library

## 概述

libnmcli 是 NetworkManager 命令行工具 nmcli 的动态库版本。它提供了与 nmcli 相同的功能，但可以作为库被其他程序调用。

## 编译

### 编译动态库

```bash
# 在 NetworkManager 源码根目录
meson setup build
cd build
ninja
```

编译完成后，动态库文件将位于：
- 库文件：`/usr/local/lib/libnmcli/libnmcli.so`
- 头文件：`/usr/local/include/libnmcli/nmcli.h`
- pkg-config 文件：`/usr/local/lib/pkgconfig/libnmcli.pc`

### 编译示例程序

```bash
# 在 build 目录中
ninja nmcli-example
```

## API 接口

### 初始化函数

```c
gboolean nmcli_lib_init(void);
```
初始化 libnmcli 库。在使用其他函数之前必须调用此函数。

### 清理函数

```c
void nmcli_lib_cleanup(void);
```
清理 libnmcli 库资源。在程序结束前调用此函数。

### 执行命令

```c
int nmcli_execute(int argc, char **argv);
```
执行 nmcli 命令，参数格式与命令行相同。

**参数：**
- `argc`: 参数数量
- `argv`: 参数数组

**返回值：**
- 成功：`NMC_RESULT_SUCCESS` (0)
- 失败：其他错误代码

### 执行命令并捕获输出

```c
int nmcli_execute_with_output(int argc, char **argv, char **output, char **error);
```
执行 nmcli 命令并捕获标准输出和错误输出。

**参数：**
- `argc`: 参数数量
- `argv`: 参数数组
- `output`: 指向输出字符串的指针（调用者负责释放）
- `error`: 指向错误信息的指针（调用者负责释放）

**返回值：**
- 成功：`NMC_RESULT_SUCCESS` (0)
- 失败：其他错误代码

### 获取版本信息

```c
const char* nmcli_get_version(void);
```
获取库的版本字符串。

## 使用示例

### 基本用法

```c
#include <stdio.h>
#include "nmcli.h"

int main()
{
    // 初始化库
    if (!nmcli_lib_init()) {
        fprintf(stderr, "Failed to initialize nmcli library\n");
        return 1;
    }

    // 执行命令
    char *args[] = {"nmcli", "general", "status"};
    int result = nmcli_execute(3, args);
    
    if (result == NMC_RESULT_SUCCESS) {
        printf("Command executed successfully\n");
    } else {
        printf("Command failed with code: %d\n", result);
    }

    // 清理库
    nmcli_lib_cleanup();
    return 0;
}
```

### 捕获输出

```c
#include <stdio.h>
#include <string.h>
#include "nmcli.h"

int main()
{
    char *output = NULL;
    char *error = NULL;
    
    if (!nmcli_lib_init()) {
        return 1;
    }

    // 执行命令并捕获输出
    char *args[] = {"nmcli", "device", "status"};
    int result = nmcli_execute_with_output(3, args, &output, &error);
    
    if (result == NMC_RESULT_SUCCESS) {
        if (output && strlen(output) > 0) {
            printf("Output:\n%s\n", output);
        }
    } else {
        if (error && strlen(error) > 0) {
            printf("Error:\n%s\n", error);
        }
    }

    // 释放内存
    g_free(output);
    g_free(error);
    nmcli_lib_cleanup();
    return 0;
}
```

## 编译你的程序

### 使用 pkg-config

```bash
gcc -o myapp myapp.c $(pkg-config --cflags --libs libnmcli)
```

### 手动编译

```bash
gcc -o myapp myapp.c -I/usr/local/include/libnmcli -L/usr/local/lib/libnmcli -lnmcli

gcc -o nmcli-example ../clients/cli/nmcli-example.c -I../clients/cli $(pkg-config --cflags glib-2.0 gobject-2.0 libnm) -L./clients/cli -lnmcli $(pkg-config --libs glib-2.0 gobject-2.0 libnm)
```

## 错误代码

libnmcli 使用与 nmcli 相同的错误代码：

- `NMC_RESULT_SUCCESS` (0): 成功
- `NMC_RESULT_ERROR_UNKNOWN` (1): 未知错误
- `NMC_RESULT_ERROR_USER_INPUT` (2): 用户输入错误
- `NMC_RESULT_ERROR_TIMEOUT_EXPIRED` (3): 超时
- `NMC_RESULT_ERROR_CON_ACTIVATION` (4): 连接激活错误
- `NMC_RESULT_ERROR_CON_DEACTIVATION` (5): 连接停用错误
- `NMC_RESULT_ERROR_DEV_DISCONNECT` (6): 设备断开错误
- `NMC_RESULT_ERROR_CON_DEL` (7): 连接删除错误
- `NMC_RESULT_ERROR_NM_NOT_RUNNING` (8): NetworkManager 未运行
- `NMC_RESULT_ERROR_NOT_FOUND` (10): 未找到连接/设备/AP

## 注意事项

1. **初始化顺序**：在使用任何其他函数之前必须先调用 `nmcli_lib_init()`
2. **清理**：程序结束前必须调用 `nmcli_lib_cleanup()`
3. **内存管理**：使用 `nmcli_execute_with_output()` 时，调用者负责释放 `output` 和 `error` 参数
4. **线程安全**：当前版本不是线程安全的，不要在多个线程中同时使用
5. **依赖项**：程序需要链接 `libnm`、`glib-2.0` 和 `readline` 库

## 向后兼容性

为了保持向后兼容性，原始的 nmcli 可执行文件仍然会被编译为 `nmcli-bin`。 