#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>

int add(int a,int b)
{
    return a+b;
}
EXPORT_SYMBOL(add);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NAZ");
MODULE_DESCRIPTION("ADDITION");