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
