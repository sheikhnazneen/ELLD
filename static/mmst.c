#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>

dev_t dev = MKDEV(269,0);//statically giving major number

static int __init mod_init(void)
{
	register_chrdev_region(dev,1,"charDriver");
	printk(KERN_INFO "\nMajor = %d Minor = %d\n",MAJOR(dev),MINOR(dev));
	printk(KERN_INFO "\nKernel char Module Inserted Successfully...\n");
	return 0;
}

static void __exit mod_exit(void)
{
	unregister_chrdev_region(dev,1);
	printk(KERN_INFO "\nKernerl char Module Removed Successfully..\n");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Priyanshu");
MODULE_DESCRIPTION("Program For Create char module with->MAJOR AND MINOR");