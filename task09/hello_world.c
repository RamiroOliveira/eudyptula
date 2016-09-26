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

static DEFINE_SEMAPHORE(foo_semaphore);

static char foo_data[PAGE_SIZE];

static struct kobject *eudyptula;

static ssize_t id_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return sprintf(buf, "%s\n", EUDYPTULA_ID);
}

static ssize_t id_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	int ret;

	ret = strncmp(buf, EUDYPTULA_ID, strlen(EUDYPTULA_ID));

	if (ret != 0)
		return -EINVAL;

	return count;
}

static struct kobj_attribute id_attribute =
	__ATTR_RW(id);

static ssize_t jiffies_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return sprintf(buf, "%lu\n", jiffies);
}

static struct kobj_attribute jiffies_attribute =
	__ATTR_RO(jiffies);

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	int ret = 0;

	down(&foo_semaphore);

	ret = sprintf(buf, "%s", foo_data);

	up(&foo_semaphore);

	return ret;
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	down(&foo_semaphore);

	strcpy(foo_data, buf);

	up(&foo_semaphore);

	return count;
}

static struct kobj_attribute foo_attribute =
	__ATTR_RW(foo);

static struct attribute *attrs[] = {
	&id_attribute.attr,
	&jiffies_attribute.attr,
	&foo_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};


static struct attribute_group attr_group = {
	.attrs = attrs,
};

static int __init hello_world(void)
{
	int retval;

	/*
	 * Create a simple kobject with the name of "kobject_example",
	 * located under /sys/kernel/
	 *
	 * As this is a simple directory, no uevent will be sent to
	 * userspace.  That is why this function should not be used for
	 * any type of dynamic kobjects, where the name and number are
	 * not known ahead of time.
	 */
	eudyptula = kobject_create_and_add("eudyptula", kernel_kobj);
	if (!eudyptula)
		return -ENOMEM;

	/* Create the files associated with this kobject */
	retval = sysfs_create_group(eudyptula, &attr_group);
	if (retval)
		kobject_put(eudyptula);

	return retval;
}

static void __exit bye_world(void)
{
	kobject_put(eudyptula);
}

module_init(hello_world);
module_exit(bye_world);
