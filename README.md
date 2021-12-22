### NOTE: support in upstream Buildroot
BeagleV Starlight and the StarFive JH7100 SoC has been merged into upstream buildroot.
[Instructions](https://github.com/buildroot/buildroot/blob/master/board/beaglev/readme.txt) are available in [buildroot master branch](https://github.com/buildroot/buildroot/blob/master/board/beaglev/). 


# Freelight U SDK #
This builds a complete RISC-V cross-compile toolchain for the StarFiveTech JH7100 SoC. It also builds U-boot and a flattened image tree (FIT)
image with a OpenSBI binary, linux kernel, device tree, ramdisk and rootdisk for the Starlight development board.

## Prerequisites ##

Recommend OS: Ubuntu 16.04/18.04

After installing the operating system.
Do not forget updating all packages

	$sudo apt update
	$sudo apt upgrade

Install required additional packages.

	$ sudo apt-get install autoconf automake autotools-dev bc bison xxd \
	build-essential curl flex gawk gdisk git gperf libgmp-dev \
	libmpc-dev libmpfr-dev libncurses-dev libssl-dev libtool \
	patchutils python screen texinfo unzip zlib1g-dev device-tree-compiler libyaml-dev

## Fetch code Instructions ##

Checkout this repository (the multimedia branch: `JH7100_starlight_multimedia`). Then you will need to checkout all of the linked
submodules using:

	$ git checkout --track origin/JH7100_starlight_multimedia
	$ git submodule update --init --recursive

This will take some time and require around 7GB of disk space. Some modules may
fail because certain dependencies don't have the best git hosting.

Once the submodules are initialized, 4 submodules `buildroot`, `HiFive_U-boot`,
`linux` and `opensbi` need checkout to corresponding branches manually, seeing `.gitmodule`

	$ cd buildroot && git checkout --track origin/starlight_multimedia && cd ..
	$ cd HiFive_U-Boot && git checkout --track origin/JH7100_Multimedia_V0.1.0 && cd ..
	$ cd linux && git checkout --track origin/beaglev-5.13.y_multimedia && cd ..
	$ cd opensbi && git checkout --track origin/master && cd ..

## Build Instructions ##

After update submodules, run `make` or `make -jx` and the complete toolchain and
fw_payload.bin.out & image.fit will be built. The completed build tree will consume about 18G of
disk space.

By default, the above generated image does not contain VPU driver module(wave511, the video hard decode driver and wave521, the video hard encode driver).  The following instructions will add VPU driver module according to your requirement:

	$ make -jx
	$ make vpudriver-build
	$ rm -rf work/buildroot_initramfs/images/rootfs.tar
	$ make -jx

Copy files fw_payload.bin.out and image.fit to tftp installation path to use

```
Path:
freelight-u-sdk/work/image.fit
freelight-u-sdk/work/opensbi/platform/generic/firmware/fw_payload.bin.out
```

The other make command:

```
make linux-menuconfig      # Kernel menuconfig
make uboot-menuconfig      # uboot menuconfig
make buildroot_initramfs-menuconfig   # initramfs menuconfig
make buildroot_rootfs-menuconfig      # rootfs menuconfig
```

## How to Switch Display Framework Between DRM and Framebuffer

The default display framework is `DRM` now.  Use `make linux-menuconfig`  follow below could change between `DRM` and `Framebuffer` framework

If switch from `DRM`to `Framebuffer` display framework (`hdmi` display device), 

```
1. Disable the DRM feature:
   CONFIG_DRM_I2C_NXP_TDA998X
   CONFIG_DRM_I2C_NXP_TDA9950
   CONFIG_DRM_STARFIVE

2. Enable the Framebuffer feature:
   CONFIG_FB_STARFIVE
   CONFIG_FB_STARFIVE_HDMI_TDA998X
   CONFIG_FB_STARFIVE_VIDEO

Note: Recommend Disable the below for usdk:
   CONFIG_NVDLA
   CONFIG_FRAMEBUFFER_CONSOLE
```

If switch from `Framebuffer` to `DRM` display framework ( `hdmi` display device):

```
1. Disable the below kernel config
   CONFIG_FB_STARFIVE
   CONFIG_FB_STARFIVE_HDMI_TDA998X
   CONFIG_FB_STARFIVE_VIDEO
   CONFIG_NVDLA
   CONFIG_FRAMEBUFFER_CONSOLE

2. Enable the below kernel config:
   CONFIG_DRM_I2C_NXP_TDA998X
   CONFIG_DRM_I2C_NXP_TDA9950
   CONFIG_DRM_STARFIVE

Note: when use DRM to hdmi pipeline, please make sure CONFIG_DRM_STARFIVE_MIPI_DSI is disable, or will cause color abnormal.
```

If switch  from `Framebuffer`  to `DRM` display framework (`mipi dsi` display device):

```
based on the above drm to hdmi pipeline config, enable the below kernel config:
CONFIG_PHY_M31_DPHY_RX0
CONFIG_DRM_STARFIVE_MIPI_DSI
```

## How to Support WM8960 and AC108 Audio Board 

The starlight board natively always support PWMDAC  to audio out, also support WM8960 board to audio in and audio out, also support AC108 board to audio in. Note that the WM8960 and AC108 could not be both supported.

 If support WM8960 board, need to modify the follow:

> HiFive_U-Boot/board/starfive/jh7100/jh7100.c:
>
> > #define STARFIVE_AUDIO_AC108    0
> > #define STARFIVE_AUDIO_WM8960    1
> > #define STARFIVE_AUDIO_VAD        0
> > #define STARFIVE_AUDIO_PWMDAC    1
> > #define STARFIVE_AUDIO_SPDIF    0
> > #define STARFIVE_AUDIO_PDM        0
>
> HiFive_U-Boot/arch/riscv/dts/jh7100-beaglev-starlight.dts:
>
> > /* #include "codecs/sf_pdm.dtsi" \*/
> > /\* #include "codecs/sf_spdif.dtsi" \*/
> > /\* #include "codecs/sf_ac108.dtsi" \*/
> > #include "codecs/sf_wm8960.dtsi"
> > /\* #include "codecs/sf_vad.dtsi" \*/

 If support AC108 board, need to modify the follow:

> HiFive_U-Boot/board/starfive/jh7100/jh7100.c:
>
> > #define STARFIVE_AUDIO_AC108    1
> > #define STARFIVE_AUDIO_WM8960    0
> > #define STARFIVE_AUDIO_VAD        0
> > #define STARFIVE_AUDIO_PWMDAC    1
> > #define STARFIVE_AUDIO_SPDIF    0
> > #define STARFIVE_AUDIO_PDM        0
>
> HiFive_U-Boot/arch/riscv/dts/jh7100-beaglev-starlight.dts:
>
> > /* #include "codecs/sf_pdm.dtsi" \*/
> > /\* #include "codecs/sf_spdif.dtsi" \*/
> > #include "codecs/sf_ac108.dtsi"
> > /\*#include "codecs/sf_wm8960.dtsi" \*/
> > /\* #include "codecs/sf_vad.dtsi" \*/

## Build TF Card Booting Image

If you don't already use a local tftp server, then you probably want to make the sdcard target; the default size is 16 GBs. NOTE THIS WILL DESTROY ALL EXISTING DATA on the target sdcard; please modify the following file.

conf/beaglev_defconfig_513:

```
change
CONFIG_CMDLINE="earlyprintk console=tty1 console=ttyS0,115200 debug rootwait stmmaceth=chain_mode:1"
to
CONFIG_CMDLINE="earlyprintk console=tty1 console=ttyS0,115200 debug rootwait stmmaceth=chain_mode:1 root=/dev/mmcblk0p3"
```

HiFive_U-Boot/configs/starfive_jh7100_starlight_smode_defconfig:

```
change
CONFIG_USE_BOOTCOMMAND is not set
to
CONFIG_USE_BOOTCOMMAND=y

change
#CONFIG_BOOTCOMMAND="run mmcsetup; run fdtsetup; run fatenv; echo 'running boot2...'; run boot2"
to
CONFIG_BOOTCOMMAND="run mmcsetup; run fdtsetup; run fatenv; echo 'running boot2...'; run boot2"
```

Please insert the TF card and run command `df -h` to check the device name `/dev/sdXX`, then run command `umount /dev/sdXX`",  then run the following instructions to build TF card image:

```
make buildroot_rootfs -jx
make -jx
make vpubuild_rootfs
make clean
make -jx
make buildroot_rootfs -jx
make DISK=/dev/sdX format-nvdla-rootfs && sync
```
Note: please also do not forget to update the `fw_payload.bin.out` which is built by this step.


## Running on Starlight Board ##

After the Starlight is properly connected to the serial port cable, network cable and power cord turn on the power from the wall power socket to power on the Starlight and you will see the startup information as follows:

	bootloader version: 210209-4547a8d
	ddr 0x00000000, 1M test
	ddr 0x00100000, 2M test
	DDR clk 2133M,Version: 210302-5aea32f
	2
Press any key as soon as it starts up to enter the **upgrade menu**. In this menu, you can update uboot

	bootloader version: 210209-4547a8d
	ddr 0x00000000, 1M test
	ddr 0x00100000, 2M test
	DDR clk 2133M,Version: 210302-5aea32f
	0
	xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	xxxxxxxxxxxFLASH PROGRAMMINGxxxxxxxxxxxxxxx
	xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	
	0:update boot
	1:quit
	select the function:

Type **"0"**  to update the uboot file fw_payload.bin.out via Xmodem mode,
and then Type **"1"** to exit Flash Programming.

After that, you will see the information `Starlight #` when press the "f" button, select the installation path
and install image.fit through TFTP:

Step1: set enviroment parameter:

	setenv bootfile vmlinuz;setenv fdt_addr_r 0x88000000;setenv fdt_high 0xffffffffffffffff;setenv fdtcontroladdr 0xffffffffffffffff;setenv initrd_high 0xffffffffffffffff;setenv kernel_addr_r 0x84000000;setenv fileaddr a0000000;setenv ipaddr 192.168.xxx.xxx;setenv serverip 192.168.xxx.xxx

Step2: upload image file to ddr:

	tftpboot ${fileaddr} ${serverip}:image.fit;

Step3: load and excute:	

	bootm start ${fileaddr};bootm loados ${fileaddr};booti 0x80200000 0x86100000:${filesize} 0x86000000

When you see the `buildroot login:` message, then congratulations, the launch was successful

	buildroot login:root
	Password: starfive

