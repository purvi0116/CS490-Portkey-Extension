#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/uaccess.h>
#include<linux/ioctl.h>

#define mem_size 1024

// Define the IOCTL code
#define WR_DATA _IOW('a','a',int32_t*)
#define RD_DATA _IOR('a','b',int32_t*)

int32_t val = 0;


dev_t dev = 0;
static struct class *dev_class;
static struct cdev my_cdev;
uint8_t *kernel_buffer;

static int __init chr_driver_init(void);
static void __exit chr_driver_exit(void);
static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t my_write(struct file *filp, const char *buf, size_t len, loff_t *off);
static long chr_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write,
	.open = my_open,
	.unlocked_ioctl = chr_ioctl,
	.release = my_release
};

static int my_open(struct inode *inode, struct file *file){
	// Creating Physical Memory
	if((kernel_buffer = kmalloc(mem_size, GFP_KERNEL)) == 0){
		printk(KERN_INFO "Cannot allocate memory to the kernel.\n");
		return -1;
	}
	printk(KERN_INFO "Device file opened.\n");
	return 0;
}

static int my_release(struct inode *inode, struct file *file){
	kfree(kernel_buffer);
	printk(KERN_INFO"Device file closed.\n");
	return 0;
}

static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off){
	// whenever read function called from user space, this funciton is called
	copy_to_user(buf, kernel_buffer, mem_size);
	printk(KERN_INFO"Data read : DONE.\n");
	return mem_size;
}

static ssize_t my_write(struct file *filp, const char *buf, size_t len, loff_t *off){
	copy_from_user(kernel_buffer, buf, len);
	printk(KERN_INFO"Data is written successfully.\n");;
	return len;
}

static long chr_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	switch(cmd){
		case WR_DATA:
			copy_from_user(&val,(int32_t*)arg, sizeof(val));
			printk(KERN_INFO "val = %d\n",val);
			break;
		case RD_DATA:
			copy_to_user((int32_t*)arg,&val,sizeof(val));
			break;
	}
	return 0;
}

static int __init chr_driver_init(void){
	// Allocate Major number
	if(alloc_chrdev_region(&dev,0,1,"my_dev") < 0){
		printk(KERN_INFO"Cannot allocate the major number.\n");
		return -1;
	}

	printk(KERN_INFO"Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

	// creating cdev structure
	cdev_init(&my_cdev, &fops);

	// Adding character device to the system
	if((cdev_add(&my_cdev, dev, 1)) < 0){
		printk(KERN_INFO"Cannot add the device to the system.\n");
		goto r_class;
	}

	// creating struct class
	if((dev_class = class_create(THIS_MODULE, "my_class")) == NULL){
		printk(KERN_INFO"Cannot create the struct class.\n");
		goto r_class;
	}
	
	// creating device
	if((device_create(dev_class, NULL, dev, NULL, "my_device")) == NULL){
		printk(KERN_INFO"Cannot create the device");
		goto r_device;
	}

	printk(KERN_INFO"Device driver insert...done properly\n");
	return 0;

r_device:
	class_destroy(dev_class);
r_class:
	unregister_chrdev_region(dev, 1);
	return -1;
}

void __exit chr_driver_exit(void) {
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO"Device driver removed successfully.\n");
}

module_init(chr_driver_init);
module_exit(chr_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Purvi Poonia");
MODULE_DESCRIPTION("The character device driver");



