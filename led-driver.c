// SPDX-License-Identifier: GPL-2.0

#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define BCM2837_GPIO_BASE_ADDR 0x3F200000
#define MAX_DATA_BUFFER_LENGTH 1024
#define GPIO_OUTPUT_CLEAR_0 0x28
#define GPIO_OUTPUT_SET_0 0x1C
#define IO_MEM_PAGE_SIZE 1024

static char data_buffer[MAX_DATA_BUFFER_LENGTH];

struct led_driver_data {
	void *gpio_registers;
	struct led_classdev led_cdev;
};

static struct led_driver_data *driver_data;

static void set_gpio_on(unsigned int pin)
{
	u32 *gpio_fsel, *gpio_on_register;
	u32 fsel_index, fsel_bitpos;

	fsel_index = pin / 10;
	fsel_bitpos = pin % 10;
	gpio_fsel = (u32 *)driver_data->gpio_registers + fsel_index;
	gpio_on_register = driver_data->gpio_registers + GPIO_OUTPUT_SET_0;

	/* Clear FSEL bits */
	*gpio_fsel &= ~(7 << 3 * fsel_bitpos);

	/* Set FSEL bit to 1 -> set the pin as an output */
	*gpio_fsel |= (1 << 3 * fsel_bitpos);

	/* Set the pin value to 1 */
	*gpio_on_register |= (1 << pin);
}

static void set_gpio_off(unsigned int pin)
{
	u32 *gpio_off_register = driver_data->gpio_registers + GPIO_OUTPUT_CLEAR_0;
	*gpio_off_register |= (1 << pin);
}

static ssize_t led_driver_read(struct file *file, char __user *user, size_t size,
			       loff_t *off)
{
	char *p = data_buffer;
	static size_t len;
	int ret;

	while (*(p++))
		len++;

	ret = copy_to_user(user, data_buffer, len);
	if (ret) {
		pr_err("Error: couldn't copy %d bytes.\n", ret);
		return -1;
	}

	return len;
}

static ssize_t led_driver_write(struct file *file, const char __user *user,
				size_t size, loff_t *off)
{
	unsigned int pin, value;
	int ret;

	memset(data_buffer, 0x0, MAX_DATA_BUFFER_LENGTH);

	ret = copy_from_user(data_buffer, user, size);
	if (ret) {
		pr_err("Error: couldn't copy %d bytes from userspace.\n", ret);
		return -1;
	}

	/* Clamp size if it crosses the buffer's limit */
	if (size > MAX_DATA_BUFFER_LENGTH)
		size = MAX_DATA_BUFFER_LENGTH - 1;

	if (sscanf(data_buffer, "%d,%d", &pin, &value) != 2) {
		pr_err("Invalid data format.\n");
		return size;
	}

	if (pin > 27 || pin < 0) {
		pr_err("Invalid pin number.\n");
		return -1;
	}

	if (value != 0 && value != 1) {
		pr_err("Invalid value.\n");
		return -1;
	}

	if (value)
		set_gpio_on(pin);
	else
		set_gpio_off(pin);

	pr_info("You said pin %d, value %d\n", pin, value);

	return size;
}

static int __init led_driver_init(void)
{
	pr_info("LED driver: init\n");

	memset(data_buffer, 0x0, MAX_DATA_BUFFER_LENGTH);

	driver_data = kzalloc(sizeof(*driver_data), GFP_KERNEL);
	if (!driver_data) {
		pr_err("Error when allocating memory for led_driver_data.\n");
		return -ENOMEM;
	}

	driver_data->gpio_registers = ioremap(BCM2837_GPIO_BASE_ADDR, IO_MEM_PAGE_SIZE);

	return 0;
}

static void __exit led_driver_exit(void)
{
	pr_info("LED driver: exit\n");
	kfree(driver_data);
	iounmap(driver_data->gpio_registers);
}

module_init(led_driver_init);
module_exit(led_driver_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Joao Marcos Costa");
MODULE_DESCRIPTION("LED driver for Raspberry Pi 3 B+");
MODULE_VERSION("1.0");
