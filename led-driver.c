// SPDX-License-Identifier: GPL-2.0

#include <linux/kernel.h>
#include <linux/module.h>

#define MAX_DATA_BUFFER_LENGTH 1024

static char data_buffer[MAX_DATA_BUFFER_LENGTH];

static int __init led_driver_init(void)
{
	pr_info("LED driver: init\n");

	memset(data_buffer, 0x0, MAX_DATA_BUFFER_LENGTH);

	return 0;
}

static void __exit led_driver_exit(void)
{
	pr_info("LED driver: exit\n");
}

module_init(led_driver_init);
module_exit(led_driver_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Joao Marcos Costa");
MODULE_DESCRIPTION("LED driver for Raspberry Pi 3 B+");
MODULE_VERSION("1.0");
