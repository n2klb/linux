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

static const struct mtk_gate_regs mm0_cg_regs = {
	.set_ofs = 0x104,
	.clr_ofs = 0x108,
	.sta_ofs = 0x100,
};

static const struct mtk_gate_regs mm1_cg_regs = {
	.set_ofs = 0x114,
	.clr_ofs = 0x118,
	.sta_ofs = 0x110,
};

#define GATE_MM0(_id, _name, _parent, _shift)	\
	GATE_MTK(_id, _name, _parent, &mm0_cg_regs, _shift, &mtk_clk_gate_ops_setclr)

#define GATE_MM1(_id, _name, _parent, _shift)	\
	GATE_MTK(_id, _name, _parent, &mm1_cg_regs, _shift, &mtk_clk_gate_ops_setclr)

static const struct mtk_gate dispsys_clks[] = {
	/* MM0 */
	GATE_MM0(CLK_MM_DISP_OVL0_2L, "mm_disp_ovl0_2l", "disp0", 0),
	GATE_MM0(CLK_MM_DISP_OVL1_2L, "mm_disp_ovl1_2l", "disp0", 1),
	GATE_MM0(CLK_MM_DISP_OVL2_2L, "mm_disp_ovl2_2l", "disp0", 2),
	GATE_MM0(CLK_MM_DISP_RSZ1, "mm_disp_rsz1", "disp0", 5),
	GATE_MM0(CLK_MM_DISP_RSZ0, "mm_disp_rsz0", "disp0", 6),
	GATE_MM0(CLK_MM_DISP_TDSHP0, "mm_disp_tdshp0", "disp0", 7),
	GATE_MM0(CLK_MM_DISP_C3D0, "mm_disp_c3d0", "disp0", 8),
	GATE_MM0(CLK_MM_DISP_COLOR0, "mm_disp_color0", "disp0", 9),
	GATE_MM0(CLK_MM_DISP_CCORR0, "mm_disp_ccorr0", "disp0", 10),
	GATE_MM0(CLK_MM_DISP_CCORR1, "mm_disp_ccorr1", "disp0", 11),
	GATE_MM0(CLK_MM_DISP_AAL0, "mm_disp_aal0", "disp0", 12),
	GATE_MM0(CLK_MM_DISP_GAMMA0, "mm_disp_gamma0", "disp0", 13),
	GATE_MM0(CLK_MM_DISP_POSTMASK0, "mm_disp_postmask0", "disp0", 14),
	GATE_MM0(CLK_MM_DISP_DITHER0, "mm_disp_dither0", "disp0", 15),
	GATE_MM0(CLK_MM_DISP_TDSHP1, "mm_disp_tdshp1", "disp0", 16),
	GATE_MM0(CLK_MM_DISP_C3D1, "mm_disp_c3d1", "disp0", 17),
	GATE_MM0(CLK_MM_DISP_CCORR2, "mm_disp_ccorr2", "disp0", 18),
	GATE_MM0(CLK_MM_DISP_CCORR3, "mm_disp_ccorr3", "disp0", 19),
	GATE_MM0(CLK_MM_DISP_GAMMA1, "mm_disp_gamma1", "disp0", 20),
	GATE_MM0(CLK_MM_DISP_DITHER1, "mm_disp_dither1", "disp0", 21),
	GATE_MM0(CLK_MM_DISP_DSC_WRAP0, "mm_disp_dsc_wrap0", "disp0", 23),
	GATE_MM0(CLK_MM_DISP_DSI0, "mm_disp_dsi0", "disp0", 24),
	GATE_MM0(CLK_MM_DISP_WDMA1, "mm_disp_wdma1", "disp0", 26),
	GATE_MM0(CLK_MM_DISP_APB_BUS, "mm_disp_apb_bus", "disp0", 27),
	GATE_MM0(CLK_MM_DISP_FAKE_ENG0, "mm_disp_fake_eng0", "disp0", 28),
	GATE_MM0(CLK_MM_DISP_FAKE_ENG1, "mm_disp_fake_eng1", "disp0", 29),
	GATE_MM0(CLK_MM_DISP_MUTEX0, "mm_disp_mutex0", "disp0", 30),
	GATE_MM0(CLK_MM_SMI_COMMON, "mm_smi_common", "disp0", 31),
	/* MM1 */
	GATE_MM1(CLK_MM_DSI0, "mm_dsi0", "disp0", 0),
	GATE_MM1(CLK_MM_26M, "mm_26m", "clk26m", 11),
};

static const struct mtk_clk_desc dispsys_desc = {
	.clks = dispsys_clks,
	.num_clks = ARRAY_SIZE(dispsys_clks),
};

static const struct mtk_gate mminfra_clks[] = {
	/* MMINFRA_CONFIG0 */
	GATE_MM0(CLK_MMINFRA_GCE_D, "mminfra_gce_d", "mminfra", 0),
	GATE_MM0(CLK_MMINFRA_GCE_M, "mminfra_gce_m", "mminfra", 1),
	/* MMINFRA_CONFIG1 */
	GATE_MM1(CLK_MMINFRA_GCE_26M, "mminfra_gce_26m", "clk26m", 17),
};

static const struct mtk_clk_desc mminfra_desc = {
	.clks = mminfra_clks,
	.num_clks = ARRAY_SIZE(mminfra_clks),
};

static const struct of_device_id of_match_clk_mt6858_mminfra[] = {
	{ .compatible = "mediatek,mt6858-mm-infra", .data = &mminfra_desc },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, of_match_clk_mt6858_mminfra);

static struct platform_driver clk_mt6858_mminfra_drv = {
	.probe = mtk_clk_simple_probe,
	.remove = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6858-mminfra",
		.of_match_table = of_match_clk_mt6858_mminfra,
	},
};

static const struct platform_device_id clk_mt6858_dispsys_id_table[] = {
	{ .name = "clk-mt6858-dispsys", .driver_data = (kernel_ulong_t)&dispsys_desc },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(platform, clk_mt6858_dispsys_id_table);

static struct platform_driver clk_mt6858_dispsys_drv = {
	.probe = mtk_clk_pdev_probe,
	.remove = mtk_clk_pdev_remove,
	.driver = {
		.name = "clk-mt6858-dispsys",
	},
	.id_table = clk_mt6858_dispsys_id_table,
};

static int __init clk_mt6858_disp_init(void)
{
	int ret = platform_driver_register(&clk_mt6858_mminfra_drv);

	if (ret)
		return ret;

	return platform_driver_register(&clk_mt6858_dispsys_drv);
}
subsys_initcall(clk_mt6858_disp_init);

static void __exit clk_mt6858_disp_exit(void)
{
	platform_driver_unregister(&clk_mt6858_dispsys_drv);
	platform_driver_unregister(&clk_mt6858_mminfra_drv);
}
module_exit(clk_mt6858_disp_exit);

MODULE_DESCRIPTION("MediaTek MT6858 display subsystem clock driver");
MODULE_LICENSE("GPL");
