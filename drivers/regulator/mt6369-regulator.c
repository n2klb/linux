// SPDX-License-Identifier: GPL-2.0
//
// Copyright (c) 2024 MediaTek Inc.
// Copyright (c) 2025 Collabora Ltd
//                    AngeloGioacchino Del Regno <angelogioacchino.delregno@collabora.com>
// Copyright (c) 2026 Jolla Mobile Ltd

#include <linux/bitfield.h>
#include <linux/delay.h>
#include <linux/devm-helpers.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/spmi.h>

#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/mt6369-regulator.h>
#include <linux/regulator/of_regulator.h>

#define MT6369_REGULATOR_MODE_NORMAL	0
#define MT6369_REGULATOR_MODE_FCCM	1
#define MT6369_REGULATOR_MODE_LP	2
#define MT6369_REGULATOR_MODE_ULP	3

#define EN_SET_OFFSET			0x1
#define EN_CLR_OFFSET			0x2

#define OC_IRQ_ENABLE_DELAY_MS		10

#define MT6369_BUCK_TOP_UNLOCK_VALUE	0x5543

enum {
	MT6369_ID_VBUCK1,
	MT6369_ID_VPA,
	MT6369_ID_VSRAM_CORE,
	MT6369_ID_VDIGRF,
	MT6369_ID_VAUX18,
	MT6369_ID_VUSB,
	MT6369_ID_VIBR,
	MT6369_ID_VIO28,
	MT6369_ID_VFP,
	MT6369_ID_VTP,
	MT6369_ID_VMCH,
	MT6369_ID_VMC,
	MT6369_ID_VCN33_1,
	MT6369_ID_VCN33_2,
	MT6369_ID_VAUD28,
	MT6369_ID_VANT18,
	MT6369_ID_VEFUSE,
};

/**
 * struct mt6369_regulator_info - MT6369 regulators information
 * @desc: Regulator description structure
 * @lp_mode_reg: Low Power mode register (normal/idle)
 * @lp_mode_mask: Low Power mode regulator mask
 * @modeset_reg: AUTO/PWM mode register
 * @modeset_mask: AUTO/PWM regulator mask
 * @oc_work: Delayed work for enabling overcurrent IRQ
 * @hwirq: PMIC-Internal HW Interrupt for overcurrent event
 * @virq: Mapped Interrupt for overcurrent event
 */
struct mt6369_regulator_info {
	struct regulator_desc desc;
	u16 lp_mode_reg;
	u16 lp_mode_mask;
	u16 modeset_reg;
	u16 modeset_mask;
	struct delayed_work oc_work;
	u8 hwirq;
	int virq;
};

