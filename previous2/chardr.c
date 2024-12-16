#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/semaphore.h>
#include <linux/wait.h>

#define BUFFER_SIZE 50

static dev_t dev_nums;
static struct cdev my_cdev;
static struct semaphore sem;
static wait_queue_head_t wait_queue;
static char kernel_buffer[BUFFER_SIZE];
static int data_available = 0;

static ssize_t my_read(struct file *file, char __user *user_buf, size_t count, loff_t *offset) {
    int bytes_read;

    if (count > BUFFER_SIZE)
        count = BUFFER_SIZE;

    // Wait if no data is available
    if (!data_available) {
        wait_event_interruptible(wait_queue, data_available);
    }

    if (copy_to_user(user_buf, kernel_buffer, count)) {
        return -EFAULT;
    }

    data_available = 0;  // Reset data availability flag
    wake_up_interruptible(&wait_queue);  // Wake up waiting write process

    return count;
}

static ssize_t my_write(struct file *file, const char __user *user_buf, size_t count, loff_t *offset) {
    if (count > BUFFER_SIZE)
        count = BUFFER_SIZE;

    // Lock critical section
    if (down_interruptible(&sem)) {
        return -ERESTARTSYS;
    }

    if (copy_from_user(kernel_buffer, user_buf, count)) {
        up(&sem);
        return -EFAULT;
    }

    data_available = 1;  // Mark data as available
    up(&sem);  // Release semaphore

    // Put the writer to sleep until data is read
    wait_event_interruptible(wait_queue, !data_available);

    return count;
}

static int my_open(struct inode *inode, struct file *file) {
    return 0;
}

static int my_release(struct inode *inode, struct file *file) {
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
    .open = my_open,
    .release = my_release,
};

static int __init my_driver_init(void) {
    if (alloc_chrdev_region(&dev_nums, 0, 2, "my_char_driver") < 0) {
        pr_err("Failed to allocate device numbers\n");
        return -1;
    }

    cdev_init(&my_cdev, &fops);
    if (cdev_add(&my_cdev, dev_nums, 2) < 0) {
        pr_err("Failed to add cdev\n");
        unregister_chrdev_region(dev_nums, 2);
        return -1;
    }

    init_waitqueue_head(&wait_queue);
    sema_init(&sem, 1);  // Initialize semaphore

    pr_info("Character driver loaded with device numbers %d and %d\n", MAJOR(dev_nums), MINOR(dev_nums));
    return 0;
}

static void __exit my_driver_exit(void) {
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_nums, 2);
    pr_info("Character driver unloaded\n");
}

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Character driver for multiple devices with synchronization");
