# libnmcli 修改总结

## 概述

本次修改将 NetworkManager 的 nmcli 工具从可执行文件改为动态库，同时保持向后兼容性。

## 修改的文件

### 1. `nmcli.h`
- 添加了动态库的公共API接口声明
- 新增函数：
  - `nmcli_lib_init()`: 初始化库
  - `nmcli_lib_cleanup()`: 清理库
  - `nmcli_execute()`: 执行命令
  - `nmcli_execute_with_output()`: 执行命令并捕获输出
  - `nmcli_get_version()`: 获取版本信息

### 2. `nmcli.c`
- 添加了动态库API的实现
- 保持了原有的main函数功能
- 新增了库初始化和清理逻辑
- 添加了输出捕获功能

### 3. `meson.build`
- 将 `executable` 改为 `shared_library`
- 添加了版本信息和安装配置
- 创建了pkg-config文件
- 保留了原始可执行文件（命名为 `nmcli-bin`）
- 添加了示例程序的编译配置

### 4. `nmcli-example.c` (新增)
- 创建了示例程序，演示如何使用libnmcli
- 包含基本用法和输出捕获示例

### 5. `README-libnmcli.md` (新增)
- 详细的使用文档
- API接口说明
- 编译和使用示例
- 错误代码说明

### 6. `test-libnmcli.sh` (新增)
- 自动化测试脚本
- 验证编译和使用功能

### 7. `Makefile` (新增)
- 简单的Makefile，用于直接编译
- 包含安装和卸载目标

## 编译方法

### 使用 meson (推荐)
```bash
meson setup build
cd build
ninja
```

### 使用 Makefile
```bash
cd clients/cli
make
```

## 安装

### 使用 meson
```bash
cd build
sudo ninja install
```

### 使用 Makefile
```bash
cd clients/cli
sudo make install
```

## 使用示例

### 基本用法
```c
#include "nmcli.h"

int main() {
    if (!nmcli_lib_init()) {
        return 1;
    }
    
    char *args[] = {"nmcli", "general", "status"};
    int result = nmcli_execute(3, args);
    
    nmcli_lib_cleanup();
    return result;
}
```

### 捕获输出
```c
char *output = NULL, *error = NULL;
char *args[] = {"nmcli", "device", "status"};
int result = nmcli_execute_with_output(3, args, &output, &error);

if (output) printf("Output: %s\n", output);
if (error) printf("Error: %s\n", error);

g_free(output);
g_free(error);
```

## 向后兼容性

- 原始的 nmcli 可执行文件仍然可用，命名为 `nmcli-bin`
- 所有原有的 nmcli 功能都保持不变
- 新的动态库提供了相同的功能，但可以作为库使用

## 文件结构

编译后的文件结构：
```
/usr/local/
├── lib/
│   └── libnmcli/
│       ├── libnmcli.so.1.0.0
│       ├── libnmcli.so.1 -> libnmcli.so.1.0.0
│       └── libnmcli.so -> libnmcli.so.1
├── include/
│   └── libnmcli/
│       └── nmcli.h
└── lib/
    └── pkgconfig/
        └── libnmcli.pc
```

## 测试

运行测试脚本：
```bash
cd clients/cli
./test-libnmcli.sh
```

## 注意事项

1. 需要安装 NetworkManager 开发包
2. 依赖 libnm、glib-2.0 和 readline 库
3. 当前版本不是线程安全的
4. 使用前必须调用 `nmcli_lib_init()`
5. 使用后必须调用 `nmcli_lib_cleanup()` 