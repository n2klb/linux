// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Driver for Hynitron cstxxx Touchscreen - I2C interface
 *
 *  Copyright (c) 2022 Chris Morgan <macromorgan@hotmail.com>
 */

#include <linux/i2c.h>
#include <linux/module.h>

#include "hynitron_cstxxx.h"

struct hynitron_ts_i2c_data {
	struct hynitron_ts_data ts_data;
	struct i2c_client *client;
};

/*
 * The vendor driver would retry twice before failing to read or write
 * to the i2c device.
 */

static int hyn_i2c_send(struct hynitron_ts_data *ts_data, const u8 *buf, int len)
{
	struct hynitron_ts_i2c_data *i2c_data =
		container_of(ts_data, struct hynitron_ts_i2c_data, ts_data);
	int ret;
	int retries = 0;

	while (retries < 2) {
		ret = i2c_master_send(i2c_data->client, buf, len);
		if (ret == len)
			return 0;
		if (ret <= 0)
			retries++;
		else
			break;
	}

	return ret < 0 ? ret : -EIO;
}

static int hyn_i2c_read(struct hynitron_ts_data *ts_data, const u8 *cmd,
			int cmd_len, u8 *val, int len)
{
	struct hynitron_ts_i2c_data *i2c_data =
		container_of(ts_data, struct hynitron_ts_i2c_data, ts_data);
	struct i2c_msg msgs[] = {
		{
			.addr = i2c_data->client->addr,
			.flags = 0,
			.len = cmd_len,
			.buf = (u8 *)cmd,
		},
		{
			.addr = i2c_data->client->addr,
			.flags = I2C_M_RD,
			.len = len,
			.buf = val,
		}
	};
	int ret;

	ret = i2c_transfer(i2c_data->client->adapter, msgs, ARRAY_SIZE(msgs));
	if (ret == ARRAY_SIZE(msgs))
		return 0;

	return ret < 0 ? ret : -EIO;
}

static const struct hynitron_ts_bus_ops hyn_i2c_ops = {
	.send = &hyn_i2c_send,
	.read = &hyn_i2c_read,
};

static int hyn_i2c_probe(struct i2c_client *client)
{
	struct hynitron_ts_i2c_data *i2c_data;

	i2c_data = devm_kzalloc(&client->dev, sizeof(*i2c_data), GFP_KERNEL);
	if (!i2c_data)
		return -ENOMEM;

	i2c_data->client = client;
	i2c_set_clientdata(client, i2c_data);

	return hyn_probe(&client->dev, &i2c_data->ts_data,
			 client->irq, &hyn_i2c_ops, BUS_I2C);
}

static const struct i2c_device_id hyn_tpd_i2c_id[] = {
	{ .name = "hynitron_ts" },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(i2c, hyn_tpd_i2c_id);

static const struct of_device_id hyn_dt_i2c_match[] = {
	{ .compatible = "hynitron,cst340", .data = &cst3xx_data },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, hyn_dt_i2c_match);

static struct i2c_driver hynitron_i2c_driver = {
	.driver = {
		.name = "Hynitron-TS-I2C",
		.of_match_table = hyn_dt_i2c_match,
		.probe_type = PROBE_PREFER_ASYNCHRONOUS,
	},
	.id_table = hyn_tpd_i2c_id,
	.probe = hyn_i2c_probe,
};

module_i2c_driver(hynitron_i2c_driver);

MODULE_AUTHOR("Chris Morgan");
MODULE_DESCRIPTION("Hynitron Touchscreen I2C Driver");
MODULE_LICENSE("GPL");
