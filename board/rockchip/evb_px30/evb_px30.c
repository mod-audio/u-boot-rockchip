/*
 * (C) Copyright 2017 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <misc.h>
#include <asm/io.h>
#include <u-boot/sha256.h>

DECLARE_GLOBAL_DATA_PTR;

#define PX30_CPUID_OFF  0x7
#define PX30_CPUID_LEN  0x10

static void setup_macaddr(void)
{
#if CONFIG_IS_ENABLED(CMD_NET)
	int ret;
	const char *cpuid = env_get("cpuid#");
	u8 hash[SHA256_SUM_LEN];
	int size = sizeof(hash);
	u8 mac_addr[6];

	/* Only generate a MAC address, if none is set in the environment */
	if (env_get("ethaddr"))
		return;

	if (!cpuid) {
		debug("%s: could not retrieve 'cpuid#'\n", __func__);
		return;
	}

	ret = hash_block("sha256", (void *)cpuid, strlen(cpuid), hash, &size);
	if (ret) {
		debug("%s: failed to calculate SHA256\n", __func__);
		return;
	}

	/* Copy 6 bytes of the hash to base the MAC address on */
	memcpy(mac_addr, hash, 6);

	/* Make this a valid MAC address and set it */
	mac_addr[0] &= 0xfe;  /* clear multicast bit */
	mac_addr[0] |= 0x02;  /* set local assignment bit (IEEE802) */
	eth_env_set_enetaddr("ethaddr", mac_addr);
#endif

	return;
}

static void setup_serial(void)
{
#if CONFIG_IS_ENABLED(ROCKCHIP_OTP)
	struct udevice *dev;
	int ret, i;
	u8 cpuid[PX30_CPUID_LEN];
	u8 low[PX30_CPUID_LEN/2], high[PX30_CPUID_LEN/2];
	char cpuid_str[PX30_CPUID_LEN * 2 + 1];
	u64 serialno;
	char serialno_str[20];

	/* retrieve the device */
	ret = uclass_get_device_by_driver(UCLASS_MISC,
					  DM_GET_DRIVER(rockchip_otp), &dev);
	if (ret) {
		debug("%s: could not find otp device\n", __func__);
		return;
	}

	/* read the cpu_id range from the otp */
	ret = misc_read(dev, PX30_CPUID_OFF, &cpuid, sizeof(cpuid));
	if (ret) {
		debug("%s: reading cpuid from the otp failed\n",
		      __func__);
		return;
	}

	memset(cpuid_str, 0, sizeof(cpuid_str));
	for (i = 0; i < 16; i++)
		sprintf(&cpuid_str[i * 2], "%02x", cpuid[i]);

	debug("cpuid: %s\n", cpuid_str);

	/*
	 * Mix the cpuid bytes using the same rules as in
	 *   ${linux}/drivers/soc/rockchip/rockchip-cpuinfo.c
	 */
	for (i = 0; i < 8; i++) {
		low[i] = cpuid[1 + (i << 1)];
		high[i] = cpuid[i << 1];
	}

	serialno = crc32_no_comp(0, low, 8);
	serialno |= (u64)crc32_no_comp(serialno, high, 8) << 32;
	snprintf(serialno_str, sizeof(serialno_str), "%llx", serialno);
	serialno_str[16] = '\0';

	env_set("cpuid#", cpuid_str);
	env_set("serial#", serialno_str);
#endif

	return;
}

int misc_init_r(void)
{
	setup_serial();
	setup_macaddr();

	return 0;
}
