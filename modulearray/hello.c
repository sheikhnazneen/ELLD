#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int valueETX = 0;           // Single integer parameter
static char *nameETX = "default"; // String parameter
static int arr_valueETX[10] = {0}; // Integer array parameter
static int arr_size = 0;          // To hold array size




module_param(valueETX, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(valueETX, "An integer parameter");

module_param(nameETX, charp, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(nameETX, "A string parameter");

module_param_array(arr_valueETX, int, &arr_size, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(arr_valueETX, "An array of integers");

static int __init hello_init(void)
{
    int i;

    printk(KERN_INFO "Module initialized\n");
    printk(KERN_INFO "Integer valueETX: %d\n", valueETX);
    printk(KERN_INFO "String nameETX: %s\n", nameETX);

    printk(KERN_INFO "Array values:\n");
    for (i = 0; i < arr_size; i++)
    {
        printk(KERN_INFO "arr_valueETX[%d] = %d\n", i, arr_valueETX[i]);
    }
    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "Module exiting\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RAMBO");
MODULE_DESCRIPTION("Module to demonstrate parameters.");
