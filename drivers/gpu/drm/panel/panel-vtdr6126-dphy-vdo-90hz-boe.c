// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2019 MediaTek Inc.
 * Copyright (c) 2026 Jolla Mobile Ltd
 */

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>

#include <drm/drm_connector.h>
#include <drm/drm_device.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

#include <drm/display/drm_dsc_helper.h>

#include <video/mipi_display.h>

//yft-drv add begin
#if IS_ENABLED(CONFIG_YFT_DEVICE_INFO_SUPPORT)
//#include "yft_devices.h"
extern int yft_set_lcm_device_used(char * module_name, int pdata);
typedef enum
{
	DEVICE_SUPPORTED = 0,
	DEVICE_USED = 1,
}campatible_type;
extern int yft_lcm_device_add(struct mipi_dsi_driver* nLcm, campatible_type isUsed);
static struct mipi_dsi_driver lcm_driver;
extern int is_yft_cts_board(void);
#endif
//yft-drv add end
#define ADJUST_BACKLIGHT(aal_bl, panel_bl) (aal_bl * (panel_bl + 1))
#define ENABLE_DSC 		1

/*LCM_DEGREE default value*/
#define PROBE_FROM_DTS 0

//extern int ocp2131_write_bytes(unsigned char addr, unsigned char value);
__maybe_unused static int current_fps = 60;
//static char bl_tb0[] = { 0x51, 0xff };

struct lcm {
	struct device *dev;
	struct drm_panel panel;
	struct gpio_desc *reset_gpio;
	struct gpio_desc *powerdm_gpio;
	struct gpio_desc *bias_pos;
	struct gpio_desc *bias_neg;
	bool prepared;
	void (*tp_reset_switch_callback)(int);

	struct drm_dsc_config dsc;

	int error;
};

#define lcm_dcs_write_seq(ctx, seq...)                                         \
	({                                                                     \
		const u8 d[] = { seq };                                        \
		BUILD_BUG_ON_MSG(ARRAY_SIZE(d) > 64,                           \
				 "DCS sequence too big for stack");            \
		lcm_dcs_write(ctx, d, ARRAY_SIZE(d));                          \
	})

#define lcm_dcs_write_seq_static(ctx, seq...)                                  \
	({                                                                     \
		static const u8 d[] = { seq };                                 \
		lcm_dcs_write(ctx, d, ARRAY_SIZE(d));                          \
	})

static inline struct lcm *panel_to_lcm(struct drm_panel *panel)
{
	return container_of(panel, struct lcm, panel);
}

static void lcm_dcs_write(struct lcm *ctx, const void *data, size_t len)
{
	struct mipi_dsi_device *dsi = to_mipi_dsi_device(ctx->dev);
	ssize_t ret;
	char *addr;

	if (ctx->error < 0)
		return;

	addr = (char *)data;
	if ((int)*addr < 0xB0)
		ret = mipi_dsi_dcs_write_buffer(dsi, data, len);
	else
		ret = mipi_dsi_generic_write(dsi, data, len);
	if (ret < 0) {
		dev_info(ctx->dev, "error %zd writing seq: %ph\n", ret, data);
		ctx->error = ret;
	}
}

#define DSC_BPG_OFFSET(x)	((u8)((x) & DSC_RANGE_BPG_OFFSET_MASK))

static const struct drm_dsc_config lcm_dsc_config = {
	.dsc_version_major = 1,
	.dsc_version_minor = 1,
	.bits_per_component = 10,
	.line_buf_depth = 11,
	.block_pred_enable = true,
	.bits_per_pixel = 8 << 4,
	.pic_height = 2272,
	.pic_width = 1032,
	.slice_height = 8,
	.slice_width = 516,
	.slice_count = 2,
#if 0
	.slice_chunk_size = 516,
	.initial_xmit_delay = 512,
	.initial_dec_delay = 514,
	.initial_scale_value = 32,
	.scale_increment_interval = 183,
	.scale_decrement_interval = 7,
	.first_line_bpg_offset = 12,
	.nfl_bpg_offset = 3511,
	.slice_bpg_offset = 3406,
	.initial_offset = 6144,
	.final_offset = 4336,
	.flatness_min_qp = 7,
	.flatness_max_qp = 16,
	.rc_model_size = 8192,
	.rc_edge_factor = 6,
	.rc_quant_incr_limit0 = 15,
	.rc_quant_incr_limit1 = 15,
	.rc_tgt_offset_high = 3,
	.rc_tgt_offset_low = 3,
	.rc_buf_thresh = {14, 28, 42, 56, 70, 84, 98, 105, 112, 119, 121, 123, 125, 126},
	.rc_range_params = {
		{ 0,  8, DSC_BPG_OFFSET(2)},
		{ 4,  8, DSC_BPG_OFFSET(0)},
		{ 5,  9, DSC_BPG_OFFSET(0)},
		{ 5, 10, DSC_BPG_OFFSET(-2)},
		{ 7, 11, DSC_BPG_OFFSET(-4)},
		{ 7, 11, DSC_BPG_OFFSET(-6)},
		{ 7, 11, DSC_BPG_OFFSET(-8)},
		{ 7, 12, DSC_BPG_OFFSET(-8)},
		{ 7, 13, DSC_BPG_OFFSET(-8)},
		{ 7, 14, DSC_BPG_OFFSET(-10)},
		{ 9, 14, DSC_BPG_OFFSET(-10)},
		{ 9, 15, DSC_BPG_OFFSET(-12)},
		{ 9, 15, DSC_BPG_OFFSET(-12)},
		{13, 16, DSC_BPG_OFFSET(-12)},
		{16, 17, DSC_BPG_OFFSET(-12)},
	},
#endif
};

