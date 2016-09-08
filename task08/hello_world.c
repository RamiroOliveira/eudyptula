#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("roliveir");
MODULE_DESCRIPTION("Hello World");

#define EUDYPTULA_ID_SIZE 13

#define EUDYPTULA_ID "2a86d024c0e5"

struct dentry *debugfs;
static DEFINE_SEMAPHORE(foo_semaphore);
static char foo_data[PAGE_SIZE];

static ssize_t hello_read(struct file *file, char *buf,
				size_t count, loff_t *ppos)
{
	return simple_read_from_buffer(buf, count, ppos, EUDYPTULA_ID,
				strlen(EUDYPTULA_ID));
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
	.read		= hello_read,
	.write		= hello_write,
};

static ssize_t foo_read(struct file *file, char *buf,
				size_t count, loff_t *ppos)
{
	int result = 0;

	down(&foo_semaphore);

	result = simple_read_from_buffer(buf, count, ppos, foo_data,
				strlen(foo_data));

	up(&foo_semaphore);

	return result;
}

static ssize_t foo_write(struct file *file, const char __user *buf,
				size_t count, loff_t *ppos)
{
	int result = 0;

	down(&foo_semaphore);

	if (count <= PAGE_SIZE) {
		result = simple_write_to_buffer(foo_data, PAGE_SIZE,
						ppos, buf, count);
	} else
		result = -EINVAL;

	up(&foo_semaphore);

	return result;
}

static const struct file_operations foo_fops = {
	.owner		= THIS_MODULE,
	.read		= foo_read,
	.write		= foo_write,
};

static int __init hello_world(void)
{
	struct dentry *hello;
	struct dentry *jiff;
	struct dentry *foo;

	debugfs = debugfs_create_dir("eudyptula", NULL);
	if (!debugfs)
		return -ENOMEM;

	hello = debugfs_create_file("id", 0666, debugfs, NULL, &hello_fops);
	if (!hello) {
		debugfs_remove_recursive(debugfs);
		debugfs = NULL;
		return -ENOMEM;
	}

	jiff = debugfs_create_u64("jiffies", 0444, debugfs, (u64 *)&jiffies);
	if (!jiff) {
		debugfs_remove_recursive(debugfs);
		debugfs = NULL;
		return -ENOMEM;
	}

	foo = debugfs_create_file("foo", 0644, debugfs, NULL, &foo_fops);
	if (!foo) {
		debugfs_remove_recursive(debugfs);
		debugfs = NULL;
		return -ENOMEM;
	}

	return 0;
}

static void __exit bye_world(void)
{
	debugfs_remove_recursive(debugfs);
}

module_init(hello_world);
module_exit(bye_world);
