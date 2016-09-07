#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("roliveir");
MODULE_DESCRIPTION("Hello World");

static int __init hello_world(void)
{
    printk(KERN_DEBUG "Hello World!\n");
    return 0;
}

static void __exit bye_world(void)
{
    
}

module_init(hello_world);
module_exit(bye_world);