static void lcm_panel_init(struct lcm *ctx)
{
//	ctx->reset_gpio = devm_gpiod_get(ctx->dev, "reset", GPIOD_OUT_HIGH);
	pr_info("[LCM] %s +++",__func__);
	
	pr_info("[LCM] yft-drv- add %s bias_pos  is on !!\n",__func__);
	gpiod_set_value(ctx->bias_pos, 1);
	
	usleep_range(10 * 1000, 15 * 1000);
	gpiod_set_value(ctx->reset_gpio, 1);
	pr_info("[LCM] yft-drv- add %s reset_gpio  is on !!\n",__func__);
	msleep(20);
	gpiod_set_value(ctx->reset_gpio, 0);
	pr_info("[LCM] yft-drv- add %s reset_gpio  is off !!\n",__func__);
	msleep(20);
	gpiod_set_value(ctx->reset_gpio, 1);
	pr_info("[LCM] yft-drv- add %s reset_gpio  is on !!\n",__func__);
	//msleep(20);
	msleep(120);

	lcm_dcs_write_seq_static(ctx, 0x03, 0x01);
	lcm_dcs_write_seq_static(ctx, 0x35, 0x00);
	//lcm_dcs_write_seq_static(ctx, 0x51 ,0x07, 0xff);
	lcm_dcs_write_seq_static(ctx, 0x53, 0x20);
	lcm_dcs_write_seq_static(ctx, 0x6C, 0x00);
	lcm_dcs_write_seq_static(ctx, 0x6D, 0x00);
	lcm_dcs_write_seq_static(ctx, 0x6F, 0x01);
	lcm_dcs_write_seq_static(ctx, 0x59, 0x09);

	lcm_dcs_write_seq_static(ctx, 0x70, 0x11, 0x00, 0x00, 0xAB, 0x30, 0x80, 0x08, 0xE0, 0x04, 0x08, 0x00, 0x08, 0x02, 0x04, 0x02, 0x04, 0x02, 0x00, 0x02, 0x02, 0x00, 0x20, 0x00, 0xB7, 0x00, 0x07, 0x00, 0x0C, 0x0D, 0xB7, 0x0D, 0x4E, 0x18, 0x00, 0x10, 0xF0, 0x07, 0x10, 0x20, 0x00, 0x06, 0x0F, 0x0F, 0x33, 0x0E, 0x1C, 0x2A, 0x38, 0x46, 0x54, 0x62, 0x69, 0x70, 0x77, 0x79, 0x7B, 0x7D, 0x7E, 0x02, 0x02, 0x22, 0x00, 0x2A, 0x40, 0x2A, 0xBE, 0x3A, 0xFC, 0x3A, 0xFA, 0x3A, 0xF8, 0x3B, 0x38, 0x3B, 0x78, 0x3B, 0xB6, 0x4B, 0xB6, 0x4B, 0xF4, 0x4B, 0xF4, 0x6C, 0x34, 0x84, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

	//CMD2 P0
	lcm_dcs_write_seq_static(ctx,0xF0,0xAA,0x10);
	lcm_dcs_write_seq_static(ctx,0xB0,0x04,0x70,0x01,0x02,0x00,0x04,0x08);
	lcm_dcs_write_seq_static(ctx,0xC1,0x80,0x06,0x08,0x11,0x00,0x06,0x08,0x11,0x00,0x06,0x08,0x11,0x00,0x06,0x02,0x11,0x00,0x80);

	lcm_dcs_write_seq_static(ctx,0xf0,0xaa,0x12);
	lcm_dcs_write_seq_static(ctx,0xb7,0x48,0x48,0x48,0x48,0x48,0x48,0x48);

	lcm_dcs_write_seq_static(ctx,0xF0,0xAA,0x14);
	//ECK
	lcm_dcs_write_seq_static(ctx,0xBE,0x10,0x10,0x00,0x18,0x22,0x0d,0x62,0x0d,0x62,0x0d,0x62,0x0d,0x62);

	lcm_dcs_write_seq_static(ctx,0xf0,0xaa,0x15);
	lcm_dcs_write_seq_static(ctx,0xbC,0x0A,0x00,0x0A,0x00,0x0A,0x00,0x0A,0x00);

	//SPR
	lcm_dcs_write_seq_static(ctx,0xF0,0xAA,0x17);
	lcm_dcs_write_seq_static(ctx,0xB0,0x90);
	lcm_dcs_write_seq_static(ctx,0xB1,0x26);
	lcm_dcs_write_seq_static(ctx,0xB2,0x44,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x20,0x00,0x00,0x00,0x20,0x20,0x00,0x00,0x40,0x00,0x20,0x20,0x00);
	lcm_dcs_write_seq_static(ctx,0xB3,0x20,0x20,0x00,0x00,0x40,0x00,0x20,0x20,0x00,0x20,0xA0,0x00,0x00,0xB0,0x00,0x20,0x20,0x00);
	lcm_dcs_write_seq_static(ctx,0xB4,0x00,0x88,0x88,0x00,0x88,0x88,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x88,0x88,0x00,0x88,0x88,0x00);
	lcm_dcs_write_seq_static(ctx,0xC0,0x21,0x00,0x65,0x20,0x61,0x05);
	lcm_dcs_write_seq_static(ctx,0xC2,0x80);
	lcm_dcs_write_seq_static(ctx,0xC3,0x55,0x55);

	lcm_dcs_write_seq_static(ctx,0xF0,0xAA,0x18);
	lcm_dcs_write_seq_static(ctx,0xB0,0x19,0x2A,0x00,0x09,0x01,0x00,0x01,0x00,0x20,0x35,0x46,0x01,0x00,0x00);
	lcm_dcs_write_seq_static(ctx,0xB1,0x39,0x01,0x0B,0x01,0x00,0x01,0x00,0x2C,0x48,0x4F,0x01);
	lcm_dcs_write_seq_static(ctx,0xB2,0x16,0x01,0x01,0x31,0x00,0xA4,0x42,0x06,0x00,0x18,0x21);
	lcm_dcs_write_seq_static(ctx,0xB3,0x26,0x00,0x0A,0x01,0x00,0x01,0x00,0x1B,0x2E,0x33,0x00,0x02,0x00,0x08);
	lcm_dcs_write_seq_static(ctx,0xB4,0x2A,0x00,0x0C,0x01,0x00,0x01,0x00,0x1D,0x33,0x45,0x00,0x02,0x00,0x06);
	lcm_dcs_write_seq_static(ctx,0xC0,0x00,0xC2,0xD9,0xC9,0x14,0x92,0x4C,0xE8,0x94,0x3A,0x9D,0x4E,0xA7,0x52,0xA8);
	lcm_dcs_write_seq_static(ctx,0xC1,0x2A,0x9D,0x0A,0x85,0x42,0x99,0x4C,0xA4,0x52,0x28,0x94,0x4A,0x04,0xF3,0x70);
	lcm_dcs_write_seq_static(ctx,0xC2,0x27,0x13,0x49,0x84,0xB1,0x58,0xA8,0x52,0x28,0x13,0x89,0x84,0xA2,0x41,0x18);
	lcm_dcs_write_seq_static(ctx,0xC3,0x11,0x08,0x40,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	lcm_dcs_write_seq_static(ctx,0xE0,0x18,0xC2,0xB4,0x0D,0x86,0x71,0x9A,0x55,0x84,0xCB,0x01,0x10,0xC7,0xCA,0x96);
	lcm_dcs_write_seq_static(ctx,0xE1,0xC6,0xB9,0xDF,0x7F,0xFF,0x73,0x9B,0x7A,0xC7,0x34,0xE5,0x79,0x4D,0x42,0xDA);
	lcm_dcs_write_seq_static(ctx,0xE2,0x85,0x31,0xBF,0x67,0x59,0x00,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x7C,0x00);
	lcm_dcs_write_seq_static(ctx,0x65,0x0C);
	lcm_dcs_write_seq_static(ctx,0xB0,0x01);
	lcm_dcs_write_seq_static(ctx,0xC0,0x60,0xD4,0x1E,0xAD,0x85,0xD2,0x71,0x3A,0x9E,0x4F,0x1F,0xCF,0xE7,0xF3,0xF8);
	lcm_dcs_write_seq_static(ctx,0xC1,0x3F,0x9F,0xCF,0xE5,0xF3,0xF1,0x78,0xBC,0x5D,0x2E,0x97,0x0B,0x85,0xB2,0xD0);
	lcm_dcs_write_seq_static(ctx,0xC2,0x2D,0x16,0x4B,0x05,0x82,0xB9,0x58,0xAA,0x55,0x2A,0x14,0xCA,0x45,0x12,0x80);
	lcm_dcs_write_seq_static(ctx,0xC3,0x18,0x0B,0xC5,0xC2,0xD1,0x60,0xAC,0x54,0x29,0x14,0x09,0xC4,0xC2,0x51,0x20);
	lcm_dcs_write_seq_static(ctx,0xC4,0x11,0x88,0x84,0x20,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	lcm_dcs_write_seq_static(ctx,0xE0,0x4A,0x0C,0x40,0x24,0x8C,0x33,0x4A,0xC1,0xA5,0xE5,0x53,0x86,0x75,0xB8,0x45);
	lcm_dcs_write_seq_static(ctx,0xE1,0x42,0xDA,0xF8,0xCE,0xB7,0xC6,0x75,0xBE,0x77,0xDE,0xFF,0xFF,0xFA,0x52,0x70);
	lcm_dcs_write_seq_static(ctx,0xE2,0x6D,0xA5,0xAA,0x6E,0x9B,0x96,0x3D,0x4C,0xFA,0x77,0xDF,0xA5,0x5C,0x6B,0xBF);
	lcm_dcs_write_seq_static(ctx,0xE3,0xFF,0xFF,0xDD,0x03,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	lcm_dcs_write_seq_static(ctx,0x65,0x0C);
	lcm_dcs_write_seq_static(ctx,0xB0,0x02);
	lcm_dcs_write_seq_static(ctx,0xC0,0xB0,0xB2,0x11,0x68,0xC4,0x69,0xB8,0xDE,0x6F,0x37,0x94,0x0A,0x05,0x02,0x80);
	lcm_dcs_write_seq_static(ctx,0xC1,0x28,0x1B,0xC9,0xE4,0xF2,0x79,0x38,0x9C,0x4D,0x26,0x93,0x09,0x84,0xB1,0x58);
	lcm_dcs_write_seq_static(ctx,0xC2,0x25,0x12,0x49,0x02,0x81,0x38,0x98,0x4A,0x24,0x11,0x88,0x84,0x20,0x10,0x00);
	lcm_dcs_write_seq_static(ctx,0xE0,0x52,0x50,0x61,0x35,0x03,0x61,0x5A,0x46,0x09,0x0E,0x11,0xD9,0x09,0xDF,0x3C);
	lcm_dcs_write_seq_static(ctx,0xE1,0xEF,0xBF,0x7B,0xDA,0x91,0xE5,0xE5,0xCA,0xF6,0xBC,0x9E,0x41,0x4C,0xF4,0x00);
	lcm_dcs_write_seq_static(ctx,0xE2,0x00,0x3E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	lcm_dcs_write_seq_static(ctx,0x65,0x0C);
	lcm_dcs_write_seq_static(ctx,0xB0,0x03);
	lcm_dcs_write_seq_static(ctx,0xC0,0xE0,0xC9,0xDD,0x4C,0xB6,0x62,0xB5,0x5C,0xAE,0x57,0x23,0xD1,0xE8,0xF4,0x78);
	lcm_dcs_write_seq_static(ctx,0xC1,0x47,0xA3,0xD1,0xE8,0xF4,0x79,0xBD,0x1C,0x8E,0x37,0x1B,0x91,0xA6,0xD4,0x60);
	lcm_dcs_write_seq_static(ctx,0xC2,0x36,0x22,0xCD,0x66,0xA2,0x51,0x24,0x90,0x47,0x13,0x89,0x84,0xA2,0x41,0x18);
	lcm_dcs_write_seq_static(ctx,0xC3,0x11,0x08,0x40,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	lcm_dcs_write_seq_static(ctx,0xE0,0x31,0x8A,0x41,0x85,0x05,0x12,0x0A,0x13,0x04,0xA0,0x22,0x16,0xF9,0x52,0xF9);
	lcm_dcs_write_seq_static(ctx,0xE1,0xDF,0x7D,0xFF,0xB9,0xCD,0x62,0xD5,0x07,0x2E,0x2E,0x53,0xD4,0xF5,0x3A,0x55);
	lcm_dcs_write_seq_static(ctx,0xE2,0xCF,0x3F,0x5A,0xD6,0x93,0x96,0x2D,0x4D,0x5E,0x78,0xA6,0x69,0x8E,0x77,0xBD);
	lcm_dcs_write_seq_static(ctx,0xE3,0xE6,0xF4,0x0F,0xF4,0x00,0xE0,0x3A,0x0E,0x80,0x00,0x00,0x00,0x00,0x00,0x00);


	lcm_dcs_write_seq_static(ctx,0xFF,0x5A,0x80);
	lcm_dcs_write_seq_static(ctx,0x65,0x26);
	lcm_dcs_write_seq_static(ctx,0xF7,0x01);
	lcm_dcs_write_seq_static(ctx,0x65,0x0A);
	lcm_dcs_write_seq_static(ctx,0xF9,0xA8);

	lcm_dcs_write_seq_static(ctx,0xFF,0x5A,0x81);
	lcm_dcs_write_seq_static(ctx,0x65,0x02);
	lcm_dcs_write_seq_static(ctx,0xF4,0x27);
	lcm_dcs_write_seq_static(ctx,0x65,0x19);

	lcm_dcs_write_seq_static(ctx,0xF8,0x01);
	lcm_dcs_write_seq_static(ctx,0x65,0x03);
	lcm_dcs_write_seq_static(ctx,0xfb,0x93,0x93);

	lcm_dcs_write_seq_static(ctx,0xF0,0xAA,0x00);
	lcm_dcs_write_seq_static(ctx,0xFF,0x5A,0x00);

	//lcm_dcs_write_seq_static(ctx,0x22);
/*
	lcm_dcs_write_seq_static(ctx,0x11);
	msleep(120);
	lcm_dcs_write_seq_static(ctx,0x29);
	msleep(1000);
	*/
	//lcm_dcs_write_seq_static(ctx,0x13);
	
	pr_info("[LCM] %s -----",__func__);
}


struct lcm *global_ctx;

/*int tp_reset_switch_register(void (*callback)(int), void *data){
	global_ctx->tp_reset_switch_callback = callback;
	return 0;
}
EXPORT_SYMBOL(tp_reset_switch_register);*/

static int lcm_unprepare(struct drm_panel *panel)
{

	struct lcm *ctx = panel_to_lcm(panel);

	pr_info("[LCM] %s +++",__func__);

	if (!ctx->prepared)
		return 0;

	lcm_dcs_write_seq_static(ctx, 0x28);
	msleep(20);
	lcm_dcs_write_seq_static(ctx, 0x10);
	msleep(100);

//	ctx->reset_gpio = devm_gpiod_get(ctx->dev, "reset", GPIOD_OUT_LOW);
	pr_info("[LCM] yft-drv- add %s reset_gpio  is off !!\n",__func__);
	gpiod_set_value(ctx->reset_gpio, 0);
//	devm_gpiod_put(ctx->dev, ctx->reset_gpio);
	msleep(5);

//	ctx->bias_neg = devm_gpiod_get_index(ctx->dev, "bias", 1, GPIOD_OUT_LOW);
	pr_info("[LCM] yft-drv- add %s bias_pos  is off !!\n",__func__);
	gpiod_set_value(ctx->bias_pos, 0);

//	devm_gpiod_put(ctx->dev, ctx->bias_neg);
	msleep(5);
//	ctx->bias_pos =	devm_gpiod_get_index(ctx->dev, "bias", 0, GPIOD_OUT_LOW);
	pr_info("[LCM] yft-drv- add %s bias_neg  is off !!\n",__func__);
	gpiod_set_value(ctx->bias_neg, 0);
//	devm_gpiod_put(ctx->dev, ctx->bias_pos);
	msleep(5);

#if 1
//	ctx->powerdm_gpio =
//		devm_gpiod_get(ctx->dev, "powerdm", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->powerdm_gpio)) {
		dev_err(ctx->dev, "%s: cannot get powerdm_gpio %ld\n",
			__func__, PTR_ERR(ctx->powerdm_gpio));
//		return PTR_ERR(ctx->powerdm_gpio);
	}
	pr_info("[LCM] yft-drv- add %s powerdm_gpio  is off !!\n",__func__);
	gpiod_set_value(ctx->powerdm_gpio, 0);
//	devm_gpiod_put(ctx->dev, ctx->powerdm_gpio);
#endif
/*
	if(NULL != global_ctx->tp_reset_switch_callback){
		global_ctx->tp_reset_switch_callback(0);
	}
	msleep(10);
*/
	ctx->error = 0;
	ctx->prepared = false;
	pr_info("[LCM] %s ---",__func__);
	return 0;
}

static int lcm_power_on(struct drm_panel *panel)
{
	int ret = 0;
	
	struct lcm *ctx = panel_to_lcm(panel);

	pr_info("[LCM] %s +++++++++",__func__);
	if (IS_ERR(ctx->powerdm_gpio)) {
		dev_err(ctx->dev, "%s: cannot get powerdm_gpio %ld\n",
			__func__, PTR_ERR(ctx->powerdm_gpio));
		return PTR_ERR(ctx->powerdm_gpio);
	}
	
	pr_info("[LCM] yft-drv- add %s powerdm_gpio  is on !!\n",__func__);
	gpiod_set_value(ctx->powerdm_gpio, 1);

	msleep(5);

	pr_info("[LCM] yft-drv- add %s bias_neg  is on !!\n",__func__);
	gpiod_set_value(ctx->bias_neg, 1);

	msleep(5);

	pr_info("[LCM] %s ---",__func__);
	return ret;
}

static int lcm_prepare(struct drm_panel *panel)
{
	struct lcm *ctx = panel_to_lcm(panel);
	int ret;

	pr_info("[LCM] %s +++",__func__);
	if (ctx->prepared)
		return 0;

	lcm_power_on(panel);

	lcm_panel_init(ctx);

	{
		struct mipi_dsi_multi_context dctx = { .dsi = to_mipi_dsi_device(ctx->dev) };

		mipi_dsi_dcs_exit_sleep_mode_multi(&dctx);
		mipi_dsi_msleep(&dctx, 120);
	}

	ret = ctx->error;
	if (ret < 0)
		lcm_unprepare(panel);

	ctx->prepared = true;

	pr_info("[LCM] %s ---",__func__);
	return ret;
}

static int lcm_enable(struct drm_panel *panel)
{
	struct lcm *ctx = panel_to_lcm(panel);
	struct mipi_dsi_multi_context dctx = { .dsi = to_mipi_dsi_device(ctx->dev) };

	dctx.dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_set_display_on_multi(&dctx);

	return 0;
}

static int lcm_disable(struct drm_panel *panel)
{
	struct lcm *ctx = panel_to_lcm(panel);
	struct mipi_dsi_multi_context dctx = { .dsi = to_mipi_dsi_device(ctx->dev) };

	mipi_dsi_dcs_set_display_off_multi(&dctx);

	dctx.dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return 0;
}


static const struct drm_display_mode default_mode = {
	.clock = (1032 + 162 + 4 + 20) * (2272 + 2316 + 4 + 20) * 60 / 1000,
	.hdisplay = 1032,
	.hsync_start = 1032 + 162,
	.hsync_end = 1032 + 162 + 4,
	.htotal = 1032 + 162 + 4 + 20,
	.vdisplay = 2272,
	.vsync_start = 2272 + 2316,
	.vsync_end = 2272 + 2316 + 4,
	.vtotal = 2272 + 2316 + 4 + 20,
};

static const struct drm_display_mode performance_mode_90hz = {
	.clock = (1032 + 162 + 4 + 20) * (2272 + 779 + 4 + 20) * 90 / 1000,
	.hdisplay = 1032,
	.hsync_start = 1032 + 162,
	.hsync_end = 1032 + 162 + 4,
	.htotal = 1032 + 162 + 4 + 20,
	.vdisplay = 2272,
	.vsync_start = 2272 + 779,
	.vsync_end = 2272 + 779 + 4,
	.vtotal = 2272 + 779 + 4 + 20,
};

static const struct drm_display_mode performance_mode_120hz = {
	.clock = (1032 + 162 + 4 + 20) * (2272 + 10 + 4 + 20) * 120 / 1000,
	.hdisplay = 1032,
	.hsync_start = 1032 + 162,
	.hsync_end = 1032 + 162 + 4,
	.htotal = 1032 + 162 + 4 + 20,
	.vdisplay = 2272,
	.vsync_start = 2272 + 10,
	.vsync_end = 2272 + 10 + 4,
	.vtotal = 2272 + 10 + 4 + 20,
};

#if defined(CONFIG_MTK_PANEL_EXT)

static struct mtk_panel_params ext_params = {
	.pll_clk = PLL_CLK,
	.cust_esd_check = 1,
	.esd_check_enable = 1,
	.lcm_degree = PROBE_FROM_DTS,
	.lcm_esd_check_table[0] = {
		.cmd = 0x0A, .count = 1, .para_list[0] = 0x9C,
	},
	.vdo_keep_hs_perline = 1, //data line frame LP11
	//.physical_width_um = 66950,//69574,
	//.physical_height_um = 147680,//157560,
#if ENABLE_DSC
	.output_mode = MTK_PANEL_DSC_SINGLE_PORT,
	.dsc_param_load_mode = 0, //0: default flow; 1: key param only; 2: full control
	.dsc_params = {
		.enable = 1,
		.ver = 17,
		.slice_mode = 1,
		.rgb_swap = 0,
		.dsc_cfg = 40,
		.rct_on = 1,
		.bit_per_channel = 10,
		.dsc_line_buf_depth = 11,
		.bp_enable = 1,
		.bit_per_pixel = 128,
		.pic_height = 2272,
		.pic_width = 1032,
		.slice_height = 8,
		.slice_width = 516,
		.chunk_size = 516,
		.xmit_delay = 512,
		.dec_delay = 514,
		.scale_value = 32,
		.increment_interval = 183,
		.decrement_interval = 7,
		.line_bpg_offset = 12,
		.nfl_bpg_offset = 3511,
		.slice_bpg_offset = 3406,
		.initial_offset = 6144,
		.final_offset = 4336,
		.flatness_minqp = 7,
		.flatness_maxqp = 16,
		.rc_model_size = 8192,
		.rc_edge_factor = 6,
		.rc_quant_incr_limit0 = 15,
		.rc_quant_incr_limit1 = 15,
		.rc_tgt_offset_hi = 3,
		.rc_tgt_offset_lo = 3,
		},
#endif		
	.data_rate = PLL_CLK*2,
};

static struct mtk_panel_params ext_params_90hz = {
	.pll_clk = PLL_CLK,
	.cust_esd_check = 1,
	.esd_check_enable = 1,
	.lcm_degree = PROBE_FROM_DTS,
	.lcm_esd_check_table[0] = {
		.cmd = 0x0A, .count = 1, .para_list[0] = 0x9C,
	},
	.vdo_keep_hs_perline = 1, //data line frame LP11
#if ENABLE_DSC
	.output_mode = MTK_PANEL_DSC_SINGLE_PORT,
	.dsc_param_load_mode = 0, //0: default flow; 1: key param only; 2: full control
	.dsc_params = {
		.enable = 1,
		.ver = 17,
		.slice_mode = 1,
		.rgb_swap = 0,
		.dsc_cfg = 40,
		.rct_on = 1,
		.bit_per_channel = 10,
		.dsc_line_buf_depth = 11,
		.bp_enable = 1,
		.bit_per_pixel = 128,
		.pic_height = 2272,
		.pic_width = 1032,
		.slice_height = 8,
		.slice_width = 516,
		.chunk_size = 516,
		.xmit_delay = 512,
		.dec_delay = 514,
		.scale_value = 32,
		.increment_interval = 183,
		.decrement_interval = 7,
		.line_bpg_offset = 12,
		.nfl_bpg_offset = 3511,
		.slice_bpg_offset = 3406,
		.initial_offset = 6144,
		.final_offset = 4336,
		.flatness_minqp = 7,
		.flatness_maxqp = 16,
		.rc_model_size = 8192,
		.rc_edge_factor = 6,
		.rc_quant_incr_limit0 = 15,
		.rc_quant_incr_limit1 = 15,
		.rc_tgt_offset_hi = 3,
		.rc_tgt_offset_lo = 3,
		},
#endif
	.data_rate = PLL_CLK*2,
};
/*
static struct mtk_panel_params ext_params_120hz = {
	.pll_clk = PLL_CLK,
	.cust_esd_check = 1,
	.esd_check_enable = 1,
	.lcm_degree = PROBE_FROM_DTS,
	.lcm_esd_check_table[0] = {
		.cmd = 0x0A, .count = 1, .para_list[0] = 0x9C,
	},
	.vdo_keep_hs_perline = 1, //data line frame LP11
#if ENABLE_DSC
	.output_mode = MTK_PANEL_DSC_SINGLE_PORT,
	.dsc_param_load_mode = 0, //0: default flow; 1: key param only; 2: full control
	.dsc_params = {
		.enable = 1,
		.ver = 17,
		.slice_mode = 1,
		.rgb_swap = 0,
		.dsc_cfg = 34,
		.rct_on = 1,
		.bit_per_channel = 8,
		.dsc_line_buf_depth = 9,
		.bp_enable = 1,
		.bit_per_pixel = 128,
		.pic_height = 2772,
		.pic_width = 1280,
		.slice_height = 12,
		.slice_width = 640,
		.chunk_size = 640,
		.xmit_delay = 512,
		.dec_delay = 577,
		.scale_value = 32,
		.increment_interval = 312,
		.decrement_interval = 8,
		.line_bpg_offset = 12,
		.nfl_bpg_offset = 2235,
		.slice_bpg_offset = 1825,
		.initial_offset = 6144,
		.final_offset = 4336,
		.flatness_minqp = 3,
		.flatness_maxqp = 12,
		.rc_model_size = 8192,
		.rc_edge_factor = 6,
		.rc_quant_incr_limit0 = 11,
		.rc_quant_incr_limit1 = 11,
		.rc_tgt_offset_hi = 3,
		.rc_tgt_offset_lo = 3,
		},
#endif
	.data_rate = PLL_CLK*2,
};


static struct mtk_panel_params ext_params_144hz = {
	.pll_clk = PLL_CLK,
	.cust_esd_check = 1,
	.esd_check_enable = 1,
	.lcm_degree = PROBE_FROM_DTS,
	.lcm_esd_check_table[0] = {
		.cmd = 0x0A, .count = 1, .para_list[0] = 0x9C,
	},
	.vdo_keep_hs_perline = 1, //data line frame LP11
#if ENABLE_DSC
	.output_mode = MTK_PANEL_DSC_SINGLE_PORT,
	.dsc_param_load_mode = 0, //0: default flow; 1: key param only; 2: full control
	.dsc_params = {
		.enable = 1,
		.ver = 17,
		.slice_mode = 1,
		.rgb_swap = 0,
		.dsc_cfg = 34,
		.rct_on = 1,
		.bit_per_channel = 8,
		.dsc_line_buf_depth = 9,
		.bp_enable = 1,
		.bit_per_pixel = 128,
		.pic_height = 2340,
		.pic_width = 1080,
		.slice_height = 20,
		.slice_width = 540,
		.chunk_size = 540,
		.xmit_delay = 512,
		.dec_delay = 526,
		.scale_value = 32,
		.increment_interval = 488,
		.decrement_interval = 7,
		.line_bpg_offset = 12,
		.nfl_bpg_offset = 1294,
		.slice_bpg_offset = 1302,
		.initial_offset = 6144,
		.final_offset = 4336,
		.flatness_minqp = 3,
		.flatness_maxqp = 12,
		.rc_model_size = 8192,
		.rc_edge_factor = 6,
		.rc_quant_incr_limit0 = 11,
		.rc_quant_incr_limit1 = 11,
		.rc_tgt_offset_hi = 3,
		.rc_tgt_offset_lo = 3,
		},
#endif
	.data_rate = PLL_CLK*2,
};
*/

#endif

static int lcm_get_modes(struct drm_panel *panel,
					struct drm_connector *connector)
{
	struct drm_display_mode *mode;
	struct drm_display_mode *mode2;
	struct drm_display_mode *mode3;
	//struct drm_display_mode *mode4;


	pr_info("%s+ yft-lcm-vtdr6126-drv- add  11111\n", __func__);
	
	mode = drm_mode_duplicate(connector->dev, &default_mode);
	if (!mode) {
		dev_info(connector->dev->dev, "failed to add mode %ux%ux@%u\n",
			 default_mode.hdisplay, default_mode.vdisplay,
			 drm_mode_vrefresh(&default_mode));
		return -ENOMEM;
	}

	drm_mode_set_name(mode);
	mode->type = DRM_MODE_TYPE_DRIVER;
	drm_mode_probed_add(connector, mode);
	
	mode2 = drm_mode_duplicate(connector->dev, &performance_mode_90hz);
	if (!mode2) {
		dev_info(connector->dev->dev, "failed to add mode %ux%ux@%u\n",
			 performance_mode_90hz.hdisplay, performance_mode_90hz.vdisplay,
			 drm_mode_vrefresh(&performance_mode_90hz));
		return -ENOMEM;
	}

	drm_mode_set_name(mode2);
	mode2->type = DRM_MODE_TYPE_DRIVER;
	drm_mode_probed_add(connector, mode2);

	mode3 = drm_mode_duplicate(connector->dev, &performance_mode_120hz);
	if (!mode3) {
		dev_info(connector->dev->dev, "failed to add mode %ux%ux@%u\n",
			 performance_mode_120hz.hdisplay, performance_mode_120hz.vdisplay,
			 drm_mode_vrefresh(&performance_mode_120hz));
		return -ENOMEM;
	}

	drm_mode_set_name(mode3);
	mode3->type = DRM_MODE_TYPE_DRIVER;
	drm_mode_probed_add(connector, mode3);

#if 0
	mode4 = drm_mode_duplicate(connector->dev, &performance_mode_144hz);
	if (!mode4) {
		dev_info(connector->dev->dev, "failed to add mode %ux%ux@%u\n",
			 performance_mode_144hz.hdisplay, performance_mode_144hz.vdisplay,
			 drm_mode_vrefresh(&performance_mode_144hz));
		return -ENOMEM;
	}

	drm_mode_set_name(mode4);
	mode4->type = DRM_MODE_TYPE_DRIVER  | DRM_MODE_TYPE_PREFERRED;
	drm_mode_probed_add(connector, mode4);
#endif

	connector->display_info.width_mm = 66;
	connector->display_info.height_mm = 159;

	//yft-drv add begin
	#if IS_ENABLED(CONFIG_YFT_DEVICE_INFO_SUPPORT)
	yft_set_lcm_device_used("panel-vtdr6126-dphy-vdo-90hz-boe", DEVICE_USED);
	#endif
	//yft-drv add end

	pr_info("%s+ yft-lcm-vtdr6126-drv- end\n", __func__);
	return 3;
}

static const struct drm_panel_funcs lcm_drm_funcs = {
	.disable = lcm_disable,
	.unprepare = lcm_unprepare,
	.prepare = lcm_prepare,
	.enable = lcm_enable,
	.get_modes = lcm_get_modes,
};

static int lcm_bl_update_status(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	u16 brightness = backlight_get_brightness(bl);
	int ret;

	ret = mipi_dsi_dcs_set_display_brightness_large(dsi, brightness);
	if (ret < 0)
		return ret;

	return 0;
}

static const struct backlight_ops lcm_bl_ops = {
	.update_status = lcm_bl_update_status,
};

static struct backlight_device *
lcm_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = 4095,
		.max_brightness = 4095,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &lcm_bl_ops, &props);
}

static int lcm_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct lcm *ctx;
	int ret;

	pr_info("%s+ xl,vtdr6126,dphy,vdo,120hz start\n", __func__);

	ctx = devm_drm_panel_alloc(dev, struct lcm, panel,
				   &lcm_drm_funcs, DRM_MODE_CONNECTOR_DSI);
	if (!ctx)
		return -ENOMEM;

	mipi_dsi_set_drvdata(dsi, ctx);

	ctx->dev = dev;
	dsi->dsc = &ctx->dsc;
	dsi->lanes = 4;
	/*
	 * Since this is a DSC panel, the format doesn't determine the actual
	 * bit depth of the video data. However, it does affect the unit in
	 * which non-active timings (horizontal sync, front porch, back porch)
	 * are specified.
	 */
	dsi->format = MIPI_DSI_FMT_RGB888;

	ctx->dsc = lcm_dsc_config;

	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			 MIPI_DSI_MODE_NO_EOT_PACKET | MIPI_DSI_MODE_LPM | MIPI_DSI_CLOCK_NON_CONTINUOUS;//MIPI_DSI_MODE_VIDEO_BURST MIPI_DSI_MODE_VIDEO_SYNC_PULSE	


	ctx->panel.prepare_prev_first = true;

	ctx->panel.backlight = lcm_create_backlight(dsi);
	if (IS_ERR(ctx->panel.backlight))
		return dev_err_probe(dev, PTR_ERR(ctx->panel.backlight),
				     "Failed to create backlight\n");

	ctx->reset_gpio = devm_gpiod_get(ctx->dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio)) {
		dev_info(dev, "cannot get reset-gpios %ld\n",
			 PTR_ERR(ctx->reset_gpio));
		return PTR_ERR(ctx->reset_gpio);
	}
//	devm_gpiod_put(ctx->dev, ctx->reset_gpio);
#if 1
	ctx->powerdm_gpio =
		devm_gpiod_get(ctx->dev, "powerdm", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->powerdm_gpio)) {
		dev_err(ctx->dev, "%s: cannot get powerdm-gpio %ld\n",
			__func__, PTR_ERR(ctx->powerdm_gpio));
		return PTR_ERR(ctx->powerdm_gpio);
	}
//	devm_gpiod_put(ctx->dev, ctx->powerdm_gpio);
#endif
	ctx->bias_pos = devm_gpiod_get_index(ctx->dev, "bias", 0, GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->bias_pos)) {
		dev_info(dev, "cannot get bias-gpios 0 %ld\n",
			 PTR_ERR(ctx->bias_pos));
		return PTR_ERR(ctx->bias_pos);
	}
