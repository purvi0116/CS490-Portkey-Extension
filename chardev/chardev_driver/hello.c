/*
* hello.c – The Hello, World! Kernel Module
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

/*
* hello_init – the init function, called when the module is loaded.
* Returns zero if successfully loaded, nonzero otherwise.
*/
static int hello_init(void)
{
printk(KERN_ALERT "Hello Kernel.\n");
return 0;
}
/*
* hello_exit – the exit function, called when the module is removed.
*/
static void hello_exit(void)
{
printk(KERN_ALERT "Exiting Kernel.\n");
}
module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Purvi");
MODULE_DESCRIPTION("A Hello, World Module");
