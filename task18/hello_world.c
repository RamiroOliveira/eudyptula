#include <linux/delay.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ramiro Oliveira <roliveir@synopsys.com>");
MODULE_DESCRIPTION("Hello World");

#define MAX_SIZE 19

static DECLARE_WAIT_QUEUE_HEAD(wee_wait);
static struct task_struct *eudyptula;
static int id_cnt;
struct mutex slock;

struct identity {
	char name[20];
	int id;
	bool busy;
	struct list_head list;
};

static LIST_HEAD(identity_list);

static int identity_create(char *name, int id)
{
	struct identity *temp;

	temp = kmalloc(sizeof(*temp), GFP_KERNEL);
	if (!temp)
		return -ENOMEM;

	strncpy(temp->name, name, 20);
	temp->id = id;
	temp->busy = false;

	INIT_LIST_HEAD(&temp->list);

	list_add_tail(&temp->list, &identity_list);

	return 0;
}

static void identity_destroy(int id)
{
	struct identity *temp, *next;

	list_for_each_entry_safe(temp, next, &identity_list, list) {
		if (temp->id == id) {
			list_del(&temp->list);
			kfree(temp);
		}
	}
}

static struct identity *identity_get(void)
{
	struct identity *temp, *next;

	list_for_each_entry_safe(temp, next, &identity_list, list) {

		mutex_lock(&slock);
		id_cnt--;
		mutex_unlock(&slock);

		list_del(&temp->list);
		return temp;
	}
	return NULL;
}

static int eudyptula_thread(void *data)
{
	struct identity *temp;

	while (!kthread_should_stop()) {
		wait_event_interruptible(wee_wait, !kthread_should_stop());

		set_current_state(TASK_RUNNING);

		temp = identity_get();

		set_current_state(TASK_INTERRUPTIBLE);

		schedule_timeout(msecs_to_jiffies(5000));

		if (temp)
			pr_debug("%s %d\n", temp->name, temp->id);

		kfree(temp);
	}
	return 0;
}

static ssize_t hello_write(struct file *file, const char __user *buf,
				size_t count, loff_t *ppos)
{
	char tmp[PAGE_SIZE];
	int result = 0;

	result = simple_write_to_buffer(tmp, PAGE_SIZE - 1, ppos, buf, count);

	if (result > MAX_SIZE + 1)
		tmp[MAX_SIZE] = '\0';
	else
		tmp[count - 1] = '\0';

	identity_create(tmp, id_cnt);

	mutex_lock(&slock);
	id_cnt++;
	mutex_unlock(&slock);

	wake_up_interruptible(&wee_wait);

	return result;
}


static const struct file_operations hello_fops = {
	.owner		= THIS_MODULE,
	.write		= hello_write,
};

static struct miscdevice hello_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "eudyptula",
	.fops = &hello_fops,
	.mode = 0222,
};

static int __init hello_world(void)
{
	int ret;

	eudyptula = kthread_create(eudyptula_thread, NULL, "eudyptula");
	if (IS_ERR(eudyptula)) {
		pr_err("Unable to start kernel thread.\n");
		ret = PTR_ERR(eudyptula);
		eudyptula = NULL;
		return ret;
	}

	ret = misc_register(&hello_dev);
	if (ret) {
		pr_err("Unable to register \"Hello, world!\" misc device\n");
		goto out;
	}

	wake_up_process(eudyptula);

	mutex_init(&slock);
	return 0;
out:
	if (eudyptula) {
		kthread_stop(eudyptula);
		eudyptula = NULL;
	}
	return ret;
}

static void __exit bye_world(void)
{
	int i;
	int tmp_id = id_cnt;

	if (eudyptula) {
		kthread_stop(eudyptula);
		eudyptula = NULL;
	}

	for (i = 0; i < tmp_id; i++) {
		identity_destroy(i);
		id_cnt--;
	}

	misc_deregister(&hello_dev);
}

module_init(hello_world);
module_exit(bye_world);
