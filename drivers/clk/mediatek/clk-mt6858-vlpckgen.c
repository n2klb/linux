// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025 MediaTek Inc.
 *               KY Liu <ky.liu@mediatek.com>
 * Copyright (c) 2026 Jolla Mobile Ltd
 */

#include <dt-bindings/clock/mediatek,mt6858-clk.h>

#include <linux/platform_device.h>

#include "clk-mtk.h"
#include "clk-mux.h"

/* MUX SEL REG */
#define VLP_CLK_CFG_UPDATE		0x0004
#define VLP_CLK_CFG_0			0x0008
#define VLP_CLK_CFG_0_SET		0x000c
#define VLP_CLK_CFG_0_CLR		0x0010
#define VLP_CLK_CFG_1			0x0014
#define VLP_CLK_CFG_1_SET		0x0018
#define VLP_CLK_CFG_1_CLR		0x001c
#define VLP_CLK_CFG_2			0x0020
#define VLP_CLK_CFG_2_SET		0x0024
#define VLP_CLK_CFG_2_CLR		0x0028
#define VLP_CLK_CFG_3			0x002c
#define VLP_CLK_CFG_3_SET		0x0030
#define VLP_CLK_CFG_3_CLR		0x0034
#define VLP_CLK_CFG_4			0x0038
#define VLP_CLK_CFG_4_SET		0x003c
#define VLP_CLK_CFG_4_CLR		0x0040
#define VLP_CLK_CFG_5			0x0044
#define VLP_CLK_CFG_5_SET		0x0048
#define VLP_CLK_CFG_5_CLR		0x004c
#define VLP_CLK_CFG_6			0x0050
#define VLP_CLK_CFG_6_SET		0x0054
#define VLP_CLK_CFG_6_CLR		0x0058

/* MUX SHIFT */
#define TOP_MUX_SCP_SHIFT			0
#define TOP_MUX_PWRAP_ULPOSC_SHIFT		1
#define TOP_MUX_SPMI_P_MST_SHIFT		2
#define TOP_MUX_SPMI_M_MST_SHIFT		3
#define TOP_MUX_DVFSRC_SHIFT			4
#define TOP_MUX_PWM_VLP_SHIFT			5
#define TOP_MUX_AXI_VLP_SHIFT			6
#define TOP_MUX_SYSTIMER_26M_SHIFT		7
#define TOP_MUX_SSPM_SHIFT			8
#define TOP_MUX_SSPM_F26M_SHIFT			9
#define TOP_MUX_SRCK_SHIFT			10
#define TOP_MUX_SCP_SPI_SHIFT			11
#define TOP_MUX_SCP_IIC_SHIFT			12
#define TOP_MUX_SCP_IIC_HIGH_SPD_SHIFT		14
#define TOP_MUX_SSPM_ULPOSC_SHIFT		15
#define TOP_MUX_TIA_ULPOSC_SHIFT		16
#define TOP_MUX_APXGPT_26M_SHIFT		17
#define TOP_MUX_CAMTG0_SHIFT			18
#define TOP_MUX_CAMTG1_SHIFT			19
#define TOP_MUX_CAMTG2_SHIFT			20
#define TOP_MUX_CAMTG3_SHIFT			21
#define TOP_MUX_KP_IRQ_GEN_SHIFT		22
#define TOP_MUX_SSR_PKA_SHIFT			23
#define TOP_MUX_SSR_DMA_SHIFT			24
#define TOP_MUX_SSR_KDF_SHIFT			25
#define TOP_MUX_SSR_RNG_SHIFT			26

/* FENC REG */
#define VLP_OCIC_FENC_STATUS_MON_0	0x0328

static const char * const vlp_scp_parents[] = {
	"clk26m",
	"adsppll",
	"univpll_d4",
	"univpll_d3",
	"mainpll_d3",
	"univpll_d6",
	"apll1",
	"mainpll_d4",
	"mainpll_d7",
	"osc_d10"
};

static const char * const vlp_pwrap_ulposc_parents[] = {
	"clk26m",
	"osc_d10",
	"osc_d7",
	"osc_d8",
	"osc_d16",
	"mainpll_d7_d8"
};

