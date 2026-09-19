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
#define CLK_CFG_UPDATE				0x0004
#define CLK_CFG_UPDATE1				0x0008
#define CLK_CFG_0				0x0010
#define CLK_CFG_0_SET				0x0014
#define CLK_CFG_0_CLR				0x0018
#define CLK_CFG_1				0x0020
#define CLK_CFG_1_SET				0x0024
#define CLK_CFG_1_CLR				0x0028
#define CLK_CFG_2				0x0030
#define CLK_CFG_2_SET				0x0034
#define CLK_CFG_2_CLR				0x0038
#define CLK_CFG_3				0x0040
#define CLK_CFG_3_SET				0x0044
#define CLK_CFG_3_CLR				0x0048
#define CLK_CFG_4				0x0050
#define CLK_CFG_4_SET				0x0054
#define CLK_CFG_4_CLR				0x0058
#define CLK_CFG_5				0x0060
#define CLK_CFG_5_SET				0x0064
#define CLK_CFG_5_CLR				0x0068
#define CLK_CFG_6				0x0070
#define CLK_CFG_6_SET				0x0074
#define CLK_CFG_6_CLR				0x0078
#define CLK_CFG_7				0x0080
#define CLK_CFG_7_SET				0x0084
#define CLK_CFG_7_CLR				0x0088
#define CLK_CFG_8				0x0090
#define CLK_CFG_8_SET				0x0094
#define CLK_CFG_8_CLR				0x0098
#define CLK_CFG_9				0x00A0
#define CLK_CFG_9_SET				0x00A4
#define CLK_CFG_9_CLR				0x00A8
#define CLK_CFG_10				0x00B0
#define CLK_CFG_10_SET				0x00B4
#define CLK_CFG_10_CLR				0x00B8
#define CLK_CFG_11				0x00C0
#define CLK_CFG_11_SET				0x00C4
#define CLK_CFG_11_CLR				0x00C8
#define CLK_CFG_12				0x00D0
#define CLK_CFG_12_SET				0x00D4
#define CLK_CFG_12_CLR				0x00D8
#define CLK_CFG_13				0x00E0
#define CLK_CFG_13_SET				0x00E4
#define CLK_CFG_13_CLR				0x00E8
#define CLK_CFG_14				0x00F0
#define CLK_CFG_14_SET				0x00F4
#define CLK_CFG_14_CLR				0x00F8
#define CLK_CFG_15				0x0100
#define CLK_CFG_15_SET				0x0104
#define CLK_CFG_15_CLR				0x0108
#define CLK_AUDDIV_0				0x0320

/* MUX SHIFT */
#define TOP_MUX_AXI_SHIFT			0
#define TOP_MUX_AXI_PERI_SHIFT			1
#define TOP_MUX_AXI_UFS_SHIFT			2
#define TOP_MUX_BUS_AXIMEM_SHIFT		3
#define TOP_MUX_DISP0_SHIFT			4
#define TOP_MUX_MMINFRA_SHIFT			5
#define TOP_MUX_MMUP_SHIFT			6
#define TOP_MUX_UART_SHIFT			7
#define TOP_MUX_SPI0_SHIFT			8
#define TOP_MUX_SPI1_SHIFT			9
#define TOP_MUX_SPI2_SHIFT			10
#define TOP_MUX_SPI3_SHIFT			11
#define TOP_MUX_SPI4_SHIFT			12
#define TOP_MUX_SPI5_SHIFT			13
#define TOP_MUX_SPI6_SHIFT			14
#define TOP_MUX_SPI7_SHIFT			15
#define TOP_MUX_MSDC_MACRO_1P_SHIFT		16
#define TOP_MUX_MSDC30_1_SHIFT			17
#define TOP_MUX_MSDC30_1_HCLK_SHIFT		18
#define TOP_MUX_AUD_INTBUS_SHIFT		19
#define TOP_MUX_ATB_SHIFT			20
#define TOP_MUX_DISP_PWM_SHIFT			21
#define TOP_MUX_USB_TOP_SHIFT			22
#define TOP_MUX_SSUSB_XHCI_SHIFT		23
#define TOP_MUX_I2C_SHIFT			24
#define TOP_MUX_SENINF_SHIFT			25
#define TOP_MUX_SENINF1_SHIFT			26
#define TOP_MUX_SENINF2_SHIFT			27
#define TOP_MUX_AUD_ENGEN1_SHIFT		28
#define TOP_MUX_AUD_ENGEN2_SHIFT		29
#define TOP_MUX_AES_UFSFDE_SHIFT		30
#define TOP_MUX_UFS_SHIFT			0
#define TOP_MUX_AUD_1_SHIFT			2
#define TOP_MUX_AUD_2_SHIFT			3
#define TOP_MUX_DPMAIF_MAIN_SHIFT		4
#define TOP_MUX_VENC_SHIFT			5
#define TOP_MUX_VDEC_SHIFT			6
#define TOP_MUX_PWM_SHIFT			7
#define TOP_MUX_AUDIO_H_SHIFT			8
#define TOP_MUX_MCUPM_SHIFT			9
#define TOP_MUX_MEM_SUB_SHIFT			10
#define TOP_MUX_MEM_SUB_PERI_SHIFT		11
#define TOP_MUX_MEM_SUB_UFS_SHIFT		12
#define TOP_MUX_EMI_N_SHIFT			13
#define TOP_MUX_DSI_OCC_SHIFT			14
#define TOP_MUX_AP2CONN_HOST_SHIFT		15
#define TOP_MUX_IMG1_SHIFT			16
#define TOP_MUX_IPE_SHIFT			17
#define TOP_MUX_CAM_SHIFT			18
#define TOP_MUX_CAMTM_SHIFT			19
#define TOP_MUX_EMI_INTERFACE_546_SHIFT		20
#define TOP_MUX_MFG_REF_SHIFT			22
#define TOP_MUX_MFGSC_REF_SHIFT			23
#define TOP_MUX_EFUSE_SHIFT			24
#define TOP_MUX_UNIPLL_SES_SHIFT		26
#define TOP_MUX_DRAMULP_SHIFT			28
#define TOP_MUX_SSUSB_FRMCNT_SHIFT		29

