#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h> // For copy_to_user and copy_from_user

#define MEM_SIZE 1024
dev_t dev = 0;

static struct cdev char_cdev;
uint8_t *kernel_buffer;

static int __init char_driver_init(void);
static void __exit char_driver_exit(void);
static int char_open(struct inode *inode, struct file *file);
static int char_release(struct inode *inode, struct file *file);
static ssize_t char_read(struct file *filep, char __user *buf, size_t len, loff_t *off);
static ssize_t char_write(struct file *filep, const char __user *buf, size_t len, loff_t *off);

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = char_read,
    .write = char_write,
    .open = char_open,
    .release = char_release,
};

static int char_open(struct inode *inode, struct file *filep)
{
    pr_info("\nDevice file opened..\n");
    return 0;
}

static int char_release(struct inode *inode, struct file *filep)
{
    pr_info("\nDevice file closed..\n");
    return 0;
}

static ssize_t char_read(struct file *filep, char __user *buf, size_t len, loff_t *off)
{
    // Ensure we only read the requested number of bytes (up to MEM_SIZE)
    if (*off >= MEM_SIZE) {
        return 0;  // End of file
    }

    // Copy data from kernel buffer to user space
    if (copy_to_user(buf, kernel_buffer, MEM_SIZE)) {
        pr_err("Data read: error\n");
        return -EFAULT;
    }

    pr_info("Data read: done\n");
    *off += MEM_SIZE;  // Update the offset
    return MEM_SIZE;   // Number of bytes read
}

static ssize_t char_write(struct file *filep, const char __user *buf, size_t len, loff_t *off)
{
    // Ensure we don't write more than MEM_SIZE
    if (len > MEM_SIZE) {
        pr_err("Data write: error, buffer overflow\n");
        return -EINVAL;
    }

    // Copy data from user space to kernel buffer
    if (copy_from_user(kernel_buffer, buf, len)) {
        pr_err("Data write: error\n");
        return -EFAULT;
    }

    pr_info("Data write: done\n");
    return len;  // Number of bytes written
}

static int __init char_driver_init(void)
{
    // Allocate a major number dynamically
    if (alloc_chrdev_region(&dev, 0, 1, "CharDevice") < 0) {
        pr_err("Cannot allocate major number\n");
        return -1;
    }
    pr_info("\nMajor->%d Minor->%d\n", MAJOR(dev), MINOR(dev));

    // Initialize the cdev structure and add the device
    cdev_init(&char_cdev, &fops);
    if (cdev_add(&char_cdev, dev, 1) < 0) {
        pr_err("Cannot add the device to the system\n");
        unregister_chrdev_region(dev, 1);  // Roll back the allocation if add fails
        return -1;
    }

    // Allocate memory for the kernel buffer
    kernel_buffer = kmalloc(MEM_SIZE, GFP_KERNEL);
    if (!kernel_buffer) {
        pr_err("Cannot allocate memory in kernel\n");
        cdev_del(&char_cdev);  // Roll back if memory allocation fails
        unregister_chrdev_region(dev, 1);
        return -ENOMEM;
    }

    strcpy(kernel_buffer, "Hello World");  // Initialize the buffer with some data
    pr_info("Device driver insert...done\n");
    return 0;
}

static void __exit char_driver_exit(void)
{
    // Free allocated memory and clean up
    kfree(kernel_buffer);
    cdev_del(&char_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Device driver removed\n");
}

module_init(char_driver_init);
module_exit(char_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NAZ");
MODULE_DESCRIPTION("Character device driver");
