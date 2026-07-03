// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025 MediaTek Inc.
 *               KY Liu <ky.liu@mediatek.com>
 * Copyright (c) 2026 Jolla Mobile Ltd
 */

#include <dt-bindings/clock/mediatek,mt6858-clk.h>

#include <linux/platform_device.h>

#include "clk-mtk.h"
#include "clk-pll.h"

#define ARMPLL_LL_CON0	0x208
#define ARMPLL_LL_CON1	0x20c
#define ARMPLL_BL_CON0	0x218
#define ARMPLL_BL_CON1	0x21c
#define CCIPLL_CON0	0x238
#define CCIPLL_CON1	0x23c
#define TVDPLL_CON0	0x248
#define TVDPLL_CON1	0x24c
#define UNIVPLL_CON0	0x308
#define UNIVPLL_CON1	0x30c
#define APLL1_CON0	0x328
#define APLL1_CON1	0x32c
#define APLL2_CON0	0x33c
#define APLL2_CON1	0x340
#define MAINPLL_CON0	0x350
#define MAINPLL_CON1	0x354
#define MSDCPLL_CON0	0x360
#define MSDCPLL_CON1	0x364
#define ADSPPLL_CON0	0x380
#define ADSPPLL_CON1	0x384
#define MMPLL_CON0	0x3a0
#define MMPLL_CON1	0x3a4
#define EMIPLL_CON0	0x3b0
#define EMIPLL_CON1	0x3b4

#define FENC_STATUS_CON0	0x0428

#define MT6858_PLL_FMAX		(3800UL * MHZ)
#define MT6858_PLL_FMIN		(1500UL * MHZ)
#define MT6858_INTEGER_BITS	8

#define PLL_FENC(_id, _name, _reg, _fenc_sta_ofs, _fenc_sta_bit,\
			_flags, _pd_reg, _pd_shift,		\
			_pcw_reg, _pcw_shift, _pcwbits) {	\
		.id = _id,					\
		.name = _name,					\
		.reg = _reg,					\
		.fenc_sta_ofs = _fenc_sta_ofs,			\
		.fenc_sta_bit = _fenc_sta_bit,			\
		.flags = _flags,				\
		.fmax = MT6858_PLL_FMAX,			\
		.fmin = MT6858_PLL_FMIN,			\
		.pd_reg = _pd_reg,				\
		.pd_shift = _pd_shift,				\
		.pcw_reg = _pcw_reg,				\
		.pcw_shift = _pcw_shift,			\
		.pcwbits = _pcwbits,				\
		.pcwibits = MT6858_INTEGER_BITS,		\
		.ops = &mtk_pll_fenc_clr_set_ops,		\
}

