#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/wait.h>
#include <linux/uaccess.h>
#define DEVICE_COUNT 2
#define BUFFER_SIZE 50
#define DEV_NAME "multi2"
static dev_t dev_num;
static struct cdev my_cdevs[DEVICE_COUNT];
static struct semaphore sem;
static wait_queue_head_t wq;
static char buffer[BUFFER_SIZE];
static int buffer_length = 0;
static int mycdev_open(struct inode *inode, struct file *filep)
{
pr_info("Device Opened\n");
return 0;
}
static int mycdev_release(struct inode *inode, struct file *filep)
{
pr_info("Device Closed\n");
return 0;
}
static ssize_t mycdev_read(struct file *filep, char __user *buf, size_t len, loff_t *pos)
{
int ret;
if (buffer_length == 0) {
pr_info("Buffer is empty\n");
return 0;
}
// Copy data from kernel buffer to user space
ret = copy_to_user(buf, buffer, buffer_length);
if (ret) {
pr_err("Failed to read %d bytes\n", ret);
return -EFAULT;
}
pr_info("Data read: %s\n", buffer);
// Reset the buffer after read
buffer_length = 0;
// Wake up the waiting writer (if any)
wake_up(&wq);
return buffer_length;
}
static ssize_t mycdev_write(struct file *filep, const char __user *buf, size_t len, loff_t
*pos)
{
int ret;
if (len > BUFFER_SIZE) {
pr_err("Data size exceeds buffer size\n");
return -EINVAL;
}
// Wait until the buffer is empty
if (down_interruptible(&sem)) {
return -ERESTARTSYS;
}
// If there is data in the buffer, sleep the writer until read occurs
if (buffer_length > 0) {
pr_info("Buffer is full, writer sleeping\n");
wait_event_interruptible(wq, buffer_length == 0);
}
// Copy data from user space to kernel buffer
ret = copy_from_user(buffer, buf, len);
if (ret) {
pr_err("Failed to write %d bytes\n", ret);
up(&sem);
return -EFAULT;
}
buffer_length = len;
pr_info("Data written: %s\n", buffer);
// Release the semaphore after successful write
up(&sem);
return len;
}
static struct file_operations my_cdev_fops = {
.owner = THIS_MODULE,
.open = mycdev_open,
.release = mycdev_release,
.read = mycdev_read,
.write = mycdev_write,
};
static int __init mycdev_init(void)
{
int ret, i;
pr_info("Initializing Multi-Device Driver\n");
// Allocate device numbers
ret = alloc_chrdev_region(&dev_num, 0, DEVICE_COUNT, DEV_NAME);
if (ret < 0) {
pr_err("Failed to allocate device numbers\n");
return ret;
}
pr_info("Major number: %d, Minor number: %d\n", MAJOR(dev_num), MINOR(dev_num));
// Initialize semaphore
sema_init(&sem, 1);
// Initialize wait queue
init_waitqueue_head(&wq);
// Initialize each device and register
for (i = 0; i < DEVICE_COUNT; i++) {
cdev_init(&my_cdevs[i], &my_cdev_fops);
ret = cdev_add(&my_cdevs[i], dev_num + i, 1);
if (ret) {
pr_err("Failed to add cdev for device %d\n", i);
unregister_chrdev_region(dev_num, DEVICE_COUNT);
return ret;
}
}
pr_info("Driver Loaded Successfully\n");
return 0;
}
static void __exit mycdev_exit(void)
{
int i;
pr_info("Exiting Multi-Device Driver\n");
// Remove each device
for (i = 0; i < DEVICE_COUNT; i++) {
cdev_del(&my_cdevs[i]);
}
// Unregister device numbers
unregister_chrdev_region(dev_num, DEVICE_COUNT);
pr_info("Driver Unloaded\n");
}
module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("NAZ");
MODULE_DESCRIPTION("Multi-Device Character Driver with Semaphore and Wait Queue");