/* CKSTA REG */
#define CKSTA_REG				0x0230
#define CKSTA_REG2				0x0238

/* DIVIDER REG */
#define CLK_AUDDIV_2				0x0328

/* FENC REG */
#define CLK_FENC_STATUS_MON_0			0x0534
#define CLK_FENC_STATUS_MON_1			0x0538

static const struct mtk_fixed_factor top_divs[] = {
	FACTOR(CLK_TOP_MAINPLL_D3, "mainpll_d3", "mainpll", 1, 3),
	FACTOR(CLK_TOP_MAINPLL_D4, "mainpll_d4", "mainpll", 1, 4),
	FACTOR(CLK_TOP_MAINPLL_D4_D2, "mainpll_d4_d2", "mainpll_d4", 1, 2),
	FACTOR(CLK_TOP_MAINPLL_D4_D4, "mainpll_d4_d4", "mainpll_d4", 1, 4),
	FACTOR(CLK_TOP_MAINPLL_D4_D8, "mainpll_d4_d8", "mainpll_d4", 1, 8),
	FACTOR(CLK_TOP_MAINPLL_D5, "mainpll_d5", "mainpll", 1, 5),
	FACTOR(CLK_TOP_MAINPLL_D5_D2, "mainpll_d5_d2", "mainpll_d5", 1, 2),
	FACTOR(CLK_TOP_MAINPLL_D5_D4, "mainpll_d5_d4", "mainpll_d5", 1, 4),
	FACTOR(CLK_TOP_MAINPLL_D5_D8, "mainpll_d5_d8", "mainpll_d5", 1, 8),
	FACTOR(CLK_TOP_MAINPLL_D6, "mainpll_d6", "mainpll", 1, 6),
	FACTOR(CLK_TOP_MAINPLL_D6_D2, "mainpll_d6_d2", "mainpll_d6", 1, 2),
	FACTOR(CLK_TOP_MAINPLL_D6_D8, "mainpll_d6_d8", "mainpll_d6", 1, 8),
	FACTOR(CLK_TOP_MAINPLL_D7, "mainpll_d7", "mainpll", 1, 7),
	FACTOR(CLK_TOP_MAINPLL_D7_D2, "mainpll_d7_d2", "mainpll_d7", 1, 2),
	FACTOR(CLK_TOP_MAINPLL_D7_D4, "mainpll_d7_d4", "mainpll_d7", 1, 4),
	FACTOR(CLK_TOP_MAINPLL_D7_D8, "mainpll_d7_d8", "mainpll_d7", 1, 8),
	FACTOR(CLK_TOP_UNIVPLL_D2, "univpll_d2", "univpll", 1, 2),
	FACTOR(CLK_TOP_UNIVPLL_D3, "univpll_d3", "univpll", 1, 3),
	FACTOR(CLK_TOP_UNIVPLL_D4, "univpll_d4", "univpll", 1, 4),
	FACTOR(CLK_TOP_UNIVPLL_D4_D2, "univpll_d4_d2", "univpll_d4", 1, 2),
	FACTOR(CLK_TOP_UNIVPLL_D4_D4, "univpll_d4_d4", "univpll_d4", 1, 4),
	FACTOR(CLK_TOP_UNIVPLL_D4_D8, "univpll_d4_d8", "univpll_d4", 1, 8),
	FACTOR(CLK_TOP_UNIVPLL_D5, "univpll_d5", "univpll", 1, 5),
	FACTOR(CLK_TOP_UNIVPLL_D5_D2, "univpll_d5_d2", "univpll_d5", 1, 2),
	FACTOR(CLK_TOP_UNIVPLL_D5_D4, "univpll_d5_d4", "univpll_d5", 1, 4),
	FACTOR(CLK_TOP_UNIVPLL_D6, "univpll_d6", "univpll", 1, 6),
	FACTOR(CLK_TOP_UNIVPLL_D6_D2, "univpll_d6_d2", "univpll_d6", 1, 2),
	FACTOR(CLK_TOP_UNIVPLL_D6_D4, "univpll_d6_d4", "univpll_d6", 1, 4),
	FACTOR(CLK_TOP_UNIVPLL_D6_D8, "univpll_d6_d8", "univpll_d6", 1, 8),
	FACTOR(CLK_TOP_UNIVPLL_D6_D16, "univpll_d6_d16", "univpll_d6", 1, 16),
	FACTOR(CLK_TOP_UNIVPLL_D7, "univpll_d7", "univpll", 1, 7),
	FACTOR(CLK_TOP_UNIVPLL_D7_D2, "univpll_d7_d2", "univpll_d7", 1, 2),
	FACTOR(CLK_TOP_UNIVPLL_192M, "univpll_192m", "univpll", 1, 13),
	FACTOR(CLK_TOP_UNIVPLL_192M_D4, "univpll_192m_d4", "univpll_192m", 1, 4),
	FACTOR(CLK_TOP_UNIVPLL_192M_D8, "univpll_192m_d8", "univpll_192m", 1, 8),
	FACTOR(CLK_TOP_UNIVPLL_192M_D10, "univpll_192m_d10", "univpll_192m", 1, 10),
	FACTOR(CLK_TOP_UNIVPLL_192M_D16, "univpll_192m_d16", "univpll_192m", 1, 16),
	FACTOR(CLK_TOP_UNIVPLL_192M_D32, "univpll_192m_d32", "univpll_192m", 1, 32),
	FACTOR(CLK_TOP_APLL1_D2, "apll1_d2", "apll1", 1, 2),
	FACTOR(CLK_TOP_APLL1_D4, "apll1_d4", "apll1", 1, 4),
	FACTOR(CLK_TOP_APLL1_D8, "apll1_d8", "apll1", 1, 8),
	FACTOR(CLK_TOP_APLL2_D2, "apll2_d2", "apll2", 1, 2),
	FACTOR(CLK_TOP_APLL2_D4, "apll2_d4", "apll2", 1, 4),
	FACTOR(CLK_TOP_APLL2_D8, "apll2_d8", "apll2", 1, 8),
	FACTOR(CLK_TOP_MSDCPLL_D2, "msdcpll_d2", "msdcpll", 1, 2),
	FACTOR(CLK_TOP_OSC_D2, "osc_d2", "ulposc", 1, 2),
	FACTOR(CLK_TOP_OSC_D4, "osc_d4", "ulposc", 1, 4),
	FACTOR(CLK_TOP_OSC_D7, "osc_d7", "ulposc", 1, 7),
	FACTOR(CLK_TOP_OSC_D8, "osc_d8", "ulposc", 1, 8),
	FACTOR(CLK_TOP_OSC_D10, "osc_d10", "ulposc", 1, 10),
	FACTOR(CLK_TOP_OSC_D16, "osc_d16", "ulposc", 1, 16),
	FACTOR(CLK_TOP_MMPLL_D4, "mmpll_d4", "mmpll", 1, 4),
	FACTOR(CLK_TOP_MMPLL_D4_D2, "mmpll_d4_d2", "mmpll_d4", 1, 2),
	FACTOR(CLK_TOP_MMPLL_D4_D4, "mmpll_d4_d4", "mmpll_d4", 1, 4),
	FACTOR(CLK_TOP_MMPLL_D5, "mmpll_d5", "mmpll", 1, 5),
	FACTOR(CLK_TOP_MMPLL_D5_D2, "mmpll_d5_d2", "mmpll_d5", 1, 2),
	FACTOR(CLK_TOP_MMPLL_D6, "mmpll_d6", "mmpll", 1, 6),
	FACTOR(CLK_TOP_MMPLL_D6_D2, "mmpll_d6_d2", "mmpll_d6", 1, 2),
	FACTOR(CLK_TOP_MMPLL_D7, "mmpll_d7", "mmpll", 1, 7),
	FACTOR(CLK_TOP_MMPLL_D9, "mmpll_d9", "mmpll", 1, 9),
};

