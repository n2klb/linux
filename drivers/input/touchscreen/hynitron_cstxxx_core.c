// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Driver for Hynitron cstxxx Touchscreen
 *
 *  Copyright (c) 2022 Chris Morgan <macromorgan@hotmail.com>
 *
 *  This code is based on hynitron_core.c authored by Hynitron.
 *  Note that no datasheet was available, so much of these registers
 *  are undocumented. This is essentially a cleaned-up version of the
 *  vendor driver with support removed for hardware I cannot test and
 *  device-specific functions replated with generic functions wherever
 *  possible.
 */

#include <linux/delay.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/input/mt.h>
#include <linux/module.h>
#include <linux/property.h>
#include <linux/unaligned.h>

#include "hynitron_cstxxx.h"

/*
 * Since I have no datasheet, these values are guessed and/or assumed
 * based on observation and testing.
 */
#define CST3XX_FIRMWARE_INFO_START_CMD		0x01d1
#define CST3XX_FIRMWARE_INFO_END_CMD		0x09d1
#define CST3XX_FIRMWARE_CHK_CODE_REG		0xfcd1
#define CST3XX_FIRMWARE_VERSION_REG		0x08d2
#define CST3XX_FIRMWARE_VER_INVALID_VAL		0xa5a5a5a5

#define CST3XX_BOOTLDR_PROG_CMD			0xaa01a0
#define CST3XX_BOOTLDR_PROG_CHK_REG		0x02a0
#define CST3XX_BOOTLDR_CHK_VAL			0xac

#define CST3XX_TOUCH_DATA_PART_REG		0x00d0
#define CST3XX_TOUCH_DATA_FULL_REG		0x07d0
#define CST3XX_TOUCH_DATA_CHK_VAL		0xab
#define CST3XX_TOUCH_DATA_TOUCH_VAL		0x03
#define CST3XX_TOUCH_DATA_STOP_CMD		0xab00d0
#define CST3XX_TOUCH_COUNT_MASK			GENMASK(6, 0)

/*
 * Hard coded reset delay value of 20ms not IC dependent in
 * vendor driver.
 */
static void hyn_reset_proc(struct hynitron_ts_data *ts_data, int delay)
{
	gpiod_set_value_cansleep(ts_data->reset_gpio, 1);
	msleep(20);
	gpiod_set_value_cansleep(ts_data->reset_gpio, 0);
	if (delay)
		fsleep(delay * 1000);
}

static irqreturn_t hyn_interrupt_handler(int irq, void *dev_id)
{
	struct hynitron_ts_data *ts_data = dev_id;

	ts_data->chip->report_touch(ts_data);

	return IRQ_HANDLED;
}

static void hyn_report_contact(struct hynitron_ts_data *ts_data, u8 id,
			       unsigned int x, unsigned int y, u8 w)
{
	input_mt_slot(ts_data->input_dev, id);
	input_mt_report_slot_state(ts_data->input_dev, MT_TOOL_FINGER, 1);
	touchscreen_report_pos(ts_data->input_dev, &ts_data->prop, x, y, true);
	input_report_abs(ts_data->input_dev, ABS_MT_TOUCH_MAJOR, w);
}

static int cst3xx_read_register(struct hynitron_ts_data *ts_data, u16 reg,
				u8 *val, u16 len)
{
	__le16 buf = cpu_to_le16(reg);
	int err;

	err = ts_data->bus_ops->read(ts_data, (u8 *)&buf, 2, val, len);
	if (err) {
		dev_err(ts_data->dev,
			"Error reading %d bytes from 0x%04x: %d\n",
			len, reg, err);
		return err;
	}

	return 0;
}

static int cst3xx_firmware_info(struct hynitron_ts_data *ts_data)
{
	int err;
	u32 tmp;
	u8 buf[4];

	/*
	 * Tests suggest this command needed to read firmware regs.
	 */
	put_unaligned_le16(CST3XX_FIRMWARE_INFO_START_CMD, buf);
	err = ts_data->bus_ops->send(ts_data, buf, 2);
	if (err)
		return err;

	usleep_range(10000, 11000);

	/*
	 * Read register for check-code to determine if device detected
	 * correctly.
	 */
	err = cst3xx_read_register(ts_data, CST3XX_FIRMWARE_CHK_CODE_REG,
				   buf, 4);
	if (err)
		return err;

	tmp = get_unaligned_le32(buf);
	if ((tmp & 0xffff0000) != ts_data->chip->ic_chkcode) {
		dev_err(ts_data->dev, "%s ic mismatch, chkcode is %u\n",
			__func__, tmp);
		return -ENODEV;
	}

	usleep_range(10000, 11000);

	/* Read firmware version and test if firmware missing. */
	err = cst3xx_read_register(ts_data, CST3XX_FIRMWARE_VERSION_REG,
				   buf, 4);
	if (err)
		return err;

	tmp = get_unaligned_le32(buf);
	if (tmp == CST3XX_FIRMWARE_VER_INVALID_VAL) {
		dev_err(ts_data->dev, "Device firmware missing\n");
		return -ENODEV;
	}

	/*
	 * Tests suggest cmd required to exit reading firmware regs.
	 */
	put_unaligned_le16(CST3XX_FIRMWARE_INFO_END_CMD, buf);
	err = ts_data->bus_ops->send(ts_data, buf, 2);
	if (err)
		return err;

	usleep_range(5000, 6000);

	return 0;
}

