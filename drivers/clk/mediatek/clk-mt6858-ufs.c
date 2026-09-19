// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025 MediaTek Inc.
 *               KY Liu <ky.liu@mediatek.com>
 * Copyright (c) 2026 Jolla Mobile Ltd
 */

#include <dt-bindings/clock/mediatek,mt6858-clk.h>
#include <dt-bindings/reset/mediatek,mt6858-resets.h>

#include <linux/platform_device.h>

#include "clk-gate.h"
#include "clk-mtk.h"

static const struct mtk_gate_regs ufs_cg_regs = {
	.set_ofs = 0x8,
	.clr_ofs = 0xc,
	.sta_ofs = 0x4,
};

static u16 ufscfg_rst_ofs[] = {
	0x48,
};

#define GATE_UFS(_id, _name, _parent, _shift) {		\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &ufs_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

static const struct mtk_gate ufs_ao_clks[] = {
	GATE_UFS(CLK_UFSAO_UNIPRO_TX_SYM, "unipro_tx_sym", "clk26m", 0),
	GATE_UFS(CLK_UFSAO_UNIPRO_SYS, "unipro_sys", "ufs", 3),
	GATE_UFS(CLK_UFSAO_UNIPRO_SAP_CFG, "unipro_sap_cfg", "clk26m", 8),
};

static u16 ufs_ao_rst_idx_map[] = {
	[MT6858_UFSAO_RST_UFS_MPHY] = 8,
};

static const struct mtk_clk_rst_desc ufs_ao_rst_desc = {
	.version = MTK_RST_SET_CLR,
	.rst_bank_ofs = ufscfg_rst_ofs,
	.rst_bank_nr = ARRAY_SIZE(ufscfg_rst_ofs),
	.rst_idx_map = ufs_ao_rst_idx_map,
	.rst_idx_map_nr = ARRAY_SIZE(ufs_ao_rst_idx_map),
};

static const struct mtk_clk_desc ufs_ao_desc = {
	.clks = ufs_ao_clks,
	.num_clks = ARRAY_SIZE(ufs_ao_clks),
	.rst_desc = &ufs_ao_rst_desc,
};

static const struct mtk_gate ufs_pdn_clks[] = {
	GATE_UFS(CLK_UFSPDN_UFSHCI_UFS, "ufshci_ufs", "ufs", 0),
	GATE_UFS(CLK_UFSPDN_UFSHCI_AES, "ufshci_aes", "aes_ufsfde", 1),
	GATE_UFS(CLK_UFSPDN_UFSHCI_UFS_AHB, "ufshci_ufs_ahb", "axi_ufs", 3),
	GATE_UFS(CLK_UFSPDN_UFS_26M, "ufs_26m", "clk26m", 6),
};

static u16 ufs_pdn_rst_idx_map[] = {
	[MT6858_UFSPDN_RST_UFS_UNIPRO] = 0,
	[MT6858_UFSPDN_RST_UFS_CRYPTO] = 1,
	[MT6858_UFSPDN_RST_UFSHCI] = 2,
};

static const struct mtk_clk_rst_desc ufs_pdn_rst_desc = {
	.version = MTK_RST_SET_CLR,
	.rst_bank_ofs = ufscfg_rst_ofs,
	.rst_bank_nr = ARRAY_SIZE(ufscfg_rst_ofs),
	.rst_idx_map = ufs_pdn_rst_idx_map,
	.rst_idx_map_nr = ARRAY_SIZE(ufs_pdn_rst_idx_map),
};

static const struct mtk_clk_desc ufs_pdn_desc = {
	.clks = ufs_pdn_clks,
	.num_clks = ARRAY_SIZE(ufs_pdn_clks),
	.rst_desc = &ufs_pdn_rst_desc,
};

static const struct of_device_id of_match_clk_mt6858_ufs[] = {
	{ .compatible = "mediatek,mt6858-ufscfg-ao", .data = &ufs_ao_desc },
	{ .compatible = "mediatek,mt6858-ufscfg-pdn", .data = &ufs_pdn_desc },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, of_match_clk_mt6858_ufs);

static struct platform_driver clk_mt6858_ufs_drv = {
	.probe = mtk_clk_simple_probe,
	.remove = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6858-ufs",
		.of_match_table = of_match_clk_mt6858_ufs,
	},
};

MODULE_DESCRIPTION("MediaTek MT6858 UFS clock driver");
module_platform_driver(clk_mt6858_ufs_drv);
MODULE_LICENSE("GPL");
