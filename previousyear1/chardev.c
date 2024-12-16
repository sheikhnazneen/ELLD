#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <linux/init.h>

#define DEVICE_NAME "chardev"
#define MAJOR_NUM 240

// IOCTL Command
#define GETSTATS _IOR(MAJOR_NUM, 1, struct stats)

// Define stats structure
struct stats {
    int size;          // Size of data in buffer
    char buff[256];    // Kernel buffer
    int r_w;           // 0 for Read, 1 for Write
};

static struct stats kernel_stats; // Kernel buffer and statistics

// File operations
static int device_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device opened\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device closed\n");
    return 0;
}

static ssize_t device_read(struct file *file, char __user *user_buffer, size_t len, loff_t *offset) {
    int bytes_to_copy;

    if (*offset >= kernel_stats.size)
        return 0; // EOF

    bytes_to_copy = min(len, (size_t)(kernel_stats.size - *offset));
    if (copy_to_user(user_buffer, kernel_stats.buff + *offset, bytes_to_copy)) {
        printk(KERN_ERR "Failed to copy data to user\n");
        return -EFAULT;
    }

    *offset += bytes_to_copy;
    kernel_stats.r_w = 0; // Read operation
    printk(KERN_INFO "Device read %d bytes\n", bytes_to_copy);
    return bytes_to_copy;
}

static ssize_t device_write(struct file *file, const char __user *user_buffer, size_t len, loff_t *offset) {
    if (len > sizeof(kernel_stats.buff) - 1) {
        printk(KERN_ERR "Buffer overflow\n");
        return -ENOMEM;
    }

    if (copy_from_user(kernel_stats.buff, user_buffer, len)) {
        printk(KERN_ERR "Failed to copy data from user\n");
        return -EFAULT;
    }

    kernel_stats.buff[len] = '\0';
    kernel_stats.size = len;
    kernel_stats.r_w = 1; // Write operation
    printk(KERN_INFO "Device wrote %d bytes\n", len);
    return len;
}

static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case GETSTATS:
            if (copy_to_user((struct stats *)arg, &kernel_stats, sizeof(struct stats))) {
                printk(KERN_ERR "Failed to send stats to user\n");
                return -EFAULT;
            }
            printk(KERN_INFO "Sent stats to user\n");
            break;

        default:
            return -EINVAL;
    }
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
    .unlocked_ioctl = device_ioctl,
};

static int __init chardev_init(void) {
    memset(&kernel_stats, 0, sizeof(struct stats)); // Initialize kernel_stats
    kernel_stats.r_w = -1; // No recent activity

    if (register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops) < 0) {
        printk(KERN_ERR "Failed to register device\n");
        return -EIO;
    }

    printk(KERN_INFO "Character device registered with major number %d\n", MAJOR_NUM);
    return 0;
}

static void __exit chardev_exit(void) {
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    printk(KERN_INFO "Character device unregistered\n");
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NAZ");
MODULE_DESCRIPTION("Character Device Driver with Read/Write and IOCTL Functionality");