static const char * const axi_parents[] = {
	"clk26m",
	"mainpll_d4_d4",
	"mainpll_d7_d2",
	"mainpll_d4_d2",
	"mainpll_d5_d2",
	"mainpll_d6_d2",
	"osc_d4"
};

static const char * const axi_p_parents[] = {
	"clk26m",
	"mainpll_d4_d4",
	"mainpll_d7_d2",
	"osc_d4"
};

static const char * const axi_ufs_parents[] = {
	"clk26m",
	"mainpll_d4_d8",
	"mainpll_d7_d4",
	"osc_d8"
};

static const char * const bus_aximem_parents[] = {
	"clk26m",
	"mainpll_d7_d2",
	"mainpll_d5_d2",
	"mainpll_d4_d2",
	"mainpll_d6"
};

static const char * const disp0_parents[] = {
	"clk26m",
	"univpll_d5_d2",
	"mmpll_d6_d2",
	"mmpll_d4_d2",
	"univpll_d6",
	"mainpll_d5",
	"tvdpll",
	"mmpll_d6",
	"mainpll_d5_d2",
	"mmpll_d4_d4"
};

static const char * const mminfra_parents[] = {
	"clk26m",
	"osc_d2",
	"mainpll_d5_d2",
	"mmpll_d6_d2",
	"mainpll_d4_d2",
	"mmpll_d4_d2",
	"mainpll_d6",
	"mmpll_d7",
	"univpll_d6",
	"mainpll_d5",
	"mmpll_d6",
	"univpll_d5",
	"mainpll_d4",
	"univpll_d4",
	"mmpll_d4",
	"mmpll_d5_d2"
};

static const char * const mmup_parents[] = {
	"clk26m",
	"mainpll_d5_d2",
	"mmpll_d4_d2",
	"mainpll_d4",
	"univpll_d4",
	"mmpll_d4",
	"mainpll_d3"
};

static const char * const uart_parents[] = {
	"clk26m",
	"univpll_d6_d8"
};

static const char * const spi0_parents[] = {
	"clk26m",
	"univpll_d6_d2",
	"univpll_192m",
	"mainpll_d6_d2",
	"univpll_d4_d4",
	"mainpll_d4_d4",
	"univpll_d5_d4",
	"univpll_d6_d4"
};

static const char * const spi1_parents[] = {
	"clk26m",
	"univpll_d6_d2",
	"univpll_192m",
	"mainpll_d6_d2",
	"univpll_d4_d4",
	"mainpll_d4_d4",
	"univpll_d5_d4",
	"univpll_d6_d4"
};

static const char * const spi2_parents[] = {
	"clk26m",
	"univpll_d6_d2",
	"univpll_192m",
	"mainpll_d6_d2",
	"univpll_d4_d4",
	"mainpll_d4_d4",
	"univpll_d5_d4",
	"univpll_d6_d4"
};

static const char * const spi3_parents[] = {
	"clk26m",
	"univpll_d6_d2",
	"univpll_192m",
	"mainpll_d6_d2",
	"univpll_d4_d4",
	"mainpll_d4_d4",
	"univpll_d5_d4",
	"univpll_d6_d4"
};

static const char * const spi4_parents[] = {
	"clk26m",
	"univpll_d6_d2",
	"univpll_192m",
	"mainpll_d6_d2",
	"univpll_d4_d4",
	"mainpll_d4_d4",
	"univpll_d5_d4",
	"univpll_d6_d4"
};

static const char * const spi5_parents[] = {
	"clk26m",
	"univpll_d6_d2",
	"univpll_192m",
	"mainpll_d6_d2",
	"univpll_d4_d4",
	"mainpll_d4_d4",
	"univpll_d5_d4",
	"univpll_d6_d4"
};

