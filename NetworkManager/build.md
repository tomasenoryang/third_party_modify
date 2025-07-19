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


ninja -C ../build && gcc -o nmcli-example ../clients/cli/nmcli-example.c -I../clients/cli $(pkg-config --cflags glib-2.0 gobject-2.0 libnm) -L./clients/cli -lnmcli $(pkg-config --libs glib-2.0 gobject-2.0 libnm) && export LD_LIBRARY_PATH=./clients/cli:$LD_LIBRARY_PATH && ./nmcli-example

错误：无法创建 NMClient 对象：发送数据出错：断开的管道。