static int cst3xx_bootloader_enter(struct hynitron_ts_data *ts_data)
{
	int err;
	u8 retry;
	u32 tmp = 0;
	u8 buf[3];

	for (retry = 0; retry < 5; retry++) {
		hyn_reset_proc(ts_data, (7 + retry));
		/* set cmd to enter program mode */
		put_unaligned_le24(CST3XX_BOOTLDR_PROG_CMD, buf);
		err = ts_data->bus_ops->send(ts_data, buf, 3);
		if (err)
			continue;

		usleep_range(2000, 2500);

		/* check whether in program mode */
		err = cst3xx_read_register(ts_data, CST3XX_BOOTLDR_PROG_CHK_REG,
					   buf, 1);
		if (err)
			continue;

		tmp = get_unaligned(buf);
		if (tmp == CST3XX_BOOTLDR_CHK_VAL)
			break;
	}

	if (tmp != CST3XX_BOOTLDR_CHK_VAL) {
		dev_err(ts_data->dev, "%s unable to enter bootloader mode\n",
			__func__);
		return -ENODEV;
	}

	hyn_reset_proc(ts_data, 40);

	return 0;
}

static int cst3xx_finish_touch_read(struct hynitron_ts_data *ts_data)
{
	u8 buf[3];
	int err;

	put_unaligned_le24(CST3XX_TOUCH_DATA_STOP_CMD, buf);
	err = ts_data->bus_ops->send(ts_data, buf, 3);
	if (err) {
		dev_err(ts_data->dev,
			"send read touch info ending failed: %d\n", err);
		return err;
	}

	return 0;
}

/*
 * Handle events from IRQ. Note that for cst3xx it appears that IRQ
 * fires continuously while touched, otherwise once every 1500ms
 * when not touched (assume touchscreen waking up periodically).
 * Note buffer is sized for 5 fingers, if more needed buffer must
 * be increased. The buffer contains 5 bytes for each touch point,
 * a touch count byte, a check byte, and then a second check byte after
 * all other touch points.
 *
 * For example 1 touch would look like this:
 * touch1[5]:touch_count[1]:chk_byte[1]
 *
 * 3 touches would look like this:
 * touch1[5]:touch_count[1]:chk_byte[1]:touch2[5]:touch3[5]:chk_byte[1]
 */
static void cst3xx_touch_report(struct hynitron_ts_data *ts_data)
{
	u8 buf[28];
	u8 finger_id, sw, w;
	unsigned int x, y;
	unsigned int touch_cnt, end_byte;
	unsigned int idx = 0;
	unsigned int i;
	int err;

	/* Read and validate the first bits of input data. */
	err = cst3xx_read_register(ts_data, CST3XX_TOUCH_DATA_PART_REG,
				   buf, 28);
	if (err ||
	    buf[6] != CST3XX_TOUCH_DATA_CHK_VAL ||
	    buf[0] == CST3XX_TOUCH_DATA_CHK_VAL) {
		dev_err(ts_data->dev, "cst3xx touch read failure\n");
		return;
	}

	/* Report to the device we're done reading the touch data. */
	err = cst3xx_finish_touch_read(ts_data);
	if (err)
		return;

	touch_cnt = buf[5] & CST3XX_TOUCH_COUNT_MASK;
	if (touch_cnt > ts_data->chip->max_touch_num) {
		dev_err(ts_data->dev, "cst3xx invalid touch count (%d vs %d max)\n",
			touch_cnt, ts_data->chip->max_touch_num);
		return;
	}

	/*
	 * Check the check bit of the last touch slot. The check bit is
	 * always present after touch point 1 for valid data, and then
	 * appears as the last byte after all other touch data.
	 */
	if (touch_cnt > 1) {
		end_byte = touch_cnt * 5 + 2;
		if (buf[end_byte] != CST3XX_TOUCH_DATA_CHK_VAL) {
			dev_err(ts_data->dev, "cst3xx touch read failure\n");
			return;
		}
	}

	/* Parse through the buffer to capture touch data. */
	for (i = 0; i < touch_cnt; i++) {
		x = ((buf[idx + 1] << 4) | ((buf[idx + 3] >> 4) & 0x0f));
		y = ((buf[idx + 2] << 4) | (buf[idx + 3] & 0x0f));
		w = (buf[idx + 4] >> 3);
		sw = (buf[idx] & 0x0f) >> 1;
		finger_id = (buf[idx] >> 4) & 0x0f;

		/* Sanity check we don't have more fingers than we expect */
		if (finger_id >= ts_data->chip->max_touch_num) {
			dev_err(ts_data->dev,
				"cst3xx invalid finger id %d\n", finger_id);
			return;
		}

		/* sw value of 0 means no touch, 0x03 means touch */
		if (sw == CST3XX_TOUCH_DATA_TOUCH_VAL)
			hyn_report_contact(ts_data, finger_id, x, y, w);

		idx += 5;

		/* Skip the 2 bytes between point 1 and point 2 */
		if (i == 0)
			idx += 2;
	}

	input_mt_sync_frame(ts_data->input_dev);
	input_sync(ts_data->input_dev);
}