static const char * const spi6_parents[] = {
	"clk26m",
	"univpll_d6_d2",
	"mainpll_d6_d2",
	"univpll_d4_d4",
	"mainpll_d4_d4",
	"univpll_d5_d4",
	"univpll_d6_d4"
};

static const char * const spi7_parents[] = {
	"clk26m",
	"univpll_d6_d2",
	"univpll_192m",
	"mainpll_d6_d2",
	"univpll_d4_d4",
	"mainpll_d4_d4",
	"univpll_d5_d4",
	"univpll_d6_d4"
};

static const char * const msdc_macro_1p_parents[] = {
	"clk26m",
	"msdcpll",
	"mainpll_d6_d2",
	"mainpll_d7_d2"
};

static const char * const msdc30_1_parents[] = {
	"clk26m",
	"mainpll_d6_d2",
	"msdcpll_d2"
};

static const char * const msdc30_1_h_parents[] = {
	"clk26m",
	"mainpll_d6_d2",
	"msdcpll_d2"
};

static const char * const aud_intbus_parents[] = {
	"clk26m",
	"mainpll_d4_d4",
	"mainpll_d7_d4"
};

static const char * const atb_parents[] = {
	"clk26m",
	"mainpll_d4_d2",
	"mainpll_d5_d2"
};

static const char * const disp_pwm_parents[] = {
	"clk26m",
	"univpll_d6_d4",
	"osc_d2",
	"osc_d4",
	"osc_d16",
	"univpll_d5_d4",
	"mainpll_d4_d4"
};

static const char * const usb_parents[] = {
	"clk26m",
	"univpll_d5_d4",
	"univpll_d6_d4"
};

static const char * const usb_xhci_parents[] = {
	"clk26m",
	"univpll_d5_d4",
	"univpll_d6_d4"
};

static const char * const i2c_parents[] = {
	"clk26m",
	"mainpll_d4_d8",
	"univpll_d5_d4",
	"mainpll_d4_d4"
};

static const char * const seninf_parents[] = {
	"clk26m",
	"osc_d2",
	"osc_ck",
	"mmpll_d5_d2",
	"univpll_d4_d2",
	"mmpll_d4_d2",
	"mmpll_d7",
	"univpll_d6",
	"univpll_d5"
};

static const char * const seninf1_parents[] = {
	"clk26m",
	"osc_d2",
	"osc_ck",
	"mmpll_d5_d2",
	"univpll_d4_d2",
	"mmpll_d4_d2",
	"mmpll_d7",
	"univpll_d6",
	"univpll_d5"
};

static const char * const seninf2_parents[] = {
	"clk26m",
	"osc_d2",
	"osc_ck",
	"mmpll_d5_d2",
	"univpll_d4_d2",
	"mmpll_d4_d2",
	"mmpll_d7",
	"univpll_d6",
	"univpll_d5"
};

static const char * const aud_engen1_parents[] = {
	"clk26m",
	"apll1_d2",
	"apll1_d4",
	"apll1_d8"
};

static const char * const aud_engen2_parents[] = {
	"clk26m",
	"apll2_d2",
	"apll2_d4",
	"apll2_d8"
};

static const char * const aes_ufsfde_parents[] = {
	"clk26m",
	"mainpll_d4",
	"mainpll_d4_d2",
	"mainpll_d6",
	"mainpll_d4_d4",
	"univpll_d4_d2",
	"univpll_d6"
};

static const char * const ufs_parents[] = {
	"clk26m",
	"mainpll_d4_d8",
	"mainpll_d4_d4",
	"mainpll_d5_d2",
	"mainpll_d6_d2",
	"univpll_d6_d2",
	"msdcpll_d2"
};

static const char * const aud_1_parents[] = {
	"clk26m",
	"apll1"
};

static const char * const aud_2_parents[] = {
	"clk26m",
	"apll2"
};

static const char * const dpmaif_main_parents[] = {
	"clk26m",
	"univpll_d6",
	"mainpll_d5",
	"mainpll_d6",
	"mainpll_d4_d2",
	"univpll_d4_d2"
};

static const char * const venc_parents[] = {
	"clk26m",
	"mmpll_d4_d2",
	"mainpll_d6",
	"univpll_d4_d2",
	"mainpll_d4_d2",
	"univpll_d6",
	"mmpll_d6",
	"mainpll_d5_d2",
	"mainpll_d6_d2",
	"mmpll_d9",
	"mmpll_d4",
	"mainpll_d4",
	"univpll_d4",
	"univpll_d5",
	"univpll_d5_d2",
	"mainpll_d5"
};

static const char * const vdec_parents[] = {
	"clk26m",
	"univpll_192m_d2",
	"univpll_d5_d4",
	"mainpll_d5",
	"mainpll_d5_d2",
	"mmpll_d6_d2",
	"univpll_d5_d2",
	"mainpll_d4_d2",
	"univpll_d4_d2",
	"univpll_d7",
	"mmpll_d7",
	"mmpll_d6",
	"univpll_d6",
	"mainpll_d4",
	"mmpll_d4_d2",
	"mmpll_d5_d2"
};

static const char * const pwm_parents[] = {
	"clk26m",
	"univpll_d4_d8"
};

static const char * const audio_h_parents[] = {
	"clk26m",
	"univpll_d7_d2",
	"apll1",
	"apll2"
};

static const char * const mcupm_parents[] = {
	"clk26m",
	"univpll_d6_d2",
	"mainpll_d5_d2",
	"mainpll_d6_d2"
};

static const char * const mem_sub_parents[] = {
	"clk26m",
	"univpll_d4_d4",
	"mainpll_d6_d2",
	"mainpll_d5_d2",
	"mainpll_d4_d2",
	"mainpll_d6",
	"mmpll_d7",
	"mainpll_d5",
	"univpll_d5",
	"mainpll_d4",
	"univpll_d4"
};

static const char * const mem_sub_p_parents[] = {
	"clk26m",
	"univpll_d4_d4",
	"mainpll_d5_d2",
	"mainpll_d4_d2",
	"mainpll_d6",
	"mainpll_d5",
	"univpll_d5",
	"mainpll_d4"
};