static const struct mtk_pll_data plls[] = {
	PLL_FENC(CLK_APMIXED_ARMPLL_LL, "armpll_ll",
		 ARMPLL_LL_CON0, FENC_STATUS_CON0, 2, PLL_AO,
		 ARMPLL_LL_CON1, 24, ARMPLL_LL_CON1, 0, 22),
	PLL_FENC(CLK_APMIXED_ARMPLL_BL, "armpll_bl",
		 ARMPLL_BL_CON0, FENC_STATUS_CON0, 1, PLL_AO,
		 ARMPLL_BL_CON1, 24, ARMPLL_BL_CON1, 0, 22),
	PLL_FENC(CLK_APMIXED_CCIPLL, "ccipll",
		 CCIPLL_CON0, FENC_STATUS_CON0, 0, PLL_AO,
		 CCIPLL_CON1, 24, CCIPLL_CON1, 0, 22),
	PLL_FENC(CLK_APMIXED_TVDPLL, "tvdpll",
		 TVDPLL_CON0, FENC_STATUS_CON0, 3, 0,
		 TVDPLL_CON1, 24, TVDPLL_CON1, 0, 22),
	PLL_FENC(CLK_APMIXED_UNIVPLL, "univpll",
		 UNIVPLL_CON0, FENC_STATUS_CON0, 10, HAVE_RST_BAR,
		 UNIVPLL_CON1, 24, UNIVPLL_CON1, 0, 22),
	PLL_FENC(CLK_APMIXED_APLL1, "apll1",
		 APLL1_CON0, FENC_STATUS_CON0, 6, 0,
		 APLL1_CON1, 24, APLL1_CON1, 0, 32),
	PLL_FENC(CLK_APMIXED_APLL2, "apll2",
		 APLL2_CON0, FENC_STATUS_CON0, 5, 0,
		 APLL2_CON1, 24, APLL2_CON1, 0, 32),
	PLL_FENC(CLK_APMIXED_MAINPLL, "mainpll",
		 MAINPLL_CON0, FENC_STATUS_CON0, 11, HAVE_RST_BAR,
		 MAINPLL_CON1, 24, MAINPLL_CON1, 0, 22),
	PLL_FENC(CLK_APMIXED_MSDCPLL, "msdcpll",
		 MSDCPLL_CON0, FENC_STATUS_CON0, 9, 0,
		 MSDCPLL_CON1, 24, MSDCPLL_CON1, 0, 22),
	PLL_FENC(CLK_APMIXED_ADSPPLL, "adsppll",
		 ADSPPLL_CON0, FENC_STATUS_CON0, 8, 0,
		 ADSPPLL_CON1, 24, ADSPPLL_CON1, 0, 22),
	PLL_FENC(CLK_APMIXED_MMPLL, "mmpll",
		 MMPLL_CON0, FENC_STATUS_CON0, 4, HAVE_RST_BAR,
		 MMPLL_CON1, 24, MMPLL_CON1, 0, 22),
	PLL_FENC(CLK_APMIXED_EMIPLL, "emipll",
		 EMIPLL_CON0, FENC_STATUS_CON0, 7, 0,
		 EMIPLL_CON1, 24, EMIPLL_CON1, 0, 22),
};

static int clk_mt6858_apmixed_probe(struct platform_device *pdev)
{
	struct clk_hw_onecell_data *clk_data;
	struct device_node *node = pdev->dev.of_node;
	int r;

	clk_data = mtk_alloc_clk_data(ARRAY_SIZE(plls));
	if (!clk_data)
		return -ENOMEM;

	r = mtk_clk_register_plls(&pdev->dev, plls, ARRAY_SIZE(plls), clk_data);
	if (r)
		goto free_apmixed_data;

	r = of_clk_add_hw_provider(node, of_clk_hw_onecell_get, clk_data);
	if (r)
		goto unregister_plls;

	platform_set_drvdata(pdev, clk_data);

	return r;

unregister_plls:
	mtk_clk_unregister_plls(plls, ARRAY_SIZE(plls), clk_data);
free_apmixed_data:
	mtk_free_clk_data(clk_data);
	return r;
}

static void clk_mt6858_apmixed_remove(struct platform_device *pdev)
{
	struct clk_hw_onecell_data *clk_data = platform_get_drvdata(pdev);
	struct device_node *node = pdev->dev.of_node;

	of_clk_del_provider(node);
	mtk_clk_unregister_plls(plls, ARRAY_SIZE(plls), clk_data);
	mtk_free_clk_data(clk_data);
}

static const struct of_device_id of_match_clk_mt6858_apmixed[] = {
	{ .compatible = "mediatek,mt6858-apmixedsys" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, of_match_clk_mt6858_apmixed);

static struct platform_driver clk_mt6858_apmixed_drv = {
	.probe = clk_mt6858_apmixed_probe,
	.remove = clk_mt6858_apmixed_remove,
	.driver = {
		.name = "clk-mt6858-apmixed",
		.of_match_table = of_match_clk_mt6858_apmixed,
	},
};

MODULE_DESCRIPTION("MediaTek MT6858 apmixedsys clock driver");
module_platform_driver(clk_mt6858_apmixed_drv);
MODULE_LICENSE("GPL");
