#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>

struct kmem_cache *mem_cache;

struct identity {
	char name[20];
	int id;
	bool busy;
	struct list_head list;
};

static LIST_HEAD(identity_list);

int identity_create(char *name, int id)
{
	struct identity *temp;

	temp = kmem_cache_alloc(mem_cache, GFP_KERNEL);
	if (!temp)
		return -ENOMEM;

	strncpy(temp->name, name, 20);
	temp->id = id;
	temp->busy = false;

	INIT_LIST_HEAD(&temp->list);

	list_add_tail(&temp->list, &identity_list);
	return 0;
}

struct identity *identity_find(int id)
{
	struct identity *temp;

	list_for_each_entry(temp, &identity_list, list) {
		if (temp->id == id)
			return temp;
	}
	return NULL;
}

void identity_destroy(int id)
{
	struct identity *temp, *next;

	list_for_each_entry_safe(temp, next, &identity_list, list) {
		if (temp->id == id) {
			list_del(&temp->list);
			kmem_cache_free(mem_cache, temp);
		}
	}
}

static int __init hello_init(void)
{
	struct identity *temp;

	mem_cache = kmem_cache_create("eudyptula_cache",
			sizeof(struct identity), 0, 0, NULL);
	if (!mem_cache)
		return -ENOMEM;

	identity_create("Alice", 1);
	identity_create("Bob", 2);
	identity_create("Dave", 3);
	identity_create("Gena", 10);

	temp = identity_find(3);
	if (temp == NULL)
		pr_debug("id 3 not found\n");
	else
		pr_debug("id 3 = %s\n", temp->name);

	temp = identity_find(42);
	if (temp == NULL)
		pr_debug("id 42 not found\n");
	else
		pr_debug("id 42 = %s\n", temp->name);

	identity_destroy(2);
	identity_destroy(1);
	identity_destroy(10);
	identity_destroy(42);
	identity_destroy(3);

	return 0;
}

static void __exit hello_exit(void)
{
	kmem_cache_destroy(mem_cache);
	return;
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