static const char * const mem_sub_ufs_parents[] = {
	"clk26m",
	"univpll_d4_d4",
	"mainpll_d5_d2",
	"mainpll_d4_d2",
	"mainpll_d6",
	"mainpll_d5",
	"univpll_d5",
	"mainpll_d4"
};

static const char * const emi_n_parents[] = {
	"clk26m",
	"mainpll_d6_d2",
	"osc_d2",
	"mainpll_d6",
	"mainpll_d4",
	"emipll"
};

static const char * const dsi_occ_parents[] = {
	"clk26m",
	"univpll_d6_d2",
	"univpll_d5_d2",
	"univpll_d4_d2"
};

static const char * const ap2conn_host_parents[] = {
	"clk26m",
	"mainpll_d7_d4"
};

static const char * const img1_parents[] = {
	"clk26m",
	"univpll_d4",
	"mmpll_d5",
	"mmpll_d6",
	"univpll_d6",
	"mmpll_d7",
	"mmpll_d4_d2",
	"univpll_d4_d2",
	"mainpll_d4_d2",
	"mmpll_d6_d2",
	"mmpll_d5_d2"
};

static const char * const ipe_parents[] = {
	"clk26m",
	"univpll_d4",
	"mainpll_d4",
	"mmpll_d6",
	"univpll_d6",
	"mainpll_d6",
	"mmpll_d4_d2",
	"univpll_d4_d2",
	"mainpll_d4_d2",
	"mmpll_d6_d2",
	"mmpll_d5_d2"
};

static const char * const cam_parents[] = {
	"clk26m",
	"mainpll_d4",
	"univpll_d4",
	"univpll_d5",
	"mmpll_d7",
	"mmpll_d6",
	"univpll_d6",
	"univpll_d4_d2",
	"mmpll_d9",
	"mmpll_d5_d2",
	"osc_d2"
};

static const char * const camtm_parents[] = {
	"clk26m",
	"osc_d2",
	"univpll_d6_d2",
	"univpll_d6_d4"
};

static const char * const md_emi_parents[] = {
	"clk26m",
	"mainpll_d4"
};

static const char * const mfg_ref_parents[] = {
	"clk26m",
	"univpll_d6",
	"mainpll_d5_d2"
};

static const char * const mfgsc_ref_parents[] = {
	"clk26m",
	"univpll_d6",
	"mainpll_d5_d2"
};

static const char * const efuse_parents[] = {
	"clk26m",
	"osc_d10"
};

static const char * const unipll_ses_parents[] = {
	"clk26m",
	"univpll_d2"
};

static const char * const dramulp_parents[] = {
	"clk26m",
	"mainpll_d6_d2"
};

static const char * const usb_frmcnt_parents[] = {
	"clk26m",
	"univpll_192m_d4"
};

static const char * const apll_m_parents[] = {
	"aud_1",
	"aud_2"
};

