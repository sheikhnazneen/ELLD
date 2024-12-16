#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include"module.h"

int a=10,b=30;

static int __init test_init(void)
{
    pr_info("addition operation: %d\n",add(a,b));
    pr_info("subtraction operation: %d\n",add(a,b));
    pr_info("multiplication operation: %d\n",add(a,b));
    return 0;
}
 
 static void __exit test_exit(void)
 {
        pr_info("test exit\n");
 }

 module_init(test_init);
 module_exit(test_exit);

 MODULE_LICENSE("GPL");
 MODULE_AUTHOR("NAZ");
 MODULE_DESCRIPTION("CALCULATOR CREATED");