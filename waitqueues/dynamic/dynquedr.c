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
MODULE_AUTHOR("NAZ");
MODULE_DESCRIPTION("Linux Driver[WaitQueue Dynmic Method]");

uint32_t read_count =0;
uint32_t write_count =0;

static struct task_struct *wait_thread;

dev_t dev =0;

static struct class *dev_class;
static struct cdev dr_cdev;

wait_queue_head_t wait_queue_dr;

int wait_queue_flag =0;


/*Function Prototype*/
static int __init dr_driver_init(void);
static void __exit dr_driver_exit(void);


/*Driver Function*/
static int dr_open(struct inode *inode,struct file *file);
static int dr_release(struct inode *inode,struct file *file);

static ssize_t dr_read(struct file *flip, char __user *buf,size_t len, loff_t *off);
static ssize_t dr_write(struct file *flip, const char *buf,size_t len, loff_t *off);


/*File Operation Structure*/
static struct file_operations fops =
{
	.owner = THIS_MODULE,
	.read = dr_read,
	.write = dr_write,
	.open = dr_open,
	.release = dr_release,
};


/*Thread Function*/
static int wait_function(void *unused)
{
	while(1)
	{
		pr_info("Waiting For Event...\n");
		wait_event_interruptible(wait_queue_dr,wait_queue_flag !=0);
		if(wait_queue_flag ==2)
		{
			pr_info("Event Came From Exit Function\n");
			return 0;
		}
		if(wait_queue_flag ==1)
		{
			pr_info("Event Came From Read Function - %d\n",++read_count);
		}
		if(wait_queue_flag ==3)
		{
			pr_info("Event Came From Write Function\n",++write_count);
			wait_queue_flag =0;
		}
		wait_queue_flag =0;
	}
	return 0;
}


/*This Function Will Be Called When We Open The Device File*/
static int dr_open(struct inode *inode, struct file *file)
{
	pr_info("Device File Opened\n");
	return 0;
}


/*This Function Will Be Called When We Read The Device File*/
static ssize_t dr_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	pr_info("Read Function\n");
	wait_queue_flag =1;
	wake_up_interruptible(&wait_queue_dr);
	return 0;
}


/*This Function Will BE Called When We Write The Device File*/
static ssize_t dr_write(struct file *filp,const char __user *buf,size_t len, loff_t *off)
{
	pr_info("Write Funtion\n");
	wait_queue_flag =3;
	wake_up_interruptible(&wait_queue_dr);
	return len;
}


/*This Function Will Be Called When We Close The Device File*/
static int dr_release(struct inode *inode,struct file *file)
{
	pr_info("Device File CLosed\n");
	return 0;
}


/*Module Init Function*/
static int __init dr_driver_init(void)
{
	/*Allocating Major Number*/
	if((alloc_chrdev_region(&dev,0,1,"Dyn_dev")) <0)
	{
		pr_info("Cannot Allocate Major Number\n");
		return -1;
	}
	pr_info("Major = %d Minor = %d\n",MAJOR(dev),MINOR(dev));


	/*Creating cdev Structure*/
	cdev_init(&dr_cdev,&fops);


	/*Adding Character Device To The System*/
	if((cdev_add(&dr_cdev,dev,1)) <0)
	{
		pr_info("Cannot Add The Device To The System\n");
		goto r_class;
	}


	/*Creating Struct Class*/
	if((dev_class = class_create(THIS_MODULE,"dr_class")) ==NULL)
	{
		pr_info("Cannot Create The Struct Class\n");
	goto r_class;
	}

	/*Creating Device*/
	if((device_create(dev_class,NULL,dev,NULL,"dr_device")) ==NULL)
	{
		pr_info("Cannot Create The Device\n");
		goto r_device;
	}


	/*Initilize Wait Queue*/
	init_waitqueue_head(&wait_queue_dr);


	/*Create The Kernel Thread With Name 'mythread'*/
	wait_thread = kthread_create(wait_function,NULL,"WaitThread");

	if(wait_thread)
	{
		pr_info("Thread Created Successfully\n");
		wake_up_process(wait_thread);
	}
	else
	{
		pr_info("Thread Creation Failed\n");
	}

	pr_info("Device Driver Inserted\n");
	return 0;

r_device:
	class_destroy(dev_class);
r_class:
	unregister_chrdev_region(dev,1);
	return -1;
}

/*Module _exit Function*/
static void __exit dr_driver_exit(void)
{
	wait_queue_flag =2;
	wake_up_interruptible(&wait_queue_dr);

	device_destroy(dev_class,dev);
	class_destroy(dev_class);

	cdev_del(&dr_cdev);
	unregister_chrdev_region(dev,1);

	pr_info("Device Driver Removed.\n");
}

module_init(dr_driver_init);
module_exit(dr_driver_exit);