static const struct mtk_mux top_muxes[] = {
	/* CLK_CFG_0 */
	MUX_CLR_SET_UPD(CLK_TOP_AXI, "axi",
		axi_parents, CLK_CFG_0, CLK_CFG_0_SET, CLK_CFG_0_CLR,
		0, 3, CLK_CFG_UPDATE, TOP_MUX_AXI_SHIFT),
	MUX_CLR_SET_UPD(CLK_TOP_AXI_P, "axi_p",
		axi_p_parents, CLK_CFG_0, CLK_CFG_0_SET, CLK_CFG_0_CLR,
		8, 2, CLK_CFG_UPDATE, TOP_MUX_AXI_PERI_SHIFT),
	MUX_CLR_SET_UPD(CLK_TOP_AXI_UFS, "axi_ufs",
		axi_p_parents, CLK_CFG_0, CLK_CFG_0_SET, CLK_CFG_0_CLR,
		16, 2, CLK_CFG_UPDATE, TOP_MUX_AXI_UFS_SHIFT),
	MUX_CLR_SET_UPD(CLK_TOP_BUS_AXIMEM, "bus_aximem",
		bus_aximem_parents, CLK_CFG_0, CLK_CFG_0_SET, CLK_CFG_0_CLR,
		24, 3, CLK_CFG_UPDATE, TOP_MUX_BUS_AXIMEM_SHIFT),
	/* CLK_CFG_1 */
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_DISP0, "disp0",
		disp0_parents, CLK_CFG_1, CLK_CFG_1_SET, CLK_CFG_1_CLR,
		0, 4, 7, CLK_CFG_UPDATE, TOP_MUX_DISP0_SHIFT,
		CLK_FENC_STATUS_MON_0, 27),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_MMINFRA, "mminfra",
		mminfra_parents, CLK_CFG_1, CLK_CFG_1_SET, CLK_CFG_1_CLR,
		8, 4, 15, CLK_CFG_UPDATE, TOP_MUX_MMINFRA_SHIFT,
		CLK_FENC_STATUS_MON_0, 26),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_MMUP, "mmup",
		mmup_parents, CLK_CFG_1, CLK_CFG_1_SET, CLK_CFG_1_CLR,
		16, 3, 23, CLK_CFG_UPDATE, TOP_MUX_MMUP_SHIFT,
		CLK_FENC_STATUS_MON_0, 25),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_UART, "uart",
		uart_parents, CLK_CFG_1, CLK_CFG_1_SET, CLK_CFG_1_CLR,
		24, 1, 31, CLK_CFG_UPDATE, TOP_MUX_UART_SHIFT,
		CLK_FENC_STATUS_MON_0, 24),
	/* CLK_CFG_2 */
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_SPI0, "spi0",
		spi0_parents, CLK_CFG_2, CLK_CFG_2_SET, CLK_CFG_2_CLR,
		0, 3, 7, CLK_CFG_UPDATE, TOP_MUX_SPI0_SHIFT,
		CLK_FENC_STATUS_MON_0, 23),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_SPI1, "spi1",
		spi1_parents, CLK_CFG_2, CLK_CFG_2_SET, CLK_CFG_2_CLR,
		8, 3, 15, CLK_CFG_UPDATE, TOP_MUX_SPI1_SHIFT,
		CLK_FENC_STATUS_MON_0, 22),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_SPI2, "spi2",
		spi2_parents, CLK_CFG_2, CLK_CFG_2_SET, CLK_CFG_2_CLR,
		16, 3, 23, CLK_CFG_UPDATE, TOP_MUX_SPI2_SHIFT,
		CLK_FENC_STATUS_MON_0, 21),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_SPI3, "spi3",
		spi3_parents, CLK_CFG_2, CLK_CFG_2_SET, CLK_CFG_2_CLR,
		24, 3, 31, CLK_CFG_UPDATE, TOP_MUX_SPI3_SHIFT,
		CLK_FENC_STATUS_MON_0, 20),
	/* CLK_CFG_3 */
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_SPI4, "spi4",
		spi4_parents, CLK_CFG_3, CLK_CFG_3_SET, CLK_CFG_3_CLR,
		0, 3, 7, CLK_CFG_UPDATE, TOP_MUX_SPI4_SHIFT,
		CLK_FENC_STATUS_MON_0, 19),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_SPI5, "spi5",
		spi5_parents, CLK_CFG_3, CLK_CFG_3_SET, CLK_CFG_3_CLR,
		8, 3, 15, CLK_CFG_UPDATE, TOP_MUX_SPI5_SHIFT,
		CLK_FENC_STATUS_MON_0, 18),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_SPI6, "spi6",
		spi6_parents, CLK_CFG_3, CLK_CFG_3_SET, CLK_CFG_3_CLR,
		16, 3, 23, CLK_CFG_UPDATE, TOP_MUX_SPI6_SHIFT,
		CLK_FENC_STATUS_MON_0, 17),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_SPI7, "spi7",
		spi7_parents, CLK_CFG_3, CLK_CFG_3_SET, CLK_CFG_3_CLR,
		24, 3, 31, CLK_CFG_UPDATE, TOP_MUX_SPI7_SHIFT,
		CLK_FENC_STATUS_MON_0, 16),
	/* CLK_CFG_4 */
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_MSDC_MACRO_1P, "msdc_macro_1p",
		msdc_macro_1p_parents, CLK_CFG_4, CLK_CFG_4_SET, CLK_CFG_4_CLR,
		0, 2, 7, CLK_CFG_UPDATE, TOP_MUX_MSDC_MACRO_1P_SHIFT,
		CLK_FENC_STATUS_MON_0, 15),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_MSDC30_1, "msdc30_1",
		msdc30_1_parents, CLK_CFG_4, CLK_CFG_4_SET, CLK_CFG_4_CLR,
		8, 2, 15, CLK_CFG_UPDATE, TOP_MUX_MSDC30_1_SHIFT,
		CLK_FENC_STATUS_MON_0, 14),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_MSDC30_1_HCLK, "msdc30_1_h",
		msdc30_1_h_parents, CLK_CFG_4, CLK_CFG_4_SET, CLK_CFG_4_CLR,
		16, 2, 23, CLK_CFG_UPDATE, TOP_MUX_MSDC30_1_HCLK_SHIFT,
		CLK_FENC_STATUS_MON_0, 13),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_AUD_INTBUS, "aud_intbus",
		aud_intbus_parents, CLK_CFG_4, CLK_CFG_4_SET, CLK_CFG_4_CLR,
		24, 2, 31, CLK_CFG_UPDATE, TOP_MUX_AUD_INTBUS_SHIFT,
		CLK_FENC_STATUS_MON_0, 12),
	/* CLK_CFG_5 */
	MUX_CLR_SET_UPD(CLK_TOP_ATB, "atb",
		atb_parents, CLK_CFG_5, CLK_CFG_5_SET, CLK_CFG_5_CLR,
		0, 2, CLK_CFG_UPDATE, TOP_MUX_ATB_SHIFT),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_DISP_PWM, "disp_pwm",
		disp_pwm_parents, CLK_CFG_5, CLK_CFG_5_SET, CLK_CFG_5_CLR,
		8, 3, 15, CLK_CFG_UPDATE, TOP_MUX_DISP_PWM_SHIFT,
		CLK_FENC_STATUS_MON_0, 10),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_USB_TOP, "usb",
		usb_parents, CLK_CFG_5, CLK_CFG_5_SET, CLK_CFG_5_CLR,
		16, 2, 23, CLK_CFG_UPDATE, TOP_MUX_USB_TOP_SHIFT,
		CLK_FENC_STATUS_MON_0, 9),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_USB_XHCI, "usb_xhci",
		usb_xhci_parents, CLK_CFG_5, CLK_CFG_5_SET, CLK_CFG_5_CLR,
		24, 2, 31, CLK_CFG_UPDATE, TOP_MUX_SSUSB_XHCI_SHIFT,
		CLK_FENC_STATUS_MON_0, 8),
	/* CLK_CFG_6 */
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_I2C, "i2c",
		i2c_parents, CLK_CFG_6, CLK_CFG_6_SET, CLK_CFG_6_CLR,
		0, 2, 7, CLK_CFG_UPDATE, TOP_MUX_I2C_SHIFT,
		CLK_FENC_STATUS_MON_0, 7),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_SENINF, "seninf",
		seninf_parents, CLK_CFG_6, CLK_CFG_6_SET, CLK_CFG_6_CLR,
		8, 4, 15, CLK_CFG_UPDATE, TOP_MUX_SENINF_SHIFT,
		CLK_FENC_STATUS_MON_0, 6),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_SENINF1, "seninf1",
		seninf1_parents, CLK_CFG_6, CLK_CFG_6_SET, CLK_CFG_6_CLR,
		16, 4, 23, CLK_CFG_UPDATE, TOP_MUX_SENINF1_SHIFT,
		CLK_FENC_STATUS_MON_0, 5),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_SENINF2, "seninf2",
		seninf2_parents, CLK_CFG_6, CLK_CFG_6_SET, CLK_CFG_6_CLR,
		24, 4, 31, CLK_CFG_UPDATE, TOP_MUX_SENINF2_SHIFT,
		CLK_FENC_STATUS_MON_0, 4),
	/* CLK_CFG_7 */
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_AUD_ENGEN1, "aud_engen1",
		aud_engen1_parents, CLK_CFG_7, CLK_CFG_7_SET, CLK_CFG_7_CLR,
		0, 2, 7, CLK_CFG_UPDATE, TOP_MUX_AUD_ENGEN1_SHIFT,
		CLK_FENC_STATUS_MON_0, 3),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_AUD_ENGEN2, "aud_engen2",
		aud_engen2_parents, CLK_CFG_7, CLK_CFG_7_SET, CLK_CFG_7_CLR,
		8, 2, 15, CLK_CFG_UPDATE, TOP_MUX_AUD_ENGEN2_SHIFT,
		CLK_FENC_STATUS_MON_0, 2),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_AES_UFSFDE, "aes_ufsfde",
		aes_ufsfde_parents, CLK_CFG_7, CLK_CFG_7_SET, CLK_CFG_7_CLR,
		16, 3, 23, CLK_CFG_UPDATE, TOP_MUX_AES_UFSFDE_SHIFT,
		CLK_FENC_STATUS_MON_0, 1),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_UFS, "ufs",
		ufs_parents, CLK_CFG_7, CLK_CFG_7_SET, CLK_CFG_7_CLR,
		24, 3, 31, CLK_CFG_UPDATE1, TOP_MUX_UFS_SHIFT,
		CLK_FENC_STATUS_MON_0, 0),
	/* CLK_CFG_8 */
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_AUD_1, "aud_1",
		aud_1_parents, CLK_CFG_8, CLK_CFG_8_SET, CLK_CFG_8_CLR,
		8, 1, 15, CLK_CFG_UPDATE1, TOP_MUX_AUD_1_SHIFT,
		CLK_FENC_STATUS_MON_1, 30),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_AUD_2, "aud_2",
		aud_2_parents, CLK_CFG_8, CLK_CFG_8_SET, CLK_CFG_8_CLR,
		16, 1, 23, CLK_CFG_UPDATE1, TOP_MUX_AUD_2_SHIFT,
		CLK_FENC_STATUS_MON_1, 29),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_DPMAIF_MAIN, "dpmaif_main",
		dpmaif_main_parents, CLK_CFG_8, CLK_CFG_8_SET, CLK_CFG_8_CLR,
		24, 3, 31, CLK_CFG_UPDATE1, TOP_MUX_DPMAIF_MAIN_SHIFT,
		CLK_FENC_STATUS_MON_1, 28),
	/* CLK_CFG_9 */
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_VENC, "venc",
		venc_parents, CLK_CFG_9, CLK_CFG_9_SET, CLK_CFG_9_CLR,
		0, 4, 7, CLK_CFG_UPDATE1, TOP_MUX_VENC_SHIFT,
		CLK_FENC_STATUS_MON_1, 27),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_VDEC, "vdec",
		vdec_parents, CLK_CFG_9, CLK_CFG_9_SET, CLK_CFG_9_CLR,
		8, 4, 15, CLK_CFG_UPDATE1, TOP_MUX_VDEC_SHIFT,
		CLK_FENC_STATUS_MON_1, 26),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_PWM, "pwm",
		pwm_parents, CLK_CFG_9, CLK_CFG_9_SET, CLK_CFG_9_CLR,
		16, 1, 23, CLK_CFG_UPDATE1, TOP_MUX_PWM_SHIFT,
		CLK_FENC_STATUS_MON_1, 25),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_AUDIO_H, "audio_h",
		audio_h_parents, CLK_CFG_9, CLK_CFG_9_SET, CLK_CFG_9_CLR,
		24, 2, 31, CLK_CFG_UPDATE1, TOP_MUX_AUDIO_H_SHIFT,
		CLK_FENC_STATUS_MON_1, 24),
	/* CLK_CFG_10 */
	MUX_CLR_SET_UPD(CLK_TOP_MCUPM, "mcupm",
		mcupm_parents, CLK_CFG_10, CLK_CFG_10_SET, CLK_CFG_10_CLR,
		0, 2, CLK_CFG_UPDATE1, TOP_MUX_MCUPM_SHIFT),
	MUX_CLR_SET_UPD(CLK_TOP_MEM_SUB, "mem_sub",
		mem_sub_parents, CLK_CFG_10, CLK_CFG_10_SET, CLK_CFG_10_CLR,
		8, 4, CLK_CFG_UPDATE1, TOP_MUX_MEM_SUB_SHIFT),
	MUX_CLR_SET_UPD(CLK_TOP_MEM_SUB_P, "mem_sub_p",
		mem_sub_p_parents, CLK_CFG_10, CLK_CFG_10_SET, CLK_CFG_10_CLR,
		16, 3, CLK_CFG_UPDATE1, TOP_MUX_MEM_SUB_PERI_SHIFT),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_MEM_SUB_UFS, "mem_sub_ufs",
		mem_sub_ufs_parents, CLK_CFG_10, CLK_CFG_10_SET, CLK_CFG_10_CLR,
		24, 3, 31, CLK_CFG_UPDATE1, TOP_MUX_MEM_SUB_UFS_SHIFT,
		CLK_FENC_STATUS_MON_1, 20),
	/* CLK_CFG_11 */
	MUX_CLR_SET_UPD(CLK_TOP_EMI_N, "emi_n",
		emi_n_parents, CLK_CFG_11, CLK_CFG_11_SET, CLK_CFG_11_CLR,
		0, 3, CLK_CFG_UPDATE1, TOP_MUX_EMI_N_SHIFT),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_DSI_OCC, "dsi_occ",
		dsi_occ_parents, CLK_CFG_11, CLK_CFG_11_SET, CLK_CFG_11_CLR,
		8, 2, 15, CLK_CFG_UPDATE1, TOP_MUX_DSI_OCC_SHIFT,
		CLK_FENC_STATUS_MON_1, 18),
	MUX_CLR_SET_UPD(CLK_TOP_AP2CONN_HOST, "ap2conn_host",
		ap2conn_host_parents, CLK_CFG_11, CLK_CFG_11_SET, CLK_CFG_11_CLR,
		16, 1, CLK_CFG_UPDATE1, TOP_MUX_AP2CONN_HOST_SHIFT),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_IMG1, "img1",
		img1_parents, CLK_CFG_11, CLK_CFG_11_SET, CLK_CFG_11_CLR,
		24, 4, 31, CLK_CFG_UPDATE1, TOP_MUX_IMG1_SHIFT,
		CLK_FENC_STATUS_MON_1, 16),
	/* CLK_CFG_12 */
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_IPE, "ipe",
		ipe_parents, CLK_CFG_12, CLK_CFG_12_SET, CLK_CFG_12_CLR,
		0, 4, 7, CLK_CFG_UPDATE1, TOP_MUX_IPE_SHIFT,
		CLK_FENC_STATUS_MON_1, 15),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_CAM, "cam",
		cam_parents, CLK_CFG_12, CLK_CFG_12_SET, CLK_CFG_12_CLR,
		8, 4, 15, CLK_CFG_UPDATE1, TOP_MUX_CAM_SHIFT,
		CLK_FENC_STATUS_MON_1, 14),
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_CAMTM, "camtm",
		camtm_parents, CLK_CFG_12, CLK_CFG_12_SET, CLK_CFG_12_CLR,
		16, 2, 23, CLK_CFG_UPDATE1, TOP_MUX_CAMTM_SHIFT,
		CLK_FENC_STATUS_MON_1, 13),
	MUX_CLR_SET_UPD(CLK_TOP_EMI_INTERFACE_546, "md_emi",
		md_emi_parents, CLK_CFG_12, CLK_CFG_12_SET, CLK_CFG_12_CLR,
		24, 1, CLK_CFG_UPDATE1, TOP_MUX_EMI_INTERFACE_546_SHIFT),
	/* CLK_CFG_13 */
	MUX_CLR_SET_UPD(CLK_TOP_MFG_REF, "mfg_ref",
		mfg_ref_parents, CLK_CFG_13, CLK_CFG_13_SET, CLK_CFG_13_CLR,
		8, 2, CLK_CFG_UPDATE1, TOP_MUX_MFG_REF_SHIFT),
	MUX_CLR_SET_UPD(CLK_TOP_MFGSC_REF, "mfgsc_ref",
		mfgsc_ref_parents, CLK_CFG_13, CLK_CFG_13_SET, CLK_CFG_13_CLR,
		16, 2, CLK_CFG_UPDATE1, TOP_MUX_MFGSC_REF_SHIFT),
	MUX_CLR_SET_UPD(CLK_TOP_EFUSE, "efuse",
		efuse_parents, CLK_CFG_13, CLK_CFG_13_SET, CLK_CFG_13_CLR,
		24, 1, CLK_CFG_UPDATE1, TOP_MUX_EFUSE_SHIFT),
	/* CLK_CFG_14 */
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_UNIPLL_SES, "unipll_ses",
		unipll_ses_parents, CLK_CFG_14, CLK_CFG_14_SET, CLK_CFG_14_CLR,
		8, 1, 15, CLK_CFG_UPDATE1, TOP_MUX_UNIPLL_SES_SHIFT,
		CLK_FENC_STATUS_MON_1, 6),
	MUX_CLR_SET_UPD(CLK_TOP_DRAMULP, "dramulp",
		dramulp_parents, CLK_CFG_14, CLK_CFG_14_SET, CLK_CFG_14_CLR,
		24, 1, CLK_CFG_UPDATE1, TOP_MUX_DRAMULP_SHIFT),
	/* CLK_CFG_15 */
	MUX_GATE_FENC_CLR_SET_UPD(CLK_TOP_USB_FRMCNT, "usb_frmcnt",
		usb_frmcnt_parents, CLK_CFG_15, CLK_CFG_15_SET, CLK_CFG_15_CLR,
		0, 1, 7, CLK_CFG_UPDATE1, TOP_MUX_SSUSB_FRMCNT_SHIFT,
		CLK_FENC_STATUS_MON_1, 3),
};

