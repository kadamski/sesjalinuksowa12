#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init ftrace_printk_init(void)
{
	trace_printk("Entering (%s:%d)...\n", __FUNCTION__, __LINE__);
	return 0;
}

static void __exit ftrace_printk_exit(void)
{
	trace_printk("Exiting (%s:%d)...\n", __FUNCTION__, __LINE__);
}

module_init(ftrace_printk_init);
module_exit(ftrace_printk_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krzysztof Adamski <k@japko.eu>");
MODULE_DESCRIPTION("ftrace_printk");
