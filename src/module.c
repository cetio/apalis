#include <linux/module.h>
#include <linux/kernel.h>
#include "pyra.h"

static int __init apalis_init(void) 
{
    printk(KERN_INFO "Apalis loaded\n");
    return 0;
}

static void __exit apalis_exit(void) 
{
    printk(KERN_INFO "Apalis unloaded\n");
}

module_init(apalis_init);
module_exit(apalis_exit);