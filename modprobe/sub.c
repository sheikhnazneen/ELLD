#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>

int sub(int a,int b)
{
    return a-b;
}
EXPORT_SYMBOL(sub);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NAZ");
MODULE_DESCRIPTION("Subtract");