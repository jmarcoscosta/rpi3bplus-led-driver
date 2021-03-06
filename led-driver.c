// SPDX-License-Identifier: GPL-2.0

#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define GPIO_PIN_LED 21

struct led_driver_data {
	struct gpio_desc *desc;
	struct led_classdev led_cdev;
};

static struct led_driver_data *driver_data;

static void set_gpio_on(void)
{
	gpiod_set_value(driver_data->desc, 1);
}

static void set_gpio_off(void)
{
	gpiod_set_value(driver_data->desc, 0);
}

static void brightness_set_callback(struct led_classdev *led_cdev,
				    enum led_brightness brightness)
{
	if (brightness)
		set_gpio_on();
	else
		set_gpio_off();
}

static int __init led_driver_init(void)
{
	pr_info("LED driver: init\n");

	driver_data = kzalloc(sizeof(*driver_data), GFP_KERNEL);
	if (!driver_data) {
		pr_err("Error when allocating memory for led_driver_data.\n");
		return -ENOMEM;
	}

	if (gpio_request(GPIO_PIN_LED, "led-driver")) {
		pr_err("Error requesting GPIO for led-driver.\n");
		kfree(driver_data);
		return -1;
	}

	driver_data->desc = gpio_to_desc(GPIO_PIN_LED);
	gpiod_direction_output(driver_data->desc, 0);

	driver_data->led_cdev.name = "ipe:green:user";
	driver_data->led_cdev.brightness_set = brightness_set_callback;

	if (led_classdev_register(NULL, &driver_data->led_cdev)) {
		pr_err("led-driver: Error registering led.\n");
		gpio_free(GPIO_PIN_LED);
		kfree(driver_data);
		return -1;
	}

	return 0;
}

static void __exit led_driver_exit(void)
{
	pr_info("LED driver: exit\n");
	gpio_free(GPIO_PIN_LED);
	kfree(driver_data);
}

module_init(led_driver_init);
module_exit(led_driver_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Joao Marcos Costa");
MODULE_DESCRIPTION("LED driver for Raspberry Pi 3 B+");
MODULE_VERSION("1.0");
