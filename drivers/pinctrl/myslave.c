#include <linux/device.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pinctrl/consumer.h>
#include <linux/mod_devicetable.h>

/** before calling the probe function, 'pinctrl_bind_pins(dev)' will be called. And it should be success.
 */
static int my_probe(struct platform_device *pdev)
{
    struct pinctrl *p = devm_pinctrl_get(&pdev->dev);
    struct pinctrl_state *s = pinctrl_lookup_state(p, "default");
    int ret = pinctrl_select_state(p, s);
    return ret;
}

static const struct of_device_id my_of_match[] = {
    {.compatible = "my,mypioslave"},
    {}
};
MODULE_DEVICE_TABLE(of, my_of_match);

static struct platform_driver my_driver = {
    .driver = {
        .name = "mypin_slave_driver",
        .of_match_table = my_of_match,
    },
    .probe = my_probe,
};
module_platform_driver(my_driver);