static int hyn_input_dev_init(struct hynitron_ts_data *ts_data, unsigned int bus_type)
{
	int err;

	ts_data->input_dev = devm_input_allocate_device(ts_data->dev);
	if (!ts_data->input_dev) {
		dev_err(ts_data->dev, "Failed to allocate input device\n");
		return -ENOMEM;
	}

	ts_data->input_dev->name = "Hynitron cstxxx Touchscreen";
	ts_data->input_dev->phys = "input/ts";
	ts_data->input_dev->id.bustype = bus_type;

	input_set_drvdata(ts_data->input_dev, ts_data);

	input_set_capability(ts_data->input_dev, EV_ABS, ABS_MT_POSITION_X);
	input_set_capability(ts_data->input_dev, EV_ABS, ABS_MT_POSITION_Y);
	input_set_abs_params(ts_data->input_dev, ABS_MT_TOUCH_MAJOR,
			     0, 255, 0, 0);

	touchscreen_parse_properties(ts_data->input_dev, true, &ts_data->prop);

	if (!ts_data->prop.max_x || !ts_data->prop.max_y) {
		dev_err(ts_data->dev,
			"Invalid x/y (%d, %d), using defaults\n",
			ts_data->prop.max_x, ts_data->prop.max_y);
		ts_data->prop.max_x = 1152;
		ts_data->prop.max_y = 1920;
		input_abs_set_max(ts_data->input_dev,
				  ABS_MT_POSITION_X, ts_data->prop.max_x);
		input_abs_set_max(ts_data->input_dev,
				  ABS_MT_POSITION_Y, ts_data->prop.max_y);
	}

	err = input_mt_init_slots(ts_data->input_dev,
				  ts_data->chip->max_touch_num,
				  INPUT_MT_DIRECT | INPUT_MT_DROP_UNUSED);
	if (err) {
		dev_err(ts_data->dev,
			"Failed to initialize input slots: %d\n", err);
		return err;
	}

	err = input_register_device(ts_data->input_dev);
	if (err) {
		dev_err(ts_data->dev,
			"Input device registration failed: %d\n", err);
		return err;
	}

	return 0;
}

int hyn_probe(struct device *dev, struct hynitron_ts_data *ts_data,
	      int irq, const struct hynitron_ts_bus_ops *bus_ops,
	      unsigned int bus_type)
{
	int err;

	ts_data->dev = dev;
	ts_data->bus_ops = bus_ops;

	ts_data->chip = device_get_match_data(dev);
	if (!ts_data->chip)
		return -EINVAL;

	ts_data->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_LOW);
	err = PTR_ERR_OR_ZERO(ts_data->reset_gpio);
	if (err) {
		dev_err(dev, "request reset gpio failed: %d\n", err);
		return err;
	}

	hyn_reset_proc(ts_data, 60);

	err = ts_data->chip->bootloader_enter(ts_data);
	if (err < 0)
		return err;

	err = hyn_input_dev_init(ts_data, bus_type);
	if (err < 0)
		return err;

	err = ts_data->chip->firmware_info(ts_data);
	if (err < 0)
		return err;

	err = devm_request_threaded_irq(dev, irq, NULL, hyn_interrupt_handler,
					IRQF_ONESHOT, "Hynitron Touch Int",
					ts_data);
	if (err) {
		dev_err(dev, "failed to request IRQ: %d\n", err);
		return err;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(hyn_probe);

const struct hynitron_ts_chip_data cst3xx_data = {
	.max_touch_num		= 5,
	.ic_chkcode		= 0xcaca0000,
	.firmware_info		= &cst3xx_firmware_info,
	.bootloader_enter	= &cst3xx_bootloader_enter,
	.report_touch		= &cst3xx_touch_report,
};
EXPORT_SYMBOL_GPL(cst3xx_data);

MODULE_AUTHOR("Chris Morgan");
MODULE_DESCRIPTION("Hynitron Touchscreen Core Driver");
MODULE_LICENSE("GPL");
