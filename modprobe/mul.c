#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>

int mul(int a,int b)
{
    return a*b;
}
EXPORT_SYMBOL(mul);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NAZ");
MODULE_DESCRIPTION("Multiplication");