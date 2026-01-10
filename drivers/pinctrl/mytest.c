#include "linux/pinctrl/pinmux.h"
#include <linux/bitops.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinconf-generic.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/platform_device.h>
#include "core.h"
#include "pinmux.h"

const struct pinctrl_pin_desc my_pins[] = {
    PINCTRL_PIN(0, "A1"),
    PINCTRL_PIN(1, "A2"),
    PINCTRL_PIN(2, "A3"),
    PINCTRL_PIN(3, "A4"),
    PINCTRL_PIN(4, "A5"),
};

// Two way to setup hdmi0
static const u32 hdmi0_0_pins[] = {0, 2};
static const u32 hdmi0_1_pins[] = {1 ,2};

/**
pinctrl_uart0: uart0grp {  
    pins = "PH0", "PH1";     // Group: which pins  
    function = "uart0";      // Function: what they do  
};
 */
static const struct pingroup my_group[] = {
    PINCTRL_PINGROUP("hdmi0_0_grp", hdmi0_0_pins, ARRAY_SIZE(hdmi0_0_pins)),
    PINCTRL_PINGROUP("hdmi0_1_grp", hdmi0_1_pins, ARRAY_SIZE(hdmi0_1_pins)),
};

static const char * const hdmi0_groups[] = { "hdmi0_0_grp", "hdmi0_1_grp" };
static const struct pinfunction my_functions[] = {  
    PINCTRL_PINFUNCTION("hdmi0", hdmi0_groups, ARRAY_SIZE(hdmi0_groups)),  
};

static int my_set_mux (struct pinctrl_dev *pctldev, unsigned int func_selector,
			unsigned int group_selector)
{
    return 0;
}

static const struct pinctrl_ops my_pctlops = {
    .get_group_name = pinctrl_generic_get_group_name, //must
    .get_groups_count = pinctrl_generic_get_group_count, //must
    .dt_node_to_map = pinconf_generic_dt_node_to_map_all, //must
    .get_group_pins = pinctrl_generic_get_group_pins, // must
};

/** 
    mypio {
		phandle = <0x9527>;
		compatible = "my,pinctrled";

		hdmi_pins: hdmi_pins {
			groups = "hdmi0_0_grp";
			function = "hdmi0";
		};
	};
 * If 'function' attribute is existing, pinmux_ops is required. Array 'my_functions' and 'my_groups' is required to add.
 */
static const struct pinmux_ops my_pmuxops = {
	.get_functions_count = pinmux_generic_get_function_count,
	.get_function_name = pinmux_generic_get_function_name,
	.get_function_groups = pinmux_generic_get_function_groups,
	.set_mux = my_set_mux,
};


static int my_pin_config_set (struct pinctrl_dev *pctldev,
			       unsigned int pin,
			       unsigned long *configs,
			       unsigned int num_configs)
{
    pr_info("pin_config_get");
    return 0;
}

static int my_pin_config_group_set (struct pinctrl_dev *pctldev,
				     unsigned int selector,
				     unsigned long *configs,
				     unsigned int num_configs)
{
    pr_info("pin_config_group_set");
    return 0;
}

/**
    mypio {
		phandle = <0x9527>;
		compatible = "my,pinctrled";

		hdmi_pins: hdmi_pins {
			pins = "A1", "A3"; // It override 'groups' attributes.
			output-enable; // must be dt_params. MUST Not exist 'function'.
		};
	};
 */
static const struct pinconf_ops my_pconfops = {
    .pin_config_set = my_pin_config_set,
    .pin_config_group_set = my_pin_config_group_set,
};

static struct pinctrl_desc my_desc = {
    .name = "mydesc",
    .pins = my_pins,
    .pctlops = &my_pctlops,
    .pmxops = &my_pmuxops,
    .confops = &my_pconfops,
    .npins = ARRAY_SIZE(my_pins),
    .owner = THIS_MODULE
};

static struct pinctrl_dev *mypinctrl;
static int my_probe(struct platform_device *pdev)
{
    int i;

    mypinctrl = devm_pinctrl_register(&pdev->dev, &my_desc, NULL);
    if (IS_ERR(mypinctrl)) {
		dev_err(&pdev->dev, "Failed to register pinctrl\n");
		return PTR_ERR(mypinctrl);
	}

    for(i=0; i<ARRAY_SIZE(my_group); i++) {
        pinctrl_generic_add_group(mypinctrl, my_group[i].name, my_group[i].pins, my_group[i].npins, NULL);
    }

    for(i=0; i<ARRAY_SIZE(my_functions); i++) {
        pinmux_generic_add_function(mypinctrl, my_functions[i].name, my_functions[i].groups, my_functions[i].ngroups, NULL);
    }

    return 0;
}

static const struct of_device_id my_of_match[] = {
    {.compatible = "my,pinctrled"},
    {}
};
MODULE_DEVICE_TABLE(of, my_of_match);



static struct platform_driver my_driver = {
    .driver = {
        .name = "mypin_driver",
        .of_match_table = my_of_match,
    },
    .probe = my_probe,
};
module_platform_driver(my_driver);
