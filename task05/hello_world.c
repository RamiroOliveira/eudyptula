#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

/*
 * Version information
 */
#define DRIVER_AUTHOR "Ramiro Oliveira <roliveir@synopsys.com>"
#define DRIVER_DESC "Hello World module"
#define DRIVER_LICENSE "GPL"

MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);

static void __exit hello_world_exit(void)
{
	pr_info("Bye!\n");
}

static int __init hello_world_init(void)
{
	pr_info("Hello, USB!");
	return 0;
}

static struct usb_device_id usb_kbd_id_table[] = {
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
			USB_INTERFACE_SUBCLASS_BOOT,
			USB_INTERFACE_PROTOCOL_KEYBOARD) },
	{}
};

MODULE_DEVICE_TABLE(usb, usb_kbd_id_table);

module_init(hello_world_init);
module_exit(hello_world_exit);
