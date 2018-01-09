#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>

static DECLARE_COMPLETION( dev_obj_is_free );
static int frequenz; /*statvariale of the device */

static void mydevice_release( struct device *dv )
{
  complete( &dev_obj_is_free );
}

struct platform_device mydevice = {
  .name = "MyDevice",
  .id = 0,
  .dev = {
    .release = mydevice_release,
  }
};

static struct device_driver mydriver = {
  .name = "MyDevDrv",
  .bus = &platform_bus_type,
};

static ssize_t read_freq( struct device *dev,
			  struct device_attribute *attr, char *buf )
{
  snprintf(buf, 256, "frequency: %d\n", frequenz );
  return strlen(buf)+1;
}

static ssize_t write_freq( struct device *dev,
			   struct device_attribute *attr, const char *buf, size_t count )
{
  int result;

  result = kstrtoint( buf, 0, &frequenz );
  if (result < 0)
    return 0;
  return strlen(buf)+1;
}

static DEVICE_ATTR( freq, S_IRUGO|S_IWUSR, read_freq, write_freq );

static int __init drv_init(void)
{
  if (driver_register(&mydriver)<0)
    return -EIO;
  if (platform_device_register( &mydevice)) /* register the device */
    goto ex_driverunreg;
  mydevice.dev.driver = &mydriver; /* now tie them together */
  if (device_bind_driver( &mydevice.dev )) //*links the drvr to the dev */
    goto ex_platdevunreg;
  if (device_create_file( &mydevice.dev, &dev_attr_freq ))
    goto ex_release;
  return 0;
 ex_release:
  device_release_driver( &mydevice.dev );
 ex_platdevunreg:
  platform_device_unregister( &mydevice );
 ex_driverunreg:
  driver_unregister(&mydriver);
  return -EIO;
}

static void __exit drv_exit(void)
{
  device_remove_file( &mydevice.dev, &dev_attr_freq );
  device_release_driver( &mydevice.dev );
  platform_device_unregister( &mydevice );
  driver_unregister(&mydriver);
  wait_for_completion( &dev_obj_is_free );
}

module_init( drv_init );
module_exit( drv_exit );
MODULE_LICENSE("GPL");