//	devm_gpiod_put(ctx->dev, ctx->bias_pos);

	ctx->bias_neg = devm_gpiod_get_index(ctx->dev, "bias", 1, GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->bias_neg)) {
		dev_info(dev, "cannot get bias-gpios 1 %ld\n",
			 PTR_ERR(ctx->bias_neg));
		return PTR_ERR(ctx->bias_neg);
	}
//	devm_gpiod_put(ctx->dev, ctx->bias_neg);
//	pr_info("%s+ yft-lcm-vtdr6126-drv- add  2222\n", __func__);
//	ctx->prepared = true;

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0)
		drm_panel_remove(&ctx->panel);

	pr_info("%s- xl,vtdr6126,dphy,vdo,120hz\n", __func__);

	return ret;
}

static void lcm_remove(struct mipi_dsi_device *dsi)
{
	struct lcm *ctx = mipi_dsi_get_drvdata(dsi);
#if defined(CONFIG_MTK_PANEL_EXT)
	struct mtk_panel_ctx *ext_ctx = find_panel_ctx(&ctx->panel);
#endif

	mipi_dsi_detach(dsi);
	drm_panel_remove(&ctx->panel);
#if defined(CONFIG_MTK_PANEL_EXT)
	mtk_panel_detach(ext_ctx);
	mtk_panel_remove(ext_ctx);
#endif

}

static const struct of_device_id lcm_of_match[] = {
	{
	    .compatible = "vtdr6126,dphy,vdo",
	},
	{}
};

MODULE_DEVICE_TABLE(of, lcm_of_match);

static struct mipi_dsi_driver lcm_driver = {
	.probe = lcm_probe,
	.remove = lcm_remove,
	.driver = {
		.name = "panel-vtdr6126-dphy-vdo-90hz-boe",
		.owner = THIS_MODULE,
		.of_match_table = lcm_of_match,
	},
};

module_mipi_dsi_driver(lcm_driver);

MODULE_AUTHOR("Cui Zhang <cui.zhang@mediatek.com>");
MODULE_DESCRIPTION("lcm vtdr6126 VDO Panel Driver");
MODULE_LICENSE("GPL v2");
