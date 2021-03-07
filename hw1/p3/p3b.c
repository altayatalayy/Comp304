#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched/signal.h>
#include <linux/sched.h>
#include <linux/list.h>

static int pid = 0;
module_param(pid, int, 0644);
MODULE_PARM_DESC(pid, "PID of the process");

int mod_init(void) {
	printk(KERN_INFO "%s","LOADING MODULE\n");

	if(pid == 0){
		printk(KERN_ERR "%s", "No pid argument given!\n");
	}else {
		struct task_struct *task;

		task = get_pid_task(find_get_pid(pid), PIDTYPE_PID);
		if(task == NULL)
			printk(KERN_ERR "%s", "Invalid pid!\n");
		else{
			printk(KERN_INFO "PID: %d PARENT PID: %d PROCESS NAME: %s\n", task->pid, task->parent->pid, task->comm);
			{
				struct task_struct *sib, *next;
				//struct list_head *list;
				printk(KERN_INFO "Siblings:\n");
				list_for_each_entry_safe(sib, next, &task->sibling, sibling){
					if(sib->pid != 0)
						printk(KERN_INFO "PID: %d PROCESS NAME: %s\n", sib->pid, sib->comm);
				}
			}
		}
	}

	return 0;
}

void mod_exit(void) {
	printk(KERN_INFO "%s","REMOVING MODULE\n");
}

module_init(mod_init);    /*    Load Module MACRO    */
module_exit(mod_exit);    /*    Remove Module MACRO    */

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("");
MODULE_AUTHOR("");