static const char * const vlp_spmi_p_parents[] = {
	"clk26m",
	"clk13m",
	"osc_d8",
	"osc_d10",
	"osc_d16",
	"osc_d7",
	"clk32k",
	"mainpll_d7_d8",
	"mainpll_d6_d8",
	"mainpll_d5_d8"
};

static const char * const vlp_spmi_m_parents[] = {
	"clk26m",
	"clk13m",
	"osc_d8",
	"osc_d10",
	"osc_d16",
	"osc_d7",
	"clk32k",
	"mainpll_d7_d8",
	"mainpll_d6_d8",
	"mainpll_d5_d8"
};

static const char * const vlp_dvfsrc_parents[] = {
	"clk26m",
	"osc_d10"
};

static const char * const vlp_pwm_vlp_parents[] = {
	"clk26m",
	"osc_d4",
	"clk32k",
	"osc_d10",
	"mainpll_d4_d8"
};

static const char * const vlp_axi_vlp_parents[] = {
	"clk26m",
	"osc_d10",
	"osc_d2",
	"mainpll_d7_d4",
	"mainpll_d7_d2"
};

static const char * const vlp_systimer_26m_parents[] = {
	"clk26m",
	"osc_d10"
};

static const char * const vlp_sspm_parents[] = {
	"clk26m",
	"osc_d10",
	"mainpll_d5_d2",
	"ulposc",
	"mainpll_d6"
};

static const char * const vlp_sspm_f26m_parents[] = {
	"clk26m",
	"osc_d10"
};

static const char * const vlp_srck_parents[] = {
	"clk26m",
	"osc_d10"
};

static const char * const vlp_scp_spi_parents[] = {
	"clk26m",
	"mainpll_d5_d4",
	"mainpll_d7_d2",
	"osc_d10"
};

static const char * const vlp_scp_iic_parents[] = {
	"clk26m",
	"mainpll_d5_d4",
	"mainpll_d7_d2",
	"osc_d10"
};

static const char * const vlp_scp_iic_hs_parents[] = {
	"clk26m",
	"mainpll_d5_d4",
	"mainpll_d7_d2",
	"osc_d10"
};

static const char * const vlp_sspm_ulposc_parents[] = {
	"ulposc",
	"univpll_d5_d2",
	"clk26m"
};

static const char * const vlp_tia_ulposc_parents[] = {
	"clk26m",
	"osc_d10",
	"osc_d7",
	"osc_d8",
	"osc_d16",
	"mainpll_d7_d8"
};

static const char * const vlp_apxgpt_26m_parents[] = {
	"clk26m",
	"osc_d10"
};

static const char * const vlp_camtg0_parents[] = {
	"clk26m",
	"univpll_192m_d8",
	"univpll_d6_d8",
	"univpll_192m_d4",
	"univpll_d6_d16",
	"clk13m",
	"univpll_192m_d10",
	"univpll_192m_d16",
	"univpll_192m_d32"
};

static const char * const vlp_camtg1_parents[] = {
	"clk26m",
	"univpll_192m_d8",
	"univpll_d6_d8",
	"univpll_192m_d4",
	"univpll_d6_d16",
	"clk13m",
	"univpll_192m_d10",
	"univpll_192m_d16",
	"univpll_192m_d32"
};

static const char * const vlp_camtg2_parents[] = {
	"clk26m",
	"univpll_192m_d8",
	"univpll_d6_d8",
	"univpll_192m_d4",
	"univpll_d6_d16",
	"clk13m",
	"univpll_192m_d10",
	"univpll_192m_d16",
	"univpll_192m_d32"
};

static const char * const vlp_camtg3_parents[] = {
	"clk26m",
	"univpll_192m_d8",
	"univpll_d6_d8",
	"univpll_192m_d4",
	"univpll_d6_d16",
	"clk13m",
	"univpll_192m_d10",
	"univpll_192m_d16",
	"univpll_192m_d32"
};

static const char * const vlp_kp_irq_gen_parents[] = {
	"clk26m",
	"mainpll_d7_d4"
};

static const char * const vlp_ssr_pka_parents[] = {
	"clk26m",
	"mainpll_d4_d4",
	"mainpll_d4_d2",
	"mainpll_d7",
	"mainpll_d6",
	"mainpll_d5"
};

static const char * const vlp_ssr_dma_parents[] = {
	"clk26m",
	"mainpll_d4_d4",
	"mainpll_d4_d2",
	"mainpll_d7",
	"mainpll_d6",
	"mainpll_d5"
};

