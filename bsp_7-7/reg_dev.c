#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>

static dev_t my_dev_number;
static struct cdev *driver_object;
static struct class *my_class;
static DECLARE_COMPLETION( dev_obj_is_free );

static struct device *my_device;

static struct device_driver my_driver = {
  .name = "my_driver",
  .bus = &platform_bus_type,
};

static struct file_operations fops = {
  .owner= THIS_MODULE,
};

static void my_release( struct device *dev )
{
  pr_info("my_release()\n");
  complete( &dev_obj_is_free );
}

static int __init mod_init(void)
{
  if (driver_register(&my_driver)!=0) {
    pr_err("my: driver_register failed\n");
    return -EIO;
  }
  if (alloc_chrdev_region(&my_dev_number,0,1,"my")<0)
    return -EIO;
  driver_object = cdev_alloc();
  if (driver_object==NULL)
    goto free_device_number;
  driver_object->owner = THIS_MODULE;
  driver_object->ops = &fops;
  if (cdev_add(driver_object,my_dev_number,1))
    goto free_cdev;
  my_class = class_create( THIS_MODULE, "my" );
  if (IS_ERR(my_class)) {
    printk("my: no udev support.\n");
    goto free_cdev;
  }
  my_device = device_create( my_class, NULL, my_dev_number+0,
			    NULL, "my%id",0);
  my_device->release = my_release;
  my_device->driver = &my_driver;
  mutex_lock( &my_device->mutex );
  if (device_bind_driver( my_device )) {
    mutex_unlock( &my_device->mutex );
    pr_err("device_bind_driver() failed\n");
    device_destroy( my_class, my_dev_number+0 );
    class_destroy( my_class );
    goto free_cdev;
  }
  mutex_unlock( &my_device->mutex );
  return 0;
 free_cdev:
  kobject_put( &driver_object->kobj );
 free_device_number:
  unregister_chrdev_region( my_dev_number, 1 );
  return -EIO;
}

static void __exit udev_driver_exit(void)
{
  device_release_driver( my_device );
  device_destroy( my_class, my_dev_number+0 );
  class_destroy( my_class );
  cdev_del( driver_object );
  unregister_chrdev_region( my_dev_number, 1 );
  driver_unregister( &my_driver );
  wait_for_completion( &dev_obj_is_free );
}

module_init( mod_init );
module_exit( udev_driver_exit );
MODULE_LICENSE("GPL");

      
  
	  
