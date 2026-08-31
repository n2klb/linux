/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *  Driver for Hynitron cstxxx Touchscreen
 *
 *  Copyright (c) 2022 Chris Morgan <macromorgan@hotmail.com>
 *  Copyright (c) 2026 Jolla Mobile Ltd
 */

#ifndef HYNITRON_CSTXXX_H
#define HYNITRON_CSTXXX_H

#include <linux/gpio/consumer.h>
#include <linux/input.h>
#include <linux/input/touchscreen.h>

/* Data generic to all (supported and non-supported) controllers. */
struct hynitron_ts_data {
	const struct hynitron_ts_chip_data *chip;
	const struct hynitron_ts_bus_ops *bus_ops;
	struct device *dev;
	struct input_dev *input_dev;
	struct touchscreen_properties prop;
	struct gpio_desc *reset_gpio;
};

/* Per chip data */
struct hynitron_ts_chip_data {
	unsigned int max_touch_num;
	u32 ic_chkcode;
	int (*firmware_info)(struct hynitron_ts_data *ts_data);
	int (*bootloader_enter)(struct hynitron_ts_data *ts_data);
	void (*report_touch)(struct hynitron_ts_data *ts_data);
};

/* Interface-specific (I2C / SPI) operations */
struct hynitron_ts_bus_ops {
	int (*send)(struct hynitron_ts_data *ts_data, const u8 *buf, int len);
	int (*read)(struct hynitron_ts_data *ts_data, const u8 *cmd,
		    int cmd_len, u8 *val, int len);
};

int hyn_probe(struct device *dev, struct hynitron_ts_data *ts_data,
	      int irq, const struct hynitron_ts_bus_ops *bus_ops,
	      unsigned int bus_type);

/* Chips supported by core driver */
extern const struct hynitron_ts_chip_data cst3xx_data;
extern const struct hynitron_ts_chip_data cst66xx_data;

#endif
