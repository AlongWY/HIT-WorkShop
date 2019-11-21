SDK_PATH="/opt/ti-processor-sdk/linux-devkit"
SDK_SYS=i686-arago-linux
REAL_MULTIMACH_TARGET_SYS=cortexa9t2hf-vfp-neon-linux-gnueabi
TOOLCHAIN_SYS=arm-linux-gnueabihf
TOOLCHAIN_PREFIX=$(TOOLCHAIN_SYS)-
SDK_PATH_NATIVE=$(SDK_PATH)/sysroots/$(SDK_SYS)
SDK_PATH_TARGET=$(SDK_PATH)/sysroots/$(REAL_MULTIMACH_TARGET_SYS)
PATH=$(SDK_PATH_NATIVE)/usr/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/opt/ti-processor-sdk/linux-devkit/sysroots/i686-arago-linux/usr/bin
CPATH=$(SDK_PATH_TARGET)/usr/include:$(CPATH)
PKG_CONFIG_SYSROOT_DIR=$(SDK_PATH_TARGET)
PKG_CONFIG_PATH=$(SDK_PATH_TARGET)/usr/lib/pkgconfig
PKG_CONFIG_ALLOW_SYSTEM_LIBS=1
CONFIG_SITE=$(SDK_PATH)/site-config-$(REAL_MULTIMACH_TARGET_SYS)
CC=${TOOLCHAIN_PREFIX}gcc
CXX=${TOOLCHAIN_PREFIX}g++
GDB=${TOOLCHAIN_PREFIX}gdb
CPP="${TOOLCHAIN_PREFIX}gcc -E"
NM=${TOOLCHAIN_PREFIX}nm
AS=${TOOLCHAIN_PREFIX}as
AR=${TOOLCHAIN_PREFIX}ar
RANLIB=${TOOLCHAIN_PREFIX}ranlib
OBJCOPY=${TOOLCHAIN_PREFIX}objcopy
OBJDUMP=${TOOLCHAIN_PREFIX}objdump
STRIP=${TOOLCHAIN_PREFIX}strip
CONFIGURE_FLAGS=--target=arm-linux-gnueabi --host=arm-linux-gnueabi --build=i686-linux --with-libtool-sysroot=$(SDK_PATH_TARGET)
CPPFLAGS=-march=armv7-a -marm -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9 --sysroot=$(SDK_PATH_TARGET)
CFLAGS=$(CPPFLAGS)
CXXFLAGS=$(CPPFLAGS)
LDFLAGS=--sysroot=$(SDK_PATH_TARGET)
OECORE_NATIVE_SYSROOT=$(SDK_PATH_NATIVE)
OECORE_TARGET_SYSROOT=$(SDK_PATH_TARGET)
OECORE_ACLOCAL_OPTS="-I $(SDK_PATH_NATIVE)/usr/share/aclocal"
OECORE_DISTRO_VERSION="2015.05"
OECORE_SDK_VERSION="2015.05"

all: led uart


led:
	$(CC) $(CFLAGS) $(LDFLAGS) led.c -o led

uart:
	$(CC) $(CFLAGS) $(LDFLAGS) uart.c -o uart

clean:
	rm led uart



