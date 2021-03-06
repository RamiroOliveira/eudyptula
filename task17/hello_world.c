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

#define EUDYPTULA_ID_SIZE 13

#define EUDYPTULA_ID "2a86d024c0e5"

static DECLARE_WAIT_QUEUE_HEAD(wee_wait);
static struct task_struct *eudyptula;

int eudyptula_thread(void *data)
{

	wait_event_interruptible(wee_wait, kthread_should_stop());

	return 0;
}

static ssize_t hello_write(struct file *file, const char __user *buf,
				size_t count, loff_t *ppos)
{
	char tmp[EUDYPTULA_ID_SIZE];
	int result = 0;

	if (count == EUDYPTULA_ID_SIZE) {
		result = simple_write_to_buffer(tmp, EUDYPTULA_ID_SIZE-1,
						ppos, buf, count) + 1;

		tmp[EUDYPTULA_ID_SIZE - 1] = '\0';
	} else
		result = -EINVAL;

	if ((*ppos) == strlen(EUDYPTULA_ID))
		result = strncmp(tmp, EUDYPTULA_ID, strlen(EUDYPTULA_ID)) ?
			-EINVAL : result;

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
	if (eudyptula) {
		kthread_stop(eudyptula);
		eudyptula = NULL;
	}
	misc_deregister(&hello_dev);
}

module_init(hello_world);
module_exit(bye_world);
