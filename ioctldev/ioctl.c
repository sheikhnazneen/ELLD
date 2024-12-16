#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/kdev_t.h>
#include "ioctl.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OMEN");
MODULE_DESCRIPTION("Ioctl Driver");

#define COUNT 1
#define DEV_NAME "Darkdev"

dev_t dev_num = 0;

static struct cdev mycdev;

static int mycdev_open(struct inode *inode, struct file *filep)
{
	pr_info("mycdev_open Invoked...\n");
	return 0;
}

static int mycdev_release(struct inode *inode, struct file *filep)
{
	pr_info("mycdev_release Invoked...\n");
	return 0;
}

static ssize_t mycdev_write(struct file *filep, const char __user *buf, size_t len, loff_t *pos)
{
	pr_info("mycdev_write Invoked...");
	return 0;
}

static ssize_t mycdev_read(struct file *filep, char __user *buf, size_t len, loff_t *pos)
{
	pr_info("mycdev_read Invoked...");
	return 0;
}

static long mycdev_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	pr_info("mycdev_ioctl Invoked...");
	switch(cmd)
	{
		case CMD_1 :
			pr_alert("This Is CMD_1 Command.\n");
			break;
		case CMD_2 :
			pr_alert("This Is CMD_2 Command. Received arg : %d\n",arg);
			break;
		default:
			pr_info("Invalid Command\n");
			//break;
	}
	return 0;
}

static struct file_operations mycdev_fops =
{
	.owner = THIS_MODULE,
	.open = mycdev_open,
	.release = mycdev_release,
	.write = mycdev_write,
	.read = mycdev_read,
	.unlocked_ioctl = mycdev_ioctl,
};

static int __init mycdev_init(void)
{
	pr_info("mycdec_init Is Invoked\n");
	alloc_chrdev_region(&dev_num,0,COUNT,DEV_NAME);

	pr_info("MAJOR dev_num %d\n",MAJOR(dev_num));
	pr_info("Minor dev_num %d\n",MINOR(dev_num));

	cdev_init(&mycdev,&mycdev_fops);
	cdev_add(&mycdev,dev_num,1);

	pr_info("mycdev Driver Loaded...\n");
	return 0;
}

static void __exit mycdev_exit(void)
{
	pr_info("mycdev_exit Is Invoked\n");
	cdev_del(&mycdev);

	unregister_chrdev_region(dev_num,1);
	pr_info("mycdev Driver Unloaded...\n");
}

module_init(mycdev_init);
module_exit(mycdev_exit);