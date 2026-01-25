// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2011 Sascha Hauer, Pengutronix <s.hauer@pengutronix.de>
 */
#include <linux/module.h>
#include <linux/clk-provider.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/of.h>
#include <linux/platform_device.h>

#define FAKE_PLL_MAX_OUTPUTS 2

struct fake_pll_output {
    struct clk_hw hw;
    u32 mult;
    u32 div;
    unsigned long rate;
};

struct fake_pll {
    struct device *dev;
    void __iomem *base;
    struct clk *osc_clk;          // 输入时钟 OSC
    struct clk *factor_clk;       // 另一个输入时钟
    struct clk_onecell_data clk_data;
    struct fake_pll_output outputs[FAKE_PLL_MAX_OUTPUTS];
    u8 num_outputs;
};

static unsigned long clk_factor_recalc_rate(struct clk_hw *hw,
		unsigned long parent_rate)
{
	return parent_rate;
}

static int clk_factor_determine_rate(struct clk_hw *hw,
				     struct clk_rate_request *req)
{
    /** 这里通过硬件设置对应的参数，来设置频率 */
    /** fake pll 直接返回设置成功 */
	return 0;
}

static int clk_factor_set_rate(struct clk_hw *hw, unsigned long rate,
				unsigned long parent_rate)
{
	/*
	 * We must report success but we can do so unconditionally because
	 * clk_factor_determine_rate returns values that ensure this call is a
	 * nop.

@set_rate:	Change the rate of this clock. The requested rate is specified
 *		by the second argument, which should typically be the return
 *		of .round_rate call.  The third argument gives the parent rate
 *		which is likely helpful for most .set_rate implementation.
 *		Returns 0 on success, -EERROR otherwise.
	 */

	return 0;
}

static unsigned long clk_factor_recalc_accuracy(struct clk_hw *hw,
						unsigned long parent_accuracy)
{
	return parent_accuracy;
}

const struct clk_ops clk_fixed_factor_ops = {
	.determine_rate = clk_factor_determine_rate,
	.set_rate = clk_factor_set_rate,
	.recalc_rate = clk_factor_recalc_rate,
	.recalc_accuracy = clk_factor_recalc_accuracy,
};
EXPORT_SYMBOL_GPL(clk_fixed_factor_ops);

#ifdef CONFIG_OF
static struct clk_hw *_of_fake_pll_setup(struct device_node *node)
{
	struct clk_hw *hw;
	const char *clk_name = node->name;
	const struct clk_parent_data pdata = { .index = 0 };
	u32 div, mult;
	int ret;

	if (of_property_read_u32(node, "clock-div", &div)) {
		pr_err("%s Fixed factor clock <%pOFn> must have a clock-div property\n",
			__func__, node);
		return ERR_PTR(-EIO);
	}

	if (of_property_read_u32(node, "clock-mult", &mult)) {
		pr_err("%s Fixed factor clock <%pOFn> must have a clock-mult property\n",
			__func__, node);
		return ERR_PTR(-EIO);
	}

	of_property_read_string(node, "clock-output-names", &clk_name);


	if (IS_ERR(hw)) {
		/*
		 * Clear OF_POPULATED flag so that clock registration can be
		 * attempted again from probe function.
		 */
		of_node_clear_flag(node, OF_POPULATED);
		return ERR_CAST(hw);
	}

	ret = of_clk_add_hw_provider(node, of_clk_hw_simple_get, hw);
	if (ret) {
		clk_hw_unregister_fixed_factor(hw);
		return ERR_PTR(ret);
	}

	return hw;
}

/**
 * of_fake_pll_setup() - Setup function for simple fixed factor clock
 * @node:	device node for the clock
 */
void __init of_fake_pll_setup(struct device_node *node)
{
	_of_fake_pll_setup(node);
}
CLK_OF_DECLARE(fake_pll, "fake-pll",
		of_fake_pll_setup);

static void of_fake_pll_remove(struct platform_device *pdev)
{
	struct clk_hw *clk = platform_get_drvdata(pdev);

	of_clk_del_provider(pdev->dev.of_node);
	clk_hw_unregister_fixed_factor(clk);
}

static int of_fake_pll_probe(struct platform_device *pdev)
{
	struct clk_hw *clk;

	/*
	 * This function is not executed when of_fake_pll_setup
	 * succeeded.
	 */
	clk = _of_fake_pll_setup(pdev->dev.of_node);
	if (IS_ERR(clk))
		return PTR_ERR(clk);

	platform_set_drvdata(pdev, clk);

	return 0;
}

static const struct of_device_id of_fake_pll_ids[] = {
	{ .compatible = "fake-pll" },
	{ }
};
MODULE_DEVICE_TABLE(of, of_fake_pll_ids);

static struct platform_driver of_fake_pll_driver = {
	.driver = {
		.name = "of_fake_pll_clk",
		.of_match_table = of_fake_pll_ids,
	},
	.probe = of_fake_pll_probe,
	.remove = of_fake_pll_remove,
};
builtin_platform_driver(of_fake_pll_driver);
#endif
