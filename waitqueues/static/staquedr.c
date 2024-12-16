#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/uaccess.h>
#include<linux/kthread.h>
#include<linux/wait.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Darky");
MODULE_DESCRIPTION("Linux Device Driver[WaitQueue Static Method]");

uint32_t read_count = 0;
static struct task_struct *wait_thread;


DECLARE_WAIT_QUEUE_HEAD(wait_queue_etx);


dev_t dev = 0;

static struct class *dev_class;
static struct cdev etx_cdev;

int wait_queue_flag = 0;

/*Function Prototype*/
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);


/*Driver Function*/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode,struct file *file);

static ssize_t etx_read(struct file *flip,char __user *buf,size_t len, loff_t *off);
static ssize_t etx_write(struct file *flip,const char *buf, size_t len, loff_t *off);


/**File Operation Structure**/
static struct file_operations fops=
{
	.owner = THIS_MODULE,
	.read = etx_read,
	.write = etx_write,
	.open = etx_open,
	.release = etx_release,
};


/*Thread Function*/
static int wait_function(void *unused)
{
	while(1)
	{
		pr_info("Waiting For Event...\n");
		wait_event_interruptible(wait_queue_etx,wait_queue_flag !=0);
		if(wait_queue_flag ==2)
		{
			pr_info("Event Came From Exit Function.\n");
			return 0;
		}
		pr_info("Event Came From Read Function - %d\n",++read_count);
		wait_queue_flag =0;
	}
	do_exit(0);
	return 0;
}

/*This Function Will Be Called When We Open The Device File*/
static int etx_open(struct inode *inode , struct file *file)
{
	pr_info("Device FIle Opened...\n");
	return 0;
}

/*This Function Will Be Called When We Read The File*/
static ssize_t etx_read(struct file *file,char __user *buf,size_t len, loff_t *off)
{
	pr_info("Read Function\n");
	wait_queue_flag =1;
	wake_up_interruptible(&wait_queue_etx);
	return 0;
}


/*This Function Will Be Called When We Write The Device FIle*/
static ssize_t etx_write(struct file *flip,const char __user *buf,size_t len, loff_t *off)
{
	pr_info("Write Function\n");
	return len;
}


/*This Function Will Be Called  When We Close The Device File*/
static int etx_release(struct inode *inode,struct file *file)
{
	pr_info("Device FIle Closed...\n");
	return 0;
}


/*Module Init Function*/
static int __init etx_driver_init(void)
{
	/*Allocating Major Number*/
	if((alloc_chrdev_region(&dev,0,1,"sta_dev")) <0)
	{
		pr_info("Cannot Allocate MAjor Number\n");
		return -1;
	}
	pr_info("Major = %d Minor = %d\n",MAJOR(dev),MINOR(dev));

	/*Creating cdev Structure*/
	cdev_init(&etx_cdev,&fops);
	etx_cdev.owner = THIS_MODULE;
	etx_cdev.ops = &fops;


	/*Adding Character Device To The System*/
	if((cdev_add(&etx_cdev,dev,1)) <0)
	{
		pr_info("Cannot Add The Device To The System\n");
		goto r_class;
	}


	/*Creating Struct Class*/
	if((dev_class = class_create(THIS_MODULE,"sta_dev")) ==NULL)
	{
		pr_info("Cannot Create The Struct Class\n");
		goto r_class;
	}


	/*Creating Device*/
	if((device_create(dev_class,NULL,dev,NULL,"sta_dev")) == NULL)
	{
		pr_info("Cannot Create The Device 1\n");
		goto r_device;
	}



	/*Creating The Kernel Thread With 'mythread'*/
	wait_thread = kthread_create(wait_function,NULL,"WaitThread");
	if(wait_thread)
	{
		pr_info("Thread Creation Successfully\n");
		wake_up_process(wait_thread);
	}
	else
	{
		pr_info("Thread Creation Failed\n");
	}

	pr_info("Device Driver Inserted.\n");
	return 0;

r_device:
	class_destroy(dev_class);
r_class:
	unregister_chrdev_region(dev,1);
	return -1;
}


/*Module Exit Function*/
static void __exit etx_driver_exit(void)
{
	wait_queue_flag =2;
	wake_up_interruptible(&wait_queue_etx);
	device_destroy(dev_class,dev);
	class_destroy(dev_class);
	cdev_del(&etx_cdev);
	unregister_chrdev_region(dev,1);
	pr_info("Device Driver Removed.\n");
}


module_init(etx_driver_init);
module_exit(etx_driver_exit);