static const struct mtk_composite top_aud_divs[] = {
	MUX_DIV_GATE(CLK_TOP_APLL_I2SIN1_MCK, "apll_i2sin1_m", apll_m_parents,
		     CLK_AUDDIV_0, 17, 1, CLK_AUDDIV_2, 8, 8, CLK_AUDDIV_0, 1),
	MUX_DIV_GATE(CLK_TOP_APLL_I2SIN2_MCK, "apll_i2sin2_m", apll_m_parents,
		     CLK_AUDDIV_0, 18, 1, CLK_AUDDIV_2, 16, 8, CLK_AUDDIV_0, 2),
};

static const struct mtk_clk_desc topck_desc = {
	.factor_clks = top_divs,
	.num_factor_clks = ARRAY_SIZE(top_divs),
	.mux_clks = top_muxes,
	.num_mux_clks = ARRAY_SIZE(top_muxes),
	.composite_clks = top_aud_divs,
	.num_composite_clks = ARRAY_SIZE(top_aud_divs)
};

static const struct of_device_id of_match_clk_mt6858_ck[] = {
	{ .compatible = "mediatek,mt6858-cksys", .data = &topck_desc },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, of_match_clk_mt6858_ck);

static struct platform_driver clk_mt6858_topck_drv = {
	.probe = mtk_clk_simple_probe,
	.remove = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6858-topck",
		.of_match_table = of_match_clk_mt6858_ck,
	},
};

MODULE_DESCRIPTION("MediaTek MT6858 top clock generators driver");
module_platform_driver(clk_mt6858_topck_drv);
MODULE_LICENSE("GPL");
