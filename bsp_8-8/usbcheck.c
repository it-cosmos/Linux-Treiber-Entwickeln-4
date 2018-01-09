#include <linux/module.h>
#include <linux/fs.h>
#include <linux/usb.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define USB_VENDOR_ID 0x05a9
#define USB_DEVICE_ID 0xa511

struct usb_device *dev;
static DEFINE_MUTEX( ulock );

static ssize_t usbcheck_read( struct file *instanz, char *buffer,
			     size_t count, loff_t *ofs )
{
  char pbuf[20];
  __u16 *status = kmalloc( sizeof(__u16), GFP_KERNEL );

  mutex_lock( &ulock ); /* Jetzt nicht disconnectent.. */
  if (usb_control_msg(dev, usb_rcvctrlpipe(dev,0), USB_REQ_GET_STATUS,
		      USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_INTERFACE,
		      0, 0, status, sizeof(*status), 5*HZ) < 0) {
    count = -EIO;
    goto read_out;
  }

  snprintf( pbuf, sizeof(pbuf), "status=%d\n", *status );
  if (strlen(pbuf) < count)
    count = strlen(pbuf);
  count -= copy_to_user(buffer,pbuf,count);
  *ofs += count;
 read_out:
  mutex_unlock( &ulock );
  kfree( status );
  return count;
}

static int usbcheck_open( struct inode *devicefile, struct file *instanz )
{
  return 0;
}

static struct file_operations usb_fops = {
  .owner = THIS_MODULE,
  .open = usbcheck_open,
  .read = usbcheck_read,
};

static struct usb_device_id usbid [] = {
  {
    USB_DEVICE(USB_VENDOR_ID, USB_DEVICE_ID), },
  { } /* Terminating entry */
};

struct usb_class_driver class_descr = {
  .name = "usbcheck",
  .fops = &usb_fops,
  .minor_base = 16,
};

static int usbcheck_probe( struct usb_interface *interface,
			  const struct usb_device_id *id)
{
  dev = interface_to_usbdev(interface);
  printk("usbcheck: 0x%4.4x|0x%4.4x, if=%p\n", dev->descriptor.idVendor,
	 dev->descriptor.idProduct, interface );
  if (dev->descriptor.idVendor==USB_VENDOR_ID
      && dev->descriptor.idProduct==USB_DEVICE_ID) {
    if (usb_register_dev( interface, &class_descr )) {
      return EIO;
    }
    printk("got minor= %d\n", interface->minor );
    return 0;
  }
  return -ENODEV;
}

static void usbcheck_disconnect( struct usb_interface *iface )
{
  /* Ausstehede Auftraege muessen abgearbietet sein... */
  mutex_lock( &ulock );
  usb_deregister_dev( iface, &class_descr );
  mutex_unlock( &ulock );
}

static struct usb_driver usbcheck = {
  .name = "usbcheck",
  .id_table = usbid,
  .probe = usbcheck_probe,
  .disconnect = usbcheck_disconnect,
};

static int __init usbcheck_init(void)
{
  if (usb_register(&usbcheck) ) {
    printk("usbcheck: unable to register usb driver\n");
    return -EIO;
  }
  return 0;
}

static void __exit usbcheck_exit(void)
{
  usb_deregister(&usbcheck);
}

module_init(usbcheck_init);
module_exit(usbcheck_exit);
MODULE_LICENSE( "GPL" );

      
      
		      