static const char * const vlp_ssr_kdf_parents[] = {
	"clk26m",
	"mainpll_d4_d4",
	"mainpll_d4_d2",
	"mainpll_d7",
	"mainpll_d6"
};

static const char * const vlp_ssr_rng_parents[] = {
	"clk26m",
	"mainpll_d4_d4",
	"mainpll_d5_d2",
	"mainpll_d4_d2"
};

static const struct mtk_mux vlp_top_muxes[] = {
	/* VLP_CLK_CFG_0 */
	MUX_GATE_FENC_CLR_SET_UPD(CLK_VLP_TOP_SCP, "vlp_scp",
		vlp_scp_parents, VLP_CLK_CFG_0, VLP_CLK_CFG_0_SET,
		VLP_CLK_CFG_0_CLR, 0, 4, 7, VLP_CLK_CFG_UPDATE,
		TOP_MUX_SCP_SHIFT, VLP_OCIC_FENC_STATUS_MON_0, 31),
	MUX_CLR_SET_UPD(CLK_VLP_TOP_PWRAP_ULPOSC, "vlp_pwrap_ulposc",
		vlp_pwrap_ulposc_parents, VLP_CLK_CFG_0, VLP_CLK_CFG_0_SET,
		VLP_CLK_CFG_0_CLR, 8, 3, VLP_CLK_CFG_UPDATE,
		TOP_MUX_PWRAP_ULPOSC_SHIFT),
	MUX_CLR_SET_UPD(CLK_VLP_TOP_SPMI_P_MST, "vlp_spmi_p",
		vlp_spmi_p_parents, VLP_CLK_CFG_0, VLP_CLK_CFG_0_SET,
		VLP_CLK_CFG_0_CLR, 16, 4, VLP_CLK_CFG_UPDATE,
		TOP_MUX_SPMI_P_MST_SHIFT),
	MUX_CLR_SET_UPD(CLK_VLP_TOP_SPMI_M_MST, "vlp_spmi_m",
		vlp_spmi_m_parents, VLP_CLK_CFG_0, VLP_CLK_CFG_0_SET,
		VLP_CLK_CFG_0_CLR, 24, 4, VLP_CLK_CFG_UPDATE,
		TOP_MUX_SPMI_M_MST_SHIFT),
	/* VLP_CLK_CFG_1 */
	MUX_CLR_SET_UPD(CLK_VLP_TOP_DVFSRC, "vlp_dvfsrc",
		vlp_dvfsrc_parents, VLP_CLK_CFG_1, VLP_CLK_CFG_1_SET,
		VLP_CLK_CFG_1_CLR, 0, 1, VLP_CLK_CFG_UPDATE,
		TOP_MUX_DVFSRC_SHIFT),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_VLP_TOP_PWM_VLP, "vlp_pwm_vlp",
		vlp_pwm_vlp_parents, VLP_CLK_CFG_1, VLP_CLK_CFG_1_SET,
		VLP_CLK_CFG_1_CLR, 8, 3, 15, VLP_CLK_CFG_UPDATE,
		TOP_MUX_PWM_VLP_SHIFT, VLP_OCIC_FENC_STATUS_MON_0, 26),
	MUX_CLR_SET_UPD(CLK_VLP_TOP_AXI_VLP, "vlp_axi_vlp",
		vlp_axi_vlp_parents, VLP_CLK_CFG_1, VLP_CLK_CFG_1_SET,
		VLP_CLK_CFG_1_CLR, 16, 3, VLP_CLK_CFG_UPDATE,
		TOP_MUX_AXI_VLP_SHIFT),
	MUX_CLR_SET_UPD(CLK_VLP_TOP_SYSTIMER_26M, "vlp_systimer_26m",
		vlp_systimer_26m_parents, VLP_CLK_CFG_1, VLP_CLK_CFG_1_SET,
		VLP_CLK_CFG_1_CLR, 24, 1, VLP_CLK_CFG_UPDATE,
		TOP_MUX_SYSTIMER_26M_SHIFT),
	/* VLP_CLK_CFG_2 */
	MUX_CLR_SET_UPD(CLK_VLP_TOP_SSPM, "vlp_sspm",
		vlp_sspm_parents, VLP_CLK_CFG_2, VLP_CLK_CFG_2_SET,
		VLP_CLK_CFG_2_CLR, 0, 3, VLP_CLK_CFG_UPDATE,
		TOP_MUX_SSPM_SHIFT),
	MUX_CLR_SET_UPD(CLK_VLP_TOP_SSPM_F26M, "vlp_sspm_f26m",
		vlp_sspm_f26m_parents, VLP_CLK_CFG_2, VLP_CLK_CFG_2_SET,
		VLP_CLK_CFG_2_CLR, 8, 1, VLP_CLK_CFG_UPDATE,
		TOP_MUX_SSPM_F26M_SHIFT),
	MUX_CLR_SET_UPD(CLK_VLP_TOP_SRCK, "vlp_srck",
		vlp_srck_parents, VLP_CLK_CFG_2, VLP_CLK_CFG_2_SET,
		VLP_CLK_CFG_2_CLR, 16, 1, VLP_CLK_CFG_UPDATE,
		TOP_MUX_SRCK_SHIFT),
	MUX_CLR_SET_UPD(CLK_VLP_TOP_SCP_SPI, "vlp_scp_spi",
		vlp_scp_spi_parents, VLP_CLK_CFG_2, VLP_CLK_CFG_2_SET,
		VLP_CLK_CFG_2_CLR, 24, 2, VLP_CLK_CFG_UPDATE,
		TOP_MUX_SCP_SPI_SHIFT),
	/* VLP_CLK_CFG_3 */
	MUX_CLR_SET_UPD(CLK_VLP_TOP_SCP_IIC, "vlp_scp_iic",
		vlp_scp_iic_parents, VLP_CLK_CFG_3, VLP_CLK_CFG_3_SET,
		VLP_CLK_CFG_3_CLR, 0, 2, VLP_CLK_CFG_UPDATE,
		TOP_MUX_SCP_IIC_SHIFT),
	MUX_CLR_SET_UPD(CLK_VLP_TOP_SCP_IIC_HIGH_SPD, "vlp_scp_iic_hs",
		vlp_scp_iic_hs_parents, VLP_CLK_CFG_3, VLP_CLK_CFG_3_SET,
		VLP_CLK_CFG_3_CLR, 16, 2, VLP_CLK_CFG_UPDATE,
		TOP_MUX_SCP_IIC_HIGH_SPD_SHIFT),
	MUX_CLR_SET_UPD(CLK_VLP_TOP_SSPM_ULPOSC, "vlp_sspm_ulposc",
		vlp_sspm_ulposc_parents, VLP_CLK_CFG_3, VLP_CLK_CFG_3_SET,
		VLP_CLK_CFG_3_CLR, 24, 2, VLP_CLK_CFG_UPDATE,
		TOP_MUX_SSPM_ULPOSC_SHIFT),
	/* VLP_CLK_CFG_4 */
	MUX_CLR_SET_UPD(CLK_VLP_TOP_TIA_ULPOSC, "vlp_tia_ulposc",
		vlp_tia_ulposc_parents, VLP_CLK_CFG_4, VLP_CLK_CFG_4_SET,
		VLP_CLK_CFG_4_CLR, 0, 3, VLP_CLK_CFG_UPDATE,
		TOP_MUX_TIA_ULPOSC_SHIFT),
	MUX_CLR_SET_UPD(CLK_VLP_TOP_APXGPT_26M, "vlp_apxgpt_26m",
		vlp_apxgpt_26m_parents, VLP_CLK_CFG_4, VLP_CLK_CFG_4_SET,
		VLP_CLK_CFG_4_CLR, 8, 1, VLP_CLK_CFG_UPDATE,
		TOP_MUX_APXGPT_26M_SHIFT),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_VLP_TOP_CAMTG0, "vlp_camtg0",
		vlp_camtg0_parents, VLP_CLK_CFG_4, VLP_CLK_CFG_4_SET,
		VLP_CLK_CFG_4_CLR, 16, 4, 23, VLP_CLK_CFG_UPDATE,
		TOP_MUX_CAMTG0_SHIFT, VLP_OCIC_FENC_STATUS_MON_0, 13),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_VLP_TOP_CAMTG1, "vlp_camtg1",
		vlp_camtg1_parents, VLP_CLK_CFG_4, VLP_CLK_CFG_4_SET,
		VLP_CLK_CFG_4_CLR, 24, 4, 31, VLP_CLK_CFG_UPDATE,
		TOP_MUX_CAMTG1_SHIFT, VLP_OCIC_FENC_STATUS_MON_0, 12),
	/* VLP_CLK_CFG_5 */
	MUX_GATE_FENC_CLR_SET_UPD(CLK_VLP_TOP_CAMTG2, "vlp_camtg2",
		vlp_camtg2_parents, VLP_CLK_CFG_5, VLP_CLK_CFG_5_SET,
		VLP_CLK_CFG_5_CLR, 0, 4, 7, VLP_CLK_CFG_UPDATE,
		TOP_MUX_CAMTG2_SHIFT, VLP_OCIC_FENC_STATUS_MON_0, 11),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_VLP_TOP_CAMTG3, "vlp_camtg3",
		vlp_camtg3_parents, VLP_CLK_CFG_5, VLP_CLK_CFG_5_SET,
		VLP_CLK_CFG_5_CLR, 8, 4, 15, VLP_CLK_CFG_UPDATE,
		TOP_MUX_CAMTG3_SHIFT, VLP_OCIC_FENC_STATUS_MON_0, 10),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_VLP_TOP_KP_IRQ_GEN, "vlp_kp_irq_gen",
		vlp_kp_irq_gen_parents, VLP_CLK_CFG_5, VLP_CLK_CFG_5_SET,
		VLP_CLK_CFG_5_CLR, 16, 1, 23, VLP_CLK_CFG_UPDATE,
		TOP_MUX_KP_IRQ_GEN_SHIFT, VLP_OCIC_FENC_STATUS_MON_0, 9),
	MUX_CLR_SET_UPD(CLK_VLP_TOP_SSR_PKA, "vlp_ssr_pka",
		vlp_ssr_pka_parents, VLP_CLK_CFG_5, VLP_CLK_CFG_5_SET,
		VLP_CLK_CFG_5_CLR, 24, 3, VLP_CLK_CFG_UPDATE,
		TOP_MUX_SSR_PKA_SHIFT),
	/* VLP_CLK_CFG_6 */
	MUX_CLR_SET_UPD(CLK_VLP_TOP_SSR_DMA, "vlp_ssr_dma",
		vlp_ssr_dma_parents, VLP_CLK_CFG_6, VLP_CLK_CFG_6_SET,
		VLP_CLK_CFG_6_CLR, 0, 3, VLP_CLK_CFG_UPDATE,
		TOP_MUX_SSR_DMA_SHIFT),
	MUX_CLR_SET_UPD(CLK_VLP_TOP_SSR_KDF, "vlp_ssr_kdf",
		vlp_ssr_kdf_parents, VLP_CLK_CFG_6, VLP_CLK_CFG_6_SET,
		VLP_CLK_CFG_6_CLR, 8, 3, VLP_CLK_CFG_UPDATE,
		TOP_MUX_SSR_KDF_SHIFT),
	MUX_CLR_SET_UPD(CLK_VLP_TOP_SSR_RNG, "vlp_ssr_rng",
		vlp_ssr_rng_parents, VLP_CLK_CFG_6, VLP_CLK_CFG_6_SET,
		VLP_CLK_CFG_6_CLR, 16, 2, VLP_CLK_CFG_UPDATE,
		TOP_MUX_SSR_RNG_SHIFT),
};

static const struct mtk_clk_desc vlpck_desc = {
	.mux_clks = vlp_top_muxes,
	.num_mux_clks = ARRAY_SIZE(vlp_top_muxes),
};

static const struct of_device_id of_match_clk_mt6858_vlpck[] = {
	{ .compatible = "mediatek,mt6858-vlp-cksys", .data = &vlpck_desc },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, of_match_clk_mt6858_vlpck);

static struct platform_driver clk_mt6858_vlpck_drv = {
	.probe = mtk_clk_simple_probe,
	.remove = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6858-vlpck",
		.of_match_table = of_match_clk_mt6858_vlpck,
	},
};

MODULE_DESCRIPTION("MediaTek MT6858 VLP clock generators driver");
module_platform_driver(clk_mt6858_vlpck_drv);
MODULE_LICENSE("GPL");
