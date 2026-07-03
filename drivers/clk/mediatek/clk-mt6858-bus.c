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

static const struct mtk_gate_regs ifrao0_cg_regs = {
	.set_ofs = 0x88,
	.clr_ofs = 0x8c,
	.sta_ofs = 0x94,
};

static const struct mtk_gate_regs ifrao1_cg_regs = {
	.set_ofs = 0xa4,
	.clr_ofs = 0xa8,
	.sta_ofs = 0xac,
};

static const struct mtk_gate_regs ifrao2_cg_regs = {
	.set_ofs = 0xc0,
	.clr_ofs = 0xc4,
	.sta_ofs = 0xc8,
};

static const struct mtk_gate_regs ifrao3_cg_regs = {
	.set_ofs = 0xe0,
	.clr_ofs = 0xe4,
	.sta_ofs = 0xe8,
};

#define GATE_IFRAO0(_id, _name, _parent, _shift)	\
	GATE_MTK(_id, _name, _parent, &ifrao0_cg_regs, _shift, &mtk_clk_gate_ops_setclr)

#define GATE_IFRAO1(_id, _name, _parent, _shift)	\
	GATE_MTK(_id, _name, _parent, &ifrao1_cg_regs, _shift, &mtk_clk_gate_ops_setclr)

#define GATE_IFRAO2(_id, _name, _parent, _shift)	\
	GATE_MTK(_id, _name, _parent, &ifrao2_cg_regs, _shift, &mtk_clk_gate_ops_setclr)

#define GATE_IFRAO3(_id, _name, _parent, _shift)	\
	GATE_MTK(_id, _name, _parent, &ifrao3_cg_regs, _shift, &mtk_clk_gate_ops_setclr)

static const struct mtk_gate ifrao_clks[] = {
	/* INFRA_INFRACFG_AO_REG0 */
	GATE_IFRAO0(CLK_IFRAO_CCIF1_AP, "ifrao_ccif1_ap", "axi", 12),
	GATE_IFRAO0(CLK_IFRAO_CCIF1_MD, "ifrao_ccif1_md", "axi", 13),
	GATE_IFRAO0(CLK_IFRAO_CCIF_AP, "ifrao_ccif_ap", "axi", 23),
	GATE_IFRAO0(CLK_IFRAO_CCIF_MD, "ifrao_ccif_md", "axi", 26),
	/* INFRA_INFRACFG_AO_REG1 */
	GATE_IFRAO1(CLK_IFRAO_CLDMA_BCLK, "ifrao_cldmabclk", "axi", 3),
	/* INFRA_INFRACFG_AO_REG2 */
	GATE_IFRAO2(CLK_IFRAO_CCIF5_MD, "ifrao_ccif5_md", "axi", 10),
	GATE_IFRAO2(CLK_IFRAO_CCIF2_AP, "ifrao_ccif2_ap", "axi", 16),
	GATE_IFRAO2(CLK_IFRAO_CCIF2_MD, "ifrao_ccif2_md", "axi", 17),
	GATE_IFRAO2(CLK_IFRAO_DPMAIF_MAIN, "ifrao_dpmaif_main", "dpmaif_main", 26),
	GATE_IFRAO2(CLK_IFRAO_CCIF4_MD, "ifrao_ccif4_md", "axi", 29),
	/* INFRA_INFRACFG_AO_REG3 */
	GATE_IFRAO3(CLK_IFRAO_RG_MMW_DPMAIF26M, "ifrao_dpmaif_26m", "clk26m", 17),
};

static const struct mtk_clk_desc ifrao_desc = {
	.clks = ifrao_clks,
	.num_clks = ARRAY_SIZE(ifrao_clks),
};

static const struct mtk_gate_regs perao0_cg_regs = {
	.set_ofs = 0x24,
	.clr_ofs = 0x28,
	.sta_ofs = 0x10,
};

static const struct mtk_gate_regs perao1_cg_regs = {
	.set_ofs = 0x2c,
	.clr_ofs = 0x30,
	.sta_ofs = 0x14,
};

static const struct mtk_gate_regs perao2_cg_regs = {
	.set_ofs = 0x34,
	.clr_ofs = 0x38,
	.sta_ofs = 0x18,
};

#define GATE_PERAO0(_id, _name, _parent, _shift)	\
	GATE_MTK(_id, _name, _parent, &perao0_cg_regs, _shift, &mtk_clk_gate_ops_setclr)

#define GATE_PERAO1(_id, _name, _parent, _shift)	\
	GATE_MTK(_id, _name, _parent, &perao1_cg_regs, _shift, &mtk_clk_gate_ops_setclr)

