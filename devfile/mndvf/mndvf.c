#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>

dev_t dev =0;
static int __init mod_init(void)
{
    if(alloc_chrdev_region(&dev,0,1,"CharDevice1")<0)
    {
      pr_err("Cannot allocate major number for device\n");
      return -1;
    }
    pr_info("Kernel module inserted successfully...\n");
    return 0;
    }
    static void __exit mod_exit(void)
    {
        unregister_chrdev_region(dev,1);
        pr_info("Kernel module removed successfully...\n");
    }

    module_init(mod_init);
    module_exit(mod_exit);

    MODULE_LICENSE("GPL");
    MODULE_AUTHOR("NAZ");
    MODULE_DESCRIPTION("Linux Driver(Mannually creating device file)");
