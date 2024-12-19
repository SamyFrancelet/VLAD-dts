/**
 * @file pmic_startup.c
 * @author Samy Francelet (samy.francelet@ik.me)
 * @brief Startup file for the PMIC nPM1300
 * @version 0.1
 * @date 2024-12-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/mfd/npm1300.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(pmic_startup);

#define SYSREG_VBUSIN_BASE 0x02

#define SYSREG_TASKUPDATEILIMSW 0x00
#define SYSREG_VBUSINILIM0 0x01
#define SYSREG_VBUSINILIM_1000MA 0x0a

static int sysreg_setup(void)
{
	int err;
	uint8_t data;
	static const struct device *pmic = DEVICE_DT_GET(DT_NODELABEL(npm1300));

	if (!pmic) {
		LOG_ERR("Could not get PMIC device");
		return -ENODEV;
	}

	err = mfd_npm1300_reg_read_burst(pmic, SYSREG_VBUSIN_BASE, SYSREG_VBUSINILIM0, &data, 0x01);
	if (err < 0) {
		LOG_ERR("Could not read VBUSINILIM0 register (err: %d)", err);
		return err;
	}

	if (data == 0) {
		data = 5;
		LOG_INF("V_SYS current limit: %d mA\n", data * 100);
		return 0;
	}

    /* Write to MFD to set SYSREG current to 500mA */
    err = mfd_npm1300_reg_write(pmic, SYSREG_VBUSIN_BASE, SYSREG_VBUSINILIM0, 0x00);
	if (err < 0) {
		LOG_ERR("Could not write VBUSINILIM0 register (err: %d)", err);
		return err;
	}

	/* Save and update */
    err = mfd_npm1300_reg_write(pmic, SYSREG_VBUSIN_BASE, SYSREG_TASKUPDATEILIMSW, 0x01);
    if (err < 0) {
        LOG_ERR("Could not write TASKUPDATEILIMSW register (err: %d)", err);
        return err;
    }

	LOG_INF("V_SYS current limit: %d mA\n", 500);
	k_sleep(K_SECONDS(2));

	return 0;
}

SYS_INIT(sysreg_setup, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);