#define GATE_PERAO2(_id, _name, _parent, _shift)	\
	GATE_MTK(_id, _name, _parent, &perao2_cg_regs, _shift, &mtk_clk_gate_ops_setclr)

static const struct mtk_gate perao_clks[] = {
	/* PERAO0 */
	GATE_PERAO0(CLK_PERAO_UART0, "perao_uart0", "uart", 0),
	GATE_PERAO0(CLK_PERAO_UART1, "perao_uart1", "uart", 1),
	GATE_PERAO0(CLK_PERAO_UART2, "perao_uart2", "uart", 2),
	GATE_PERAO0(CLK_PERAO_UART3, "perao_uart3", "uart", 3),
	GATE_PERAO0(CLK_PERAO_PWM_H, "perao_pwm_h", "axi_p", 4),
	GATE_PERAO0(CLK_PERAO_PWM_B, "perao_pwm_b", "pwm", 5),
	GATE_PERAO0(CLK_PERAO_PWM_FB1, "perao_pwm_fb1", "pwm", 6),
	GATE_PERAO0(CLK_PERAO_PWM_FB2, "perao_pwm_fb2", "pwm", 7),
	GATE_PERAO0(CLK_PERAO_PWM_FB3, "perao_pwm_fb3", "pwm", 8),
	GATE_PERAO0(CLK_PERAO_PWM_FB4, "perao_pwm_fb4", "pwm", 9),
	GATE_PERAO0(CLK_PERAO_DISP_PWM0, "perao_disp_pwm0", "disp_pwm", 10),
	GATE_PERAO0(CLK_PERAO_DISP_PWM1, "perao_disp_pwm1", "disp_pwm", 11),
	GATE_PERAO0(CLK_PERAO_SPI0_B, "perao_spi0_b", "spi0", 12),
	GATE_PERAO0(CLK_PERAO_SPI1_B, "perao_spi1_b", "spi1", 13),
	GATE_PERAO0(CLK_PERAO_SPI2_B, "perao_spi2_b", "spi2", 14),
	GATE_PERAO0(CLK_PERAO_SPI3_B, "perao_spi3_b", "spi3", 15),
	GATE_PERAO0(CLK_PERAO_SPI4_B, "perao_spi4_b", "spi4", 16),
	GATE_PERAO0(CLK_PERAO_SPI5_B, "perao_spi5_b", "spi5", 17),
	GATE_PERAO0(CLK_PERAO_SPI6_B, "perao_spi6_b", "spi6", 18),
	GATE_PERAO0(CLK_PERAO_SPI7_B, "perao_spi7_b", "spi7", 19),
	GATE_PERAO0(CLK_PERAO_I2C, "perao_i2c", "axi_p", 28),
	GATE_PERAO0(CLK_PERAO_DMA_B, "perao_dma_b", "axi_p", 29),
	/* PERAO1 */
	GATE_PERAO1(CLK_PERAO_MSDC1, "perao_msdc1", "msdc30_1", 12),
	GATE_PERAO1(CLK_PERAO_MSDC1_H, "perao_msdc1_h", "msdc30_1_h", 13),
	GATE_PERAO1(CLK_PERAO_MSDC1_MST_F, "perao_msdc1_mst_f", "axi_p", 14),
	GATE_PERAO1(CLK_PERAO_MSDC1_SLV_H, "perao_msdc1_slv_h", "axi_p", 15),
	/* PERAO2 */
	GATE_PERAO2(CLK_PERAO_AUDIO0, "perao_audio0", "axi_p", 0),
	GATE_PERAO2(CLK_PERAO_AUDIO1, "perao_audio1", "axi_p", 1),
	GATE_PERAO2(CLK_PERAO_AUDIO2, "perao_audio2", "aud_intbus", 2),
};

static const struct mtk_clk_desc perao_desc = {
	.clks = perao_clks,
	.num_clks = ARRAY_SIZE(perao_clks),
};

static const struct of_device_id of_match_clk_mt6858_bus[] = {
	{ .compatible = "mediatek,mt6858-infracfg-ao", .data = &ifrao_desc },
	{ .compatible = "mediatek,mt6858-pericfg-ao", .data = &perao_desc },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, of_match_clk_mt6858_bus);

static struct platform_driver clk_mt6858_bus_drv = {
	.probe = mtk_clk_simple_probe,
	.remove = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6858-bus",
		.of_match_table = of_match_clk_mt6858_bus,
	},
};

MODULE_DESCRIPTION("MediaTek MT6858 bus clock driver");
module_platform_driver(clk_mt6858_bus_drv);
MODULE_LICENSE("GPL");