#define MT6369_BUCK(match, vreg, min, max, step, ocp_intn)		\
[MT6369_ID_##vreg] = {							\
	.desc = {							\
		.name = match,						\
		.supply_name = "vsys-"match,				\
		.of_match = of_match_ptr(match),			\
		.ops = &mt6369_vreg_setclr_ops,				\
		.type = REGULATOR_VOLTAGE,				\
		.id = MT6369_ID_##vreg,					\
		.owner = THIS_MODULE,					\
		.n_voltages = (max - min) / step + 1,			\
		.min_uV = min,						\
		.uV_step = step,					\
		.enable_reg = MT6369_RG_BUCK_##vreg##_EN_ADDR,		\
		.enable_mask = BIT(MT6369_RG_BUCK_##vreg##_EN_BIT),	\
		.vsel_reg = MT6369_RG_BUCK_##vreg##_VOSEL_ADDR,		\
		.vsel_mask = MT6369_RG_BUCK_##vreg##_VOSEL_MASK,	\
		.of_map_mode = mt6369_map_mode,				\
	},								\
	.lp_mode_reg = MT6369_RG_BUCK_##vreg##_LP_ADDR,			\
	.lp_mode_mask = BIT(MT6369_RG_BUCK_##vreg##_LP_BIT),		\
	.modeset_reg = MT6369_RG_##vreg##_MODESET_ADDR,			\
	.modeset_mask = BIT(MT6369_RG_##vreg##_MODESET_BIT),		\
	.hwirq = ocp_intn,						\
}

#define MT6369_LDO_L(match, vreg, in_sup, min, max, step, ocp_intn)	\
[MT6369_ID_##vreg] = {							\
	.desc = {							\
		.name = match,						\
		.supply_name = in_sup,					\
		.of_match = of_match_ptr(match),			\
		.ops = &mt6369_ldo_linear_ops,				\
		.type = REGULATOR_VOLTAGE,				\
		.id = MT6369_ID_##vreg,					\
		.owner = THIS_MODULE,					\
		.n_voltages = (max - min) / step + 1,			\
		.min_uV = min,						\
		.uV_step = step,					\
		.enable_reg = MT6369_RG_LDO_##vreg##_ADDR,		\
		.enable_mask = BIT(MT6369_RG_LDO_##vreg##_EN_BIT),	\
		.vsel_reg = MT6369_RG_LDO_##vreg##_VOSEL_ADDR,		\
		.vsel_mask = MT6369_RG_LDO_##vreg##_VOSEL_MASK,		\
		.of_map_mode = mt6369_map_mode,				\
	},								\
	.lp_mode_reg = MT6369_RG_LDO_##vreg##_ADDR,			\
	.lp_mode_mask = BIT(MT6369_RG_LDO_##vreg##_LP_BIT),		\
	.hwirq = ocp_intn,						\
}

#define MT6369_LDO_VT(match, vreg, in_sup, vrnum, ocp_intn)		\
[MT6369_ID_##vreg] = {							\
	.desc = {							\
		.name = match,						\
		.supply_name = in_sup,					\
		.of_match = of_match_ptr(match),			\
		.ops = &mt6369_ldo_vtable_ops,				\
		.type = REGULATOR_VOLTAGE,				\
		.id = MT6369_ID_##vreg,					\
		.owner = THIS_MODULE,					\
		.n_voltages = ARRAY_SIZE(ldo_volt_ranges##vrnum) * 11,	\
		.linear_ranges = ldo_volt_ranges##vrnum,		\
		.n_linear_ranges = ARRAY_SIZE(ldo_volt_ranges##vrnum),	\
		.linear_range_selectors_bitfield = ldos_cal_selectors,	\
		.enable_reg = MT6369_RG_LDO_##vreg##_ADDR,		\
		.enable_mask = BIT(MT6369_RG_LDO_##vreg##_EN_BIT),	\
		.vsel_reg = MT6369_RG_##vreg##_VOCAL_ADDR,		\
		.vsel_mask = MT6369_RG_##vreg##_VOCAL_MASK,		\
		.vsel_range_reg = MT6369_RG_##vreg##_VOSEL_ADDR,	\
		.vsel_range_mask = MT6369_RG_##vreg##_VOSEL_MASK,	\
		.of_map_mode = mt6369_map_mode,				\
	},								\
	.lp_mode_reg = MT6369_RG_LDO_##vreg##_ADDR,			\
	.lp_mode_mask = BIT(MT6369_RG_LDO_##vreg##_LP_BIT),		\
	.hwirq = ocp_intn,						\
}

static const unsigned int ldos_cal_selectors[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};

static const struct linear_range ldo_volt_ranges1[] = {
	REGULATOR_LINEAR_RANGE(1800000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(1900000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(2000000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(2100000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(2200000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(2300000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(2400000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(2500000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(2600000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(2700000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(2800000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(2900000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(3000000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(3100000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(3200000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(3300000, 0, 10, 10000)
};

static const struct linear_range ldo_volt_ranges2[] = {
	REGULATOR_LINEAR_RANGE(1200000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(1300000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(1500000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(1700000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(1800000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(2000000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(2500000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(2600000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(2700000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(2800000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(2900000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(3000000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(3100000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(3300000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(3400000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(3500000, 0, 10, 10000)
};

static const struct linear_range ldo_volt_ranges3[] = {
	REGULATOR_LINEAR_RANGE(600000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(700000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(800000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(900000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(1000000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(1100000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(1200000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(1300000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(1400000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(1500000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(1600000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(1700000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(1800000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(1900000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(2000000, 0, 10, 10000),
	REGULATOR_LINEAR_RANGE(2100000, 0, 10, 10000)
};

static int mt6369_vreg_enable_setclr(struct regulator_dev *rdev)
{
	return regmap_write(rdev->regmap, rdev->desc->enable_reg + EN_SET_OFFSET,
			    rdev->desc->enable_mask);
}

static int mt6369_vreg_disable_setclr(struct regulator_dev *rdev)
{
	return regmap_write(rdev->regmap, rdev->desc->enable_reg + EN_CLR_OFFSET,
			    rdev->desc->enable_mask);
}

static inline unsigned int mt6369_map_mode(unsigned int mode)
{
	switch (mode) {
	case MT6369_REGULATOR_MODE_NORMAL:
		return REGULATOR_MODE_NORMAL;
	case MT6369_REGULATOR_MODE_FCCM:
		return REGULATOR_MODE_FAST;
	case MT6369_REGULATOR_MODE_LP:
		return REGULATOR_MODE_IDLE;
	case MT6369_REGULATOR_MODE_ULP:
		return REGULATOR_MODE_STANDBY;
	default:
		return REGULATOR_MODE_INVALID;
	}
}

static unsigned int mt6369_regulator_get_mode(struct regulator_dev *rdev)
{
	struct mt6369_regulator_info *info = rdev_get_drvdata(rdev);
	unsigned int val;
	int ret;

	if (info->modeset_reg) {
		ret = regmap_read(rdev->regmap, info->modeset_reg, &val);
		if (ret) {
			dev_err(&rdev->dev, "Failed to get mt6369 mode: %d\n", ret);
			return ret;
		}

		if (val & info->modeset_mask)
			return REGULATOR_MODE_FAST;
	} else {
		val = 0;
	}

	ret = regmap_read(rdev->regmap, info->lp_mode_reg, &val);
	val &= info->lp_mode_mask;

	if (ret) {
		dev_err(&rdev->dev, "Failed to get lp mode: %d\n", ret);
		return ret;
	}

	if (val)
		return REGULATOR_MODE_IDLE;
	else
		return REGULATOR_MODE_NORMAL;
}

static int mt6369_buck_unlock(struct regmap *map, bool unlock)
{
	u16 buf = unlock ? MT6369_BUCK_TOP_UNLOCK_VALUE : 0;

	return regmap_bulk_write(map, MT6369_BUCK_TOP_KEY_PROT_LO, &buf, sizeof(buf));
}

static int mt6369_regulator_set_mode(struct regulator_dev *rdev,
				     unsigned int mode)
{
	struct mt6369_regulator_info *info = rdev_get_drvdata(rdev);
	struct regmap *regmap = rdev->regmap;
	int cur_mode, ret;

	if (!info->modeset_reg && mode == REGULATOR_MODE_FAST)
		return -EOPNOTSUPP;

	switch (mode) {
	case REGULATOR_MODE_FAST:
		ret = mt6369_buck_unlock(regmap, true);
		if (ret)
			break;

		ret = regmap_set_bits(regmap, info->modeset_reg, info->modeset_mask);

		mt6369_buck_unlock(regmap, false);
		break;
	case REGULATOR_MODE_NORMAL:
		cur_mode = mt6369_regulator_get_mode(rdev);
		if (cur_mode < 0) {
			ret = cur_mode;
			break;
		}

		if (cur_mode == REGULATOR_MODE_FAST) {
			ret = mt6369_buck_unlock(regmap, true);
			if (ret)
				break;

			ret = regmap_clear_bits(regmap, info->modeset_reg, info->modeset_mask);

			mt6369_buck_unlock(regmap, false);
			break;
		} else if (cur_mode == REGULATOR_MODE_IDLE) {
			ret = regmap_clear_bits(regmap, info->lp_mode_reg, info->lp_mode_mask);
			if (ret == 0)
				usleep_range(100, 200);
		} else {
			ret = 0;
		}
		break;
	case REGULATOR_MODE_IDLE:
		ret = regmap_set_bits(regmap, info->lp_mode_reg, info->lp_mode_mask);
		break;
	default:
		ret = -EINVAL;
	}

	if (ret) {
		dev_err(&rdev->dev, "Failed to set mode %u: %d\n", mode, ret);
		return ret;
	}

	return 0;
}

static void mt6369_oc_irq_enable_work(struct work_struct *work)
{
	struct delayed_work *dwork = to_delayed_work(work);
	struct mt6369_regulator_info *info =
		container_of(dwork, struct mt6369_regulator_info, oc_work);

	enable_irq(info->virq);
}

static irqreturn_t mt6369_oc_isr(int irq, void *data)
{
	struct regulator_dev *rdev = (struct regulator_dev *)data;
	struct mt6369_regulator_info *info = rdev_get_drvdata(rdev);

	disable_irq_nosync(info->virq);

	if (regulator_is_enabled_regmap(rdev))
		regulator_notifier_call_chain(rdev, REGULATOR_EVENT_OVER_CURRENT, NULL);

	schedule_delayed_work(&info->oc_work, msecs_to_jiffies(OC_IRQ_ENABLE_DELAY_MS));

	return IRQ_HANDLED;
}

static int mt6369_set_ocp(struct regulator_dev *rdev, int lim, int severity, bool enable)
{
	struct mt6369_regulator_info *info = rdev_get_drvdata(rdev);

	/* MT6369 supports only enabling protection and does not support limits */
	if (lim || severity != REGULATOR_SEVERITY_PROT || !enable)
		return -EOPNOTSUPP;

	/* If there is no OCP interrupt, there's nothing to set */
	if (info->virq <= 0)
		return -EOPNOTSUPP;

	return devm_request_threaded_irq(&rdev->dev, info->virq, NULL,
					 mt6369_oc_isr, IRQF_ONESHOT,
					 info->desc.name, rdev);
}

static const struct regulator_ops mt6369_vreg_setclr_ops = {
	.list_voltage = regulator_list_voltage_linear,
	.map_voltage = regulator_map_voltage_linear,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.set_voltage_time_sel = regulator_set_voltage_time_sel,
	.enable = mt6369_vreg_enable_setclr,
	.disable = mt6369_vreg_disable_setclr,
	.is_enabled = regulator_is_enabled_regmap,
	.set_mode = mt6369_regulator_set_mode,
	.get_mode = mt6369_regulator_get_mode,
	.set_over_current_protection = mt6369_set_ocp,
};

static const struct regulator_ops mt6369_ldo_linear_ops = {
	.list_voltage = regulator_list_voltage_linear,
	.map_voltage = regulator_map_voltage_linear,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.set_voltage_time_sel = regulator_set_voltage_time_sel,
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.is_enabled = regulator_is_enabled_regmap,
	.set_mode = mt6369_regulator_set_mode,
	.get_mode = mt6369_regulator_get_mode,
	.set_over_current_protection = mt6369_set_ocp,
};

static const struct regulator_ops mt6369_ldo_vtable_ops = {
	.list_voltage = regulator_list_voltage_pickable_linear_range,
	.map_voltage = regulator_map_voltage_pickable_linear_range,
	.set_voltage_sel = regulator_set_voltage_sel_pickable_regmap,
	.get_voltage_sel = regulator_get_voltage_sel_pickable_regmap,
	.set_voltage_time_sel = regulator_set_voltage_time_sel,
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.is_enabled = regulator_is_enabled_regmap,
	.set_mode = mt6369_regulator_set_mode,
	.get_mode = mt6369_regulator_get_mode,
	.set_over_current_protection = mt6369_set_ocp,
};

/* The array is indexed by id(MT6369_ID_XXX) */
static struct mt6369_regulator_info mt6369_regulators[] = {
	MT6369_BUCK("vbuck1", VBUCK1, 0, 1193750, 6250, 0),
	MT6369_BUCK("vpa", VPA, 500000, 3600000, 50000, 6),
	MT6369_LDO_L("vsram-core", VSRAM_CORE, "fixme", 400000, 1193750, 6250, 23),
	MT6369_LDO_L("vdigrf", VDIGRF, "fixme", 400000, 1193750, 6250, 24),
	MT6369_LDO_VT("vaux18", VAUX18, "fixme", 1, 26),
	MT6369_LDO_VT("vusb", VUSB, "fixme", 2, 16),
	MT6369_LDO_VT("vibr", VIBR, "fixme", 2, 12),
	MT6369_LDO_VT("vio28", VIO28, "fixme", 2, 13),
	MT6369_LDO_VT("vfp", VFP, "fixme", 2, 14),
	MT6369_LDO_VT("vtp", VTP, "fixme", 2, 15),
	MT6369_LDO_VT("vmch", VMCH, "fixme", 2, 21),
	MT6369_LDO_VT("vmc", VMC, "fixme", 2, 22),
	MT6369_LDO_VT("vcn33-1", VCN33_1, "fixme", 2, 18),
	MT6369_LDO_VT("vcn33-2", VCN33_2, "fixme", 2, 19),
	MT6369_LDO_VT("vaud28", VAUD28, "fixme", 1, 17),
	MT6369_LDO_VT("vant18", VANT18, "fixme", 3, 25),
	MT6369_LDO_VT("vefuse", VEFUSE, "fixme", 2, 20),
};

static void mt6369_irq_remove(void *data)
{
	int *virq = data;

	irq_dispose_mapping(*virq);
}

static void mt6369_spmi_remove(void *data)
{
	struct spmi_device *sdev = data;

	spmi_device_remove(sdev);
};

static struct regmap *mt6369_spmi_register_regmap(struct device *dev)
{
	struct regmap_config mt6369_regmap_config = {
		.reg_bits = 16,
		.val_bits = 16,
		.max_register = 0x1f90,
		.fast_io = true,
	};
	struct spmi_device *sdev, *sparent;
	u32 base;
	int ret;

	if (!dev->parent)
		return ERR_PTR(-ENODEV);

	ret = device_property_read_u32(dev, "reg", &base);
	if (ret)
		return ERR_PTR(ret);

	sparent = to_spmi_device(dev->parent);
	if (!sparent)
		return ERR_PTR(-ENODEV);

	sdev = spmi_device_alloc(sparent->ctrl);
	if (!sdev)
		return ERR_PTR(-ENODEV);

	sdev->usid = sparent->usid;
	dev_set_name(&sdev->dev, "%d-%02x-regulator", sdev->ctrl->nr, sdev->usid);
	ret = device_add(&sdev->dev);
	if (ret) {
		put_device(&sdev->dev);
		return ERR_PTR(ret);
	};

	ret = devm_add_action_or_reset(dev, mt6369_spmi_remove, sdev);
	if (ret)
		return ERR_PTR(ret);

	mt6369_regmap_config.reg_base = base;

	return devm_regmap_init_spmi_ext(sdev, &mt6369_regmap_config);
}

static int mt6369_regulator_probe(struct platform_device *pdev)
{
	struct device_node *interrupt_parent;
	struct regulator_config config = {};
	struct mt6369_regulator_info *info;
	struct device *dev = &pdev->dev;
	struct regulator_dev *rdev;
	struct irq_domain *domain;
	struct irq_fwspec fwspec;
	struct spmi_device *sdev;
	int i, ret;
	dev_info(dev, "%s()", __func__); /* debug */

	config.regmap = mt6369_spmi_register_regmap(dev);
	if (IS_ERR(config.regmap))
		return dev_err_probe(dev, PTR_ERR(config.regmap),
				     "Cannot get regmap\n");
	config.dev = dev;
	sdev = to_spmi_device(dev->parent);

	interrupt_parent = of_irq_find_parent(dev->of_node);
	if (!interrupt_parent)
		return dev_err_probe(dev, -EINVAL, "Cannot find IRQ parent\n");

	domain = irq_find_host(interrupt_parent);
	of_node_put(interrupt_parent);
	fwspec.fwnode = domain->fwnode;

	fwspec.param_count = 3;
	fwspec.param[0] = sdev->usid;
	fwspec.param[2] = IRQ_TYPE_LEVEL_HIGH;

#if 0
	/* TODO: get polarity from DT */
	regmap_clear_bits(config.regmap, MT6369_RG_LDO_VMCH_EINT_ADDR,
			  BIT(MT6369_RG_LDO_VMCH_EINT_POL_BIT));
#endif

	for (i = 0; i < ARRAY_SIZE(mt6369_regulators); i++) {
		info = &mt6369_regulators[i];

		fwspec.param[1] = info->hwirq;
		info->virq = irq_create_fwspec_mapping(&fwspec);
		if (!info->virq)
			return dev_err_probe(dev, -EINVAL,
					     "Failed to map IRQ%d\n", info->hwirq);

		ret = devm_add_action_or_reset(dev, mt6369_irq_remove, &info->virq);
		if (ret)
			return ret;

		config.driver_data = info;
		INIT_DELAYED_WORK(&info->oc_work, mt6369_oc_irq_enable_work);

		rdev = devm_regulator_register(dev, &info->desc, &config);
		if (IS_ERR(rdev))
			return dev_err_probe(dev, PTR_ERR(rdev),
					     "failed to register %s\n", info->desc.name);
	}

	return 0;
}

static const struct of_device_id mt6369_regulator_match[] = {
	{ .compatible = "mediatek,mt6369-regulator" },
	{ /* sentinel */ }
};

static struct platform_driver mt6369_regulator_driver = {
	.driver = {
		.name = "mt6369-regulator",
		.probe_type = PROBE_PREFER_ASYNCHRONOUS,
		.of_match_table = mt6369_regulator_match,
	},
	.probe = mt6369_regulator_probe,
};
module_platform_driver(mt6369_regulator_driver);

MODULE_DESCRIPTION("Regulator Driver for MediaTek MT6369 PMIC");
MODULE_LICENSE("GPL");
