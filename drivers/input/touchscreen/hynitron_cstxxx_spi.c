// SPDX-License-Identifier: GPL-2.0-only
/*
 * Driver for Hynitron cstxxx Touchscreen - SPI interface
 *
 * Copyright (c) 2026 Jolla Mobile Ltd
 */

#include <linux/module.h>
#include <linux/spi/spi.h>

#include "hynitron_cstxxx.h"

struct hynitron_ts_spi_data {
	struct hynitron_ts_data ts_data;
	struct spi_device *spi;
};

static int hyn_spi_send(struct hynitron_ts_data *ts_data, const u8 *buf, int len)
{
	struct hynitron_ts_spi_data *spi_data =
		container_of(ts_data, struct hynitron_ts_spi_data, ts_data);

	return spi_write_then_read(spi_data->spi, buf, len, NULL, 0);
}

static int hyn_spi_read(struct hynitron_ts_data *ts_data, const u8 *cmd,
			int cmd_len, u8 *val, int len)
{
	struct hynitron_ts_spi_data *spi_data =
		container_of(ts_data, struct hynitron_ts_spi_data, ts_data);
	struct spi_transfer xfers[] = {
		{
			.len = cmd_len,
			.cs_change = 1,
			.cs_change_delay = {
				.value = 20,
				.unit = SPI_DELAY_UNIT_USECS,
			},
		},
		{
			.len = len,
		},
	};
	int err;

	u8 *buf __free(kfree) = kzalloc(max(len, cmd_len), GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	memcpy(buf, cmd, cmd_len);

	xfers[0].tx_buf = buf;
	xfers[1].rx_buf = buf;

	err = spi_sync_transfer(spi_data->spi, xfers, ARRAY_SIZE(xfers));
	if (err)
		return err;

	memcpy(val, buf, len);

	return 0;
}

static const struct hynitron_ts_bus_ops hyn_spi_ops = {
	.send = &hyn_spi_send,
	.read = &hyn_spi_read,
};

static int hyn_spi_probe(struct spi_device *spi)
{
	struct hynitron_ts_spi_data *spi_data;
	int err;

	spi->mode = SPI_MODE_0;
	spi->bits_per_word = 8;
	err = spi_setup(spi);
	if (err)
		return err;

	spi_data = devm_kzalloc(&spi->dev, sizeof(*spi_data), GFP_KERNEL);
	if (!spi_data)
		return -ENOMEM;

	spi_data->spi = spi;

	return hyn_probe(&spi->dev, &spi_data->ts_data,
			 spi->irq, &hyn_spi_ops, BUS_SPI);
}

static const struct spi_device_id hyn_ts_spi_ids[] = {
	{ .name = "cst3556-spi", .driver_data = (long)&cst66xx_data },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(spi, hyn_ts_spi_ids);

static const struct of_device_id hyn_dt_spi_match[] = {
	{ .compatible = "hynitron,cst3556-spi", .data = &cst66xx_data },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, hyn_dt_spi_match);

static struct spi_driver hynitron_spi_driver = {
	.driver = {
		.name = "Hynitron-TS-SPI",
		.of_match_table = hyn_dt_spi_match,
		.probe_type = PROBE_PREFER_ASYNCHRONOUS,
	},
	.id_table = hyn_ts_spi_ids,
	.probe = hyn_spi_probe,
};

module_spi_driver(hynitron_spi_driver);

MODULE_AUTHOR("Nikolai Burov");
MODULE_DESCRIPTION("Hynitron Touchscreen SPI Driver");
MODULE_LICENSE("GPL");
