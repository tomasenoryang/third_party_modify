git clone https://github.com/NetworkManager/NetworkManager.git
git checkout tags/1.22.10
sudo apt install meson ninja-build

meson setup build -Dnbft=false


sudo apt install libudev-dev libndp-dev

sudo apt install gobject-introspection libgirepository1.0-dev

sudo apt install libsystemd-dev

sudo apt install libaudit-dev


sudo apt install libpolkit-gobject-1-dev

sudo apt install libnss3-dev

sudo apt install ppp-dev

sudo apt install libmm-glib-dev

sudo apt install libjansson-dev

sudo apt install libpsl-dev

sudo apt install libcurl4-gnutls-dev

sudo apt install libreadline-dev

sudo apt install libnewt-dev

sudo apt install libnvme-dev

sudo apt install xsltproc

sudo apt-get install linux-libc-dev
sudo apt install intltool


meson setup build -Dqt=false


ninja -C build

find build -type f -name nmcli

meson setup build --reconfigure


ninja -C build

# 编译示例程序（不依赖glib）

## 动态库版本
gcc -o nmcli-example ./clients/cli/nmcli-example.c -I./clients/cli -L./build/clients/cli -lnmcli

## 静态库版本
gcc -o nmcli-example-static ./clients/cli/nmcli-example.c -I./clients/cli ./build/clients/cli/libnmcli_static.a -L./build/libnm -lnm -lglib-2.0 -lgobject-2.0 -lgio-2.0 -lreadline -lpthread

## C++版本（使用std::vector）
g++ -o nmcli-example-cpp ./clients/cli/nmcli-example.cpp -I./clients/cli ./build/clients/cli/libnmcli_static.a -L./build/libnm -lnm -lglib-2.0 -lgobject-2.0 -lgio-2.0 -lreadline -lpthread

**注意**: 已修复全局状态重置问题。现在`nmcli_execute`和`nmcli_execute_with_output`函数在每次调用时都会正确重置全局状态，确保输出格式的一致性。同时修复了C++编译警告，使用`const char*`和`const_cast`来正确处理字符串常量。

# 运行示例程序

## 运行动态库版本
export LD_LIBRARY_PATH=./build/clients/cli:$LD_LIBRARY_PATH && ./nmcli-example

## 运行静态库版本（不需要设置LD_LIBRARY_PATH）
./nmcli-example-static

## 运行C++版本（不需要设置LD_LIBRARY_PATH）
./nmcli-example-cpp

# 崩溃复现程序编译

## 基础崩溃复现程序
g++ -o crash_reproducer crash_reproducer.cpp -L./build/clients/cli -lnmcli -I./clients/cli

## 高级崩溃复现程序（触发缓冲区不足场景）
g++ -o crash_reproducer_advanced crash_reproducer_advanced.cpp -L./build/clients/cli -lnmcli -I./clients/cli -pthread

# 运行崩溃复现程序

## 运行基础版本
LD_LIBRARY_PATH=./build/clients/cli ./crash_reproducer

## 运行高级版本（已修复，不再崩溃）
LD_LIBRARY_PATH=./build/clients/cli ./crash_reproducer_advanced

## 使用 gdb 调试崩溃
LD_LIBRARY_PATH=./build/clients/cli gdb -batch -ex "run" -ex "bt" ./crash_reproducer_advanced

**注意**: 高级崩溃复现程序通过故意设置很小的缓冲区大小（10字节）来触发 `NMC_RESULT_ERROR_BUFFER_TOO_SMALL` 错误，从而复现多次调用 `nmcli_execute_with_output` 时的全局状态管理问题。

# 崩溃修复说明

## 问题描述
在多次调用 `nmcli_execute_with_output` 函数时，特别是在触发 `NMC_RESULT_ERROR_BUFFER_TOO_SMALL` 错误并重试的情况下，程序会发生段错误崩溃。

## 崩溃原因
1. **全局状态管理问题**: `nm_cli` 全局变量在 `nmc_cleanup` 后被清理，但没有正确重新初始化
2. **argv 指针错误**: `process_command_line` 函数中的 `next_arg` 调用会修改 `argv` 指针，但 `nmc_do_cmd` 函数仍使用原始的指针访问

## 修复方案
1. **在 `nmcli_execute_with_output` 中添加重新初始化代码**:
   ```c
   /* Reinitialize nm_cli structure */
   nm_cli.return_text = g_string_new (_("Success"));
   nm_cli.return_value = NMC_RESULT_SUCCESS;
   // ... 其他字段初始化
   ```

2. **修复 `nmc_do_cmd` 函数中的 argv 指针使用**:
   ```c
   // 修复前: nmc_arg_is_help (*(argv+1))
   // 修复后: nmc_arg_is_help (argv[0])
   ```

## 修复文件
- `clients/cli/nmcli.c`: 修复 `nmcli_execute_with_output` 函数
- `clients/cli/common.c`: 修复 `nmc_do_cmd` 函数

## 测试结果
修复后，程序可以正常处理多次调用和缓冲区不足的情况，不再发生崩溃。
