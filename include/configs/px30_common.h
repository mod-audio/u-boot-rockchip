/*
 * (C) Copyright 2017 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef __CONFIG_PX30_COMMON_H
#define __CONFIG_PX30_COMMON_H

#include "rockchip-common.h"

#define CONFIG_SYS_MALLOC_LEN		(192 << 20) /* 192M */
#define CONFIG_SYS_CBSIZE		1024
#define CONFIG_SKIP_LOWLEVEL_INIT

#define CONFIG_SPL_FRAMEWORK

#define CONFIG_SYS_NS16550_MEM32

#define CONFIG_SYS_TEXT_BASE		0x00200000
#define CONFIG_SYS_INIT_SP_ADDR		0x00400000
#define CONFIG_SYS_LOAD_ADDR		0x00800800
#define CONFIG_SPL_STACK		0x00400000
#define CONFIG_SPL_TEXT_BASE		0x00000000
#define CONFIG_SPL_MAX_SIZE		0x40000
#define CONFIG_SPL_BSS_START_ADDR	0x2000000
#define CONFIG_SPL_BSS_MAX_SIZE		0x2000
#define CONFIG_SYS_BOOTM_LEN		(64 << 20)	/* 64M */

#define COUNTER_FREQUENCY		24000000

#define GICD_BASE			0xff131000
#define GICC_BASE			0xff132000

#define CONFIG_SYS_BOOTM_LEN	(64 << 20)	/* 64M */

/* MMC/SD IP block */
#define CONFIG_BOUNCE_BUFFER

#define CONFIG_SYS_SDRAM_BASE		0
#define SDRAM_MAX_SIZE			0xff000000
#define SDRAM_BANK_SIZE			(2UL << 30)
#ifdef CONFIG_DM_DVFS
#define CONFIG_PREBOOT			"dvfs repeat"
#else
#define CONFIG_PREBOOT
#endif

#ifndef CONFIG_SPL_BUILD

/* usb mass storage */
#define CONFIG_USB_FUNCTION_MASS_STORAGE
#define CONFIG_ROCKUSB_G_DNL_PID        0x330d

#ifdef CONFIG_ARM64
#define ENV_MEM_LAYOUT_SETTINGS \
	"scriptaddr=0x00500000\0" \
	"pxefile_addr_r=0x00600000\0" \
	"fdt_addr_r=0x08300000\0" \
	"kernel_addr_r=0x00280000\0" \
	"kernel_addr_c=0x03e80000\0" \
	"ramdisk_addr_r=0x0a200000\0"
#else
#define ENV_MEM_LAYOUT_SETTINGS \
	"scriptaddr=0x00000000\0" \
	"pxefile_addr_r=0x00100000\0" \
	"fdt_addr_r=0x08300000\0" \
	"kernel_addr_r=0x02008000\0" \
	"ramdisk_addr_r=0x0a200000\0"
#endif

#include <config_distro_bootcmd.h>
#define CONFIG_EXTRA_ENV_SETTINGS \
	ENV_MEM_LAYOUT_SETTINGS \
	"partitions=" PARTS_DEFAULT \
	ROCKCHIP_DEVICE_SETTINGS \
	RKIMG_DET_BOOTDEV \
	BOOTENV

#endif

/* rockchip ohci host driver */
#define CONFIG_USB_OHCI_NEW
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS	1

#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	"devtype=mmc\0" \
	"devnum=0\0" \
	"scriptaddr=0x00500000\0" \
	"fdt_addr_r=0x08300000\0" \
	"kernel_addr_r=0x00280000\0" \
	"bootdelay=1\0" \
	"bootcmd=if test -e mmc 0:5 /boot-restore; then run boot_restore; else run boot_main; fi; run boot_restore; run boot_maskrom\0" \
	"console=ttyS2,115200n8\0" \
	"loglevel=8\0" \
	"bootenv=/uEnv-ff-px30.txt\0" \
	"kernel=/Image-ff-px30\0" \
	"fdtbin=/moddwarf-ff-px30.dtb\0" \
	"boot_image=booti ${kernel_addr_r} - ${fdt_addr_r}\0" \
	"setbootargs=setenv bootargs console=${console} init=/sbin/init root=${root} loglevel=${loglevel} ${extraargs}\0" \
	"main_bootargs=setenv root \"/dev/mmcblk0p4\"\0" \
	"main_loadbootenv=ext4load mmc 0:4 ${scriptaddr} /boot${bootenv}; env import ${scriptaddr} ${filesize}\0" \
	"main_script=ext4load mmc 0:4 ${fdt_addr_r} /boot${fdtbin}\0" \
	"main_kernel=ext4load mmc 0:4 ${kernel_addr_r} /boot${kernel}\0" \
	"boot_main=run main_bootargs main_loadbootenv main_script main_kernel setbootargs boot_image\0" \
	"restore_bootargs=setenv root \"\"\0" \
	"restore_loadbootenv=fatload mmc 0:3 ${scriptaddr} ${bootenv}; env import ${scriptaddr} ${filesize}\0" \
	"restore_script=fatload mmc 0:3 ${fdt_addr_r} ${fdtbin}\0" \
	"restore_kernel=fatload mmc 0:3 ${kernel_addr_r} ${kernel}\0" \
	"boot_restore=run restore_bootargs restore_loadbootenv restore_script restore_kernel setbootargs boot_image\0" \
	"boot_maskrom=echo hmi:maskrom mode active; echo hmi:maskrom mode active; rockusb 0 mmc 0\0" \
	"loadbootenv=echo\0"

#endif
