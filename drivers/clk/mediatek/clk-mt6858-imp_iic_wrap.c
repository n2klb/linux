// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025 MediaTek Inc.
 *                    Guangjie Song <guangjie.song@mediatek.com>
 * Copyright (c) 2025 Collabora Ltd.
 *                    Laura Nao <laura.nao@collabora.com>
 * Copyright (c) 2026 Jolla Mobile Ltd
 */

#include <dt-bindings/clock/mediatek,mt6858-clk.h>

#include <linux/clk-provider.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/pm_clock.h>

#include "clk-gate.h"
#include "clk-mtk.h"

static const struct mtk_gate_regs imp_cg_regs = {
	.set_ofs = 0xe08,
	.clr_ofs = 0xe04,
	.sta_ofs = 0xe00,
};

#define GATE_IMP(_id, _name, _parent, _shift)				\
	GATE_MTK_FLAGS(_id, _name, _parent, &imp_cg_regs, _shift,	\
		       &mtk_clk_gate_ops_setclr, CLK_OPS_PARENT_ENABLE)

static const struct mtk_gate impc_clks[] = {
	GATE_IMP(CLK_IMPC_I2C1, "impc_i2c1", "i2c", 0),
	GATE_IMP(CLK_IMPC_I2C3, "impc_i2c3", "i2c", 1),
	GATE_IMP(CLK_IMPC_I2C5, "impc_i2c5", "i2c", 2),
	GATE_IMP(CLK_IMPC_I2C6, "impc_i2c6", "i2c", 3),
	GATE_IMP(CLK_IMPC_I2C10, "impc_i2c10", "i2c", 4),
	GATE_IMP(CLK_IMPC_I2C12, "impc_i2c12", "i2c", 5),
};

static const struct mtk_clk_desc impc_mcd = {
	.clks = impc_clks,
	.num_clks = ARRAY_SIZE(impc_clks),
	.need_runtime_pm = true,
	.need_runtime_pm_clk = true,
};

static const struct mtk_gate impes_clks[] = {
	GATE_IMP(CLK_IMPES_I2C4, "impes_i2c4", "i2c", 0),
	GATE_IMP(CLK_IMPES_I2C7, "impes_i2c7", "i2c", 1),
	GATE_IMP(CLK_IMPES_I2C8, "impes_i2c8", "i2c", 2),
};

static const struct mtk_clk_desc impes_mcd = {
	.clks = impes_clks,
	.num_clks = ARRAY_SIZE(impes_clks),
	.need_runtime_pm = true,
	.need_runtime_pm_clk = true,
};

static const struct mtk_gate imps_clks[] = {
	GATE_IMP(CLK_IMPS_I2C0, "imps_i2c0", "i2c", 0),
	GATE_IMP(CLK_IMPS_I2C2, "imps_i2c2", "i2c", 1),
	GATE_IMP(CLK_IMPS_I2C9, "imps_i2c9", "i2c", 2),
	GATE_IMP(CLK_IMPS_I2C11, "imps_i2c11", "i2c", 3),
};

static const struct mtk_clk_desc imps_mcd = {
	.clks = imps_clks,
	.num_clks = ARRAY_SIZE(imps_clks),
	.need_runtime_pm = true,
	.need_runtime_pm_clk = true,
};

static const struct of_device_id of_match_clk_mt6858_imp_iic_wrap[] = {
	{ .compatible = "mediatek,mt6858-imp-iic-wrap-c", .data = &impc_mcd },
	{ .compatible = "mediatek,mt6858-imp-iic-wrap-es", .data = &impes_mcd },
	{ .compatible = "mediatek,mt6858-imp-iic-wrap-s", .data = &imps_mcd },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, of_match_clk_mt6858_imp_iic_wrap);

static const struct dev_pm_ops clk_mt6858_imp_iic_wrap_pm = {
	SET_RUNTIME_PM_OPS(pm_clk_suspend, pm_clk_resume, NULL)
};

static struct platform_driver clk_mt6858_imp_iic_wrap_drv = {
	.probe = mtk_clk_simple_probe,
	.remove = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6858-imp_iic_wrap",
		.of_match_table = of_match_clk_mt6858_imp_iic_wrap,
		.pm = &clk_mt6858_imp_iic_wrap_pm,
	},
};
module_platform_driver(clk_mt6858_imp_iic_wrap_drv);

MODULE_DESCRIPTION("MediaTek MT6858 I2C Wrapper clocks driver");
MODULE_LICENSE("GPL");
