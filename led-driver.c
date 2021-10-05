// SPDX-License-Identifier: GPL-2.0

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define MAX_DATA_BUFFER_LENGTH 1024

static struct proc_dir_entry *led_driver_proc_entry;

static char data_buffer[MAX_DATA_BUFFER_LENGTH];

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
	int ret;

	memset(data_buffer, 0x0, MAX_DATA_BUFFER_LENGTH);

	ret = copy_from_user(data_buffer, user, size);
	if (ret) {
		pr_err("Error: couldn't copy %d bytes from userspace.\n", ret);
		return -1;
	}

	pr_info("You said: %s\n", data_buffer);

	return size;
}

static const struct proc_ops led_driver_proc_fops = {
	.proc_read = led_driver_read,
	.proc_write = led_driver_write,
};

static int __init led_driver_init(void)
{
	pr_info("LED driver: init\n");

	memset(data_buffer, 0x0, MAX_DATA_BUFFER_LENGTH);

	led_driver_proc_entry = proc_create("led-driver", 0666, NULL,
					    &led_driver_proc_fops);
	if (!led_driver_proc_entry) {
		pr_err("Failed to create procfs entry\n");
		return -1;
	}

	return 0;
}

static void __exit led_driver_exit(void)
{
	pr_info("LED driver: exit\n");
	proc_remove(led_driver_proc_entry);
}

module_init(led_driver_init);
module_exit(led_driver_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Joao Marcos Costa");
MODULE_DESCRIPTION("LED driver for Raspberry Pi 3 B+");
MODULE_VERSION("1.0");
