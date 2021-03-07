#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/slab.h>

struct birthday {
	int day;
	int month;
	int year;
	struct list_head list;
};

/**
 * The following defines and initializes a list_head object named files_list
 */

static LIST_HEAD(birthday_list);

struct birthday* create_birthday(int day, int month, int year){
	struct birthday* b;
	b = (struct birthday*)kmalloc(sizeof(b), GFP_KERNEL);
	b->day = day;
	b->month = month;
	b->year = year;
	INIT_LIST_HEAD(&b->list);
	return b;
}

int fileinfo304_init(void)
{
	printk(KERN_INFO "Loading Module-assignment2 from book\n");
	{
		int i;
		for(i=0; i < 5; i++)
			list_add_tail(&create_birthday(1+i, 1+i, 1990+i)->list, &birthday_list);

		{
			struct birthday * ptr, * nxt;
			list_for_each_entry_safe(ptr, nxt, &birthday_list, list)
				printk(KERN_INFO "birtday : %d/%d/%d\n", ptr->day, ptr->month, ptr->year);
		}
		printk(KERN_INFO "appended %d birtdays to the list\n", i);
	}
	return 0;
}

void fileinfo304_exit(void) {

	printk(KERN_INFO "Removing Module-assigment2 from book\n");

	{
		int i = 0;
		struct birthday *ptr, *nxt;
		list_for_each_entry_safe(ptr, nxt, &birthday_list, list){
			list_del(&ptr->list);
			kfree(ptr);
			i++;
		}
		printk(KERN_INFO "removed %d birtdays from the list\n", i);
	}
}

module_init( fileinfo304_init);
module_exit( fileinfo304_exit);

MODULE_LICENSE( "GPL");
MODULE_DESCRIPTION( "Exercise for COMP304");
MODULE_AUTHOR("Your Name");

