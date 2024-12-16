#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/timer.h>
#include<linux/jiffies.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Darky");
MODULE_DESCRIPTION("LDD[Timer]");

/*Declaration*/
#define TIMEOUT 1000 //1second = 1000ms
static struct timer_list dr_timer;
static unsigned int count = 0;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev dr_cdev;

static int __init dr_driver_init(void);
static void __exit dr_driver_exit(void);

/*Driver function Declaration*/
static int dr_open(struct inode *inode,struct file *file);
static int dr_release(struct inode *inode,struct file *file);

static ssize_t dr_read(struct file *file,char __user *ubuff,size_t count, loff_t *off);
static ssize_t dr_write(struct file *file,const char __user *ubuff,size_t count, loff_t *off);

struct file_operations fops =
{
	.owner = THIS_MODULE,
	.open = dr_open,
	.read = dr_read,
	.write = dr_write,
	.release = dr_release,
};

void timer_callback(struct timer_list *data)
{
	pr_info("Timer Callback Function Called [%d]\n",count++);
	mod_timer(&dr_timer,jiffies+msecs_to_jiffies(TIMEOUT));
}


static int dr_open(struct inode *inode,struct file *file)
{
	pr_info("Opening Device File.\n");
	return 0;
}

static ssize_t dr_read(struct file *file,char __user *ubuff,size_t count, loff_t *off)
{
	pr_info("In Read Function.\n");
	return 0;
}

static ssize_t dr_write(struct file *file,const char __user *ubuff, size_t count, loff_t *off)
{
	pr_info("In Write Function.\n");
	return count;
}


static int dr_release(struct inode *inode, struct file *file)
{
	pr_info("Closing Device File\n");
	return 0;
}


static int dr_driver_init(void)
{
	/*Allocating MAjor Number*/
	if(alloc_chrdev_region(&dev,0,1,"dr_Timer")<0)
	{
		pr_err("Error In Allocating Major Number.\n");
		return -1;
	}
	pr_info("Allocating: Major: %d Minor: %d\n",MAJOR(dev),MINOR(dev));

	/*Creating Device To The System[Creating cdev Structure]*/
	cdev_init(&dr_cdev,&fops);


	/*Adding Character Device*/
	if(cdev_add(&dr_cdev,dev,1)<0)
	{
		pr_err("Cannot Add Device To The System.\n");
		goto r_class;
	}


	/*Creating Struct Class*/
	if(dev_class = class_create(THIS_MODULE,"dr_class") ==NULL)
	{
		pr_err("Cannot Create The Class Strcuture");
		goto r_class;
	}


	/*Creating Device*/
	if(device_create(dev_class,NULL,dev,NULL,"dr_device") ==NULL)
	{
		pr_err("Cannot Create The Device\n");
		goto r_device;
	}


	/*Setup Timer To Call Timer Callback*/
	timer_setup(&dr_timer,timer_callback,0);
	mod_timer(&dr_timer,jiffies+msecs_to_jiffies(TIMEOUT));
	pr_info("Device Driver Inserted.\n");
	return 0;


r_device:
	class_destroy(dev_class);

r_class:
	unregister_chrdev_region(dev,1);
	return -1;
}


static void dr_driver_exit(void)
{
	printk("In Exit Function\n");
	del_timer(&dr_timer);  //removing kernel timer when uloading the module
	printk("Timer Removed\n");
	device_destroy(dev_class,dev);
	class_destroy(dev_class);
	cdev_del(&dr_cdev);
	printk("Success In Deactivate\n");
	unregister_chrdev_region(dev,1);
	pr_info("Succes In Deregister\n");
	printk("Device Driver Removed\n");
}


module_init(dr_driver_init);
module_exit(dr_driver_exit);