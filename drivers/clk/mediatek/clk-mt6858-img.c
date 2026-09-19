// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025 MediaTek Inc.
 *               KY Liu <ky.liu@mediatek.com>
 * Copyright (c) 2026 Jolla Mobile Ltd
 *               Nikolai Burov <nikolai.burov@jolla.com>
 */

#include <dt-bindings/clock/mediatek,mt6858-clk.h>

#include <linux/platform_device.h>

#include "clk-gate.h"
#include "clk-mtk.h"

static const struct mtk_gate_regs img_cg_regs = {
	.set_ofs = 0x4,
	.clr_ofs = 0x8,
	.sta_ofs = 0x0,
};

#define GATE_IMG(_id, _name, _parent, _shift)				\
	GATE_MTK_FLAGS(_id, _name, _parent, &img_cg_regs, _shift,	\
		       &mtk_clk_gate_ops_setclr, CLK_OPS_PARENT_ENABLE)

static const struct mtk_gate imgsys1_clks[] = {
	GATE_IMG(CLK_IMGSYS1_LARB9, "imgsys1_larb9", "img1", 0),
	GATE_IMG(CLK_IMGSYS1_LARB10, "imgsys1_larb10", "img1", 1),
	GATE_IMG(CLK_IMGSYS1_DIP, "imgsys1_dip", "img1", 2),
};

static const struct mtk_clk_desc imgsys1_desc = {
	.clks = imgsys1_clks,
	.num_clks = ARRAY_SIZE(imgsys1_clks),
	.need_runtime_pm = true,
};

static const struct mtk_gate imgsys2_clks[] = {
	GATE_IMG(CLK_IMGSYS2_LARB9, "imgsys2_larb9", "img1", 0),
	GATE_IMG(CLK_IMGSYS2_MFB, "imgsys2_mfb", "img1", 6),
	GATE_IMG(CLK_IMGSYS2_WPE, "imgsys2_wpe", "img1", 7),
	GATE_IMG(CLK_IMGSYS2_MSS, "imgsys2_mss", "img1", 8),
};

static const struct mtk_clk_desc imgsys2_desc = {
	.clks = imgsys2_clks,
	.num_clks = ARRAY_SIZE(imgsys2_clks),
	.need_runtime_pm = true,
};

static const struct mtk_gate ipe_clks[] = {
	GATE_IMG(CLK_IPE_LARB20, "ipe_larb20", "ipe", 1),
	GATE_IMG(CLK_IPE_SMI_SUBCOM, "ipe_smi_subcom", "ipe", 2),
	GATE_IMG(CLK_IPE_FD, "ipe_fd", "ipe", 3),
	GATE_IMG(CLK_IPE_RSC, "ipe_rsc", "ipe", 5),
};

static const struct mtk_clk_desc ipe_desc = {
	.clks = ipe_clks,
	.num_clks = ARRAY_SIZE(ipe_clks),
	.need_runtime_pm = true,
};

static const struct of_device_id of_match_clk_mt6858_img[] = {
	{ .compatible = "mediatek,mt6858-imgsys1", .data = &imgsys1_desc },
	{ .compatible = "mediatek,mt6858-imgsys2", .data = &imgsys2_desc },
	{ .compatible = "mediatek,mt6858-ipesys", .data = &ipe_desc },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, of_match_clk_mt6858_img);

static struct platform_driver clk_mt6858_img_drv = {
	.probe = mtk_clk_simple_probe,
	.remove = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6858-img",
		.of_match_table = of_match_clk_mt6858_img,
	},
};

MODULE_DESCRIPTION("MediaTek MT6858 image processing clock driver");
module_platform_driver(clk_mt6858_img_drv);
MODULE_LICENSE("GPL");
