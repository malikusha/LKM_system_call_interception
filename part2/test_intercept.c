#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
//#include <asm-generic/uaccess.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/list.h>
//#include <stdlib.h>

unsigned long **sys_call_table;

//save the old version of syscall2
asmlinkage long (*ref_sys_cs3013_syscall2)(void);
//ancestry struct to be referenced lated
typedef struct ancestry {
  pid_t ancestors[10];
  pid_t siblings[100];
  pid_t children[100];
} ancestry;
//makes task_struct easier to use
typedef struct task_struct task_struct;
//a recursion function to pick up the ancestors
void recurse_ancestors(task_struct* parent, pid_t* ances_ptr){
  pid_t tmp_pid = parent->pid; //read the pid of the current ancestor
  *ances_ptr++ = tmp_pid; //add it to the array
  printk(KERN_INFO "A parent: %d!\n", tmp_pid);
  if (tmp_pid == 1 || tmp_pid == 0){ //stop if weve hit init
    return;
  }
  parent = parent->parent;
  recurse_ancestors(parent, ances_ptr);
}

asmlinkage long new_sys_cs3013_syscall2(unsigned short *target_pid, struct ancestry *response) {

  //we have to define a bunch of values b/c C90
  unsigned short copied_pid;
  ancestry new_vals;
  ancestry* new_vals_ptr = &new_vals;
  task_struct* test;
  //task_struct* tsk;
  pid_t* sibl_ptr;
  pid_t* chldrn_ptr;
  pid_t tmp_pid;
  pid_t* ances_ptr;
  struct list_head sibl;
  struct list_head chldrn;
  task_struct *p;
  task_struct* parent;

  //printk("We made it to the first part!\n");
  //make sure the pid pointer is valid
  if (copy_from_user(&copied_pid, target_pid, sizeof(unsigned short))){
    return EFAULT;
  }

  //make sure the ancestry code is valid
  if (copy_from_user(new_vals_ptr, response, sizeof(ancestry))){
    return EFAULT;
  }

  //define pointers to elements of the ancestor struct
  ances_ptr = new_vals_ptr->ancestors;
  sibl_ptr = new_vals_ptr->siblings;
  chldrn_ptr = new_vals_ptr->children;

  //get the task struct from the current pid
  test = pid_task(find_vpid(copied_pid), PIDTYPE_PID);
  sibl = test->parent->children; //reference yourself from your parent to get siblings
  chldrn = test->children; //the children list head

 printk(KERN_INFO "This is my PID! %d\n", test->pid);

 //list all the children and store the pids
  list_for_each_entry (p, &(test->children), sibling){
    tmp_pid = p->pid;
    *chldrn_ptr++ = tmp_pid;
    printk(KERN_INFO "A child: %d!\n", tmp_pid);

  }

  //list all the siblings and store the pids
  list_for_each_entry (p, &(test->sibling), sibling){
    tmp_pid = p->pid;
    *sibl_ptr++ = tmp_pid;
    printk(KERN_INFO "A sibling: %d!\n", tmp_pid);
  }

  //if we are init, dont look for parents
  if (test->pid != 1){
    parent = test->parent;
    //recurse through until we hit init
    recurse_ancestors(parent, ances_ptr);
  }

  //copy over and test that the ancestry pointer is valid
  if (copy_to_user(response, new_vals_ptr, sizeof(ancestry))){
    return EFAULT;
  }

  return 0;
}

static unsigned long **find_sys_call_table(void) {
  unsigned long int offset = PAGE_OFFSET;
  unsigned long **sct;

  while (offset < ULLONG_MAX) {
    sct = (unsigned long **)offset;

    if (sct[__NR_close] == (unsigned long *) sys_close) {
      printk(KERN_INFO "Interceptor: Found syscall table at address: 0x%02lX",
      (unsigned long) sct);
      return sct;
    }

    offset += sizeof(void *);
  }

  return NULL;
}

static void disable_page_protection(void) {
  /*
  Control Register 0 (cr0) governs how the CPU operates.

  Bit #16, if set, prevents the CPU from writing to memory marked as
  read only. Well, our system call table meets that description.
  But, we can simply turn off this bit in cr0 to allow us to make
  changes. We read in the current value of the register (32 or 64
  bits wide), and AND that with a value where all bits are 0 except
  the 16th bit (using a negation operation), causing the write_cr0
  value to have the 16th bit cleared (with all other bits staying
  the same. We will thus be able to write to the protected memory.

  It's good to be the kernel!
  */
  write_cr0 (read_cr0 () & (~ 0x10000));
}

static void enable_page_protection(void) {
  /*
  See the above description for cr0. Here, we use an OR to set the
  16th bit to re-enable write protection on the CPU.
  */
  write_cr0 (read_cr0 () | 0x10000);
}

static int __init interceptor_start(void) {
  /* Find the system call table */
  if(!(sys_call_table = find_sys_call_table())) {
    /* Well, that didn't work.
    Cancel the module loading step. */
    return -1;
  }

  /* Store a copy of all the existing functions */
  ref_sys_cs3013_syscall2 = (void *)sys_call_table[__NR_cs3013_syscall2];

  /* Replace the existing system calls */
  disable_page_protection();

  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)new_sys_cs3013_syscall2;

  enable_page_protection();

  /* And indicate the load was successful */
  printk(KERN_INFO "Loaded interceptor!");

  return 0;
}

static void __exit interceptor_end(void) {
  /* If we don't know what the syscall table is, don't bother. */
  if(!sys_call_table)
  return;

  /* Revert all system calls to what they were before we began. */
  disable_page_protection();
  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)ref_sys_cs3013_syscall2;
  enable_page_protection();

  printk(KERN_INFO "Unloaded interceptor!");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);
