#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "led_driver"
#define MAJOR_NUM 240
#define GPIO_LED 23

// IOCTL Command to set blink interval
#define SET_PERIOD _IOW(MAJOR_NUM, 1, int)

// Blink interval in milliseconds (default 500 ms)
static int blink_period = 500;

// Timer and GPIO state
static struct timer_list led_timer;
static bool led_state = false;

// File operations
static int device_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "LED driver opened\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "LED driver closed\n");
    return 0;
}

// IOCTL functionality to set blink interval
static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int new_period;

    switch (cmd) {
        case SET_PERIOD:
            if (copy_from_user(&new_period, (int __user *)arg, sizeof(new_period))) {
                printk(KERN_ERR "Failed to copy period from user\n");
                return -EFAULT;
            }
            if (new_period < 100) {
                printk(KERN_ERR "Period too low, must be >= 100 ms\n");
                return -EINVAL;
            }
            blink_period = new_period;
            printk(KERN_INFO "Blink period set to %d ms\n", blink_period);
            break;

        default:
            return -EINVAL;
    }
    return 0;
}

// Timer callback function
static void led_blink_callback(struct timer_list *timer) {
    led_state = !led_state; // Toggle LED state
    gpio_set_value(GPIO_LED, led_state); // Update GPIO

    // Restart the timer
    mod_timer(&led_timer, jiffies + msecs_to_jiffies(blink_period));
}

// File operations structure
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .unlocked_ioctl = device_ioctl,
};

static int __init led_driver_init(void) {
    int ret;

    // Register character device
    ret = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
    if (ret < 0) {
        printk(KERN_ERR "Failed to register device\n");
        return ret;
    }

    // Request GPIO
    if (!gpio_is_valid(GPIO_LED)) {
        printk(KERN_ERR "Invalid GPIO %d\n", GPIO_LED);
        unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
        return -ENODEV;
    }

    ret = gpio_request(GPIO_LED, "GPIO_LED");
    if (ret) {
        printk(KERN_ERR "Failed to request GPIO %d\n", GPIO_LED);
        unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
        return ret;
    }

    gpio_direction_output(GPIO_LED, 0); // Set GPIO as output
    gpio_set_value(GPIO_LED, 0); // Turn off LED initially

    // Initialize timer
    timer_setup(&led_timer, led_blink_callback, 0);
    mod_timer(&led_timer, jiffies + msecs_to_jiffies(blink_period));

    printk(KERN_INFO "LED driver initialized, blinking at %d ms\n", blink_period);
    return 0;
}

static void __exit led_driver_exit(void) {
    del_timer_sync(&led_timer); // Delete timer
    gpio_set_value(GPIO_LED, 0); // Turn off LED
    gpio_free(GPIO_LED); // Free GPIO
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME); // Unregister device
    printk(KERN_INFO "LED driver removed\n");
}

module_init(led_driver_init);
module_exit(led_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NAZ");
MODULE_DESCRIPTION("LED Driver Using GPIO and Kernel Timers");
