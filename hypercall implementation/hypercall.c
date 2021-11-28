#include <linux/init.h>          
#include <linux/module.h>        
#include <linux/device.h>        
#include <linux/kernel.h>       
#include <linux/fs.h>            
#include <asm/uaccess.h>         
#include<linux/mm.h>
#include<linux/sched.h>
#include<linux/moduleparam.h>
#include<linux/types.h>
#include<linux/errno.h>
#include<linux/rmap.h>
#include<linux/pagemap.h>
#include<linux/ioctl.h>
#include<linux/uio.h>
#include<linux/delay.h>
#include<linux/cpumask.h>
#include<uapi/linux/kvm_para.h>
#include<linux/linkage.h>
#include<linux/kvm_host.h>
#include <../arch/x86/kvm/x86.h>
#include <linux/net.h>
#include <net/sock.h>
#include <linux/tcp.h>
#include <net/tcp.h>
#include <linux/in.h>
#include <asm/uaccess.h>
#include <linux/socket.h>
#include <linux/slab.h>



MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Purvi");

extern void *hello_hypervisor2;

struct data_to_print{
    char name[6]; // hello
    int num_call;
};
// NOTE: the arguments are the HVA of the structs and we need to read formthose structs
// so we have created the structs also (1st, 2nd  line) in the physical machine
void print_data(struct kvm_vcpu *vcpu, unsigned long number_passed, unsigned long struct_addr_data_to_print)
{
    struct data_to_print struct_data;
    unsigned long data_addr, addr;
	gfn_t gfn;
	int offset;
	long int kvm_return;
    // direct number passing
    printk(KERN_INFO "Purvi: Number passed is %ld\n", number_passed);

	// address traslation as the physical addreess that is passed is recognizable inside
	// the VM not inside the PM. gfn is the guest frame no of that virtual machine that is passed thru the vcpu
    // Given guest physical address convert it to host virtual address
    gfn = struct_addr_data_to_print >> PAGE_SHIFT;
    offset = offset_in_page(struct_addr_data_to_print);  // at this gfn where is data lying
    addr = gfn_to_hva(vcpu->kvm, gfn); // convert the gfn to the hva(host virtual address) 
    kvm_return=kvm_is_error_hva(addr);
    if (kvm_is_error_hva(addr)){
        printk(KERN_INFO "Purvi: Wrong struct Address, kvm return value = %ld\n", kvm_return);
        return;
    }

    data_addr = offset + addr;  // actual adress form where we can get the data - actual virtual adress in the physical machine
    // for the physical mahince the data that we are accessing for the vm lies in user space and not in kernel space
    // therefore have to copy that data in the kernel space, else it will generate security related threats/errors
    if(copy_from_user(&struct_data, (void __user*)data_addr, sizeof(struct_data))){
        printk(KERN_INFO "Copy from read guest for struct address is not success\n");
    }
    else{
        printk(KERN_INFO"Purvi: Hello %s\n",struct_data.name); 
        printk(KERN_INFO"Purvi: This is the %d call to print\n",struct_data.num_call);
    }
    return;
}

static int __init hypercall_init(void)
{
	pr_info("function pointers updated\n");
	hello_hypervisor2 = print_data;
	return 0;
}
static void __exit hypercall_exit(void)
{
	hello_hypervisor2 = NULL;
	pr_info("function pointers removed\n");
}

module_init(hypercall_init);
module_exit(hypercall_exit);
