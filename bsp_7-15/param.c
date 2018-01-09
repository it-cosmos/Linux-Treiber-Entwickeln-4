#include <linux/fs.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

static char *para = NULL;
static unsigned char mybyte;
static short myshort;
static int intarray[4];
static int intarraycount=4;
static char string[10];
module_param( mybyte, byte, 0644 );
module_param( myshort, short, 0644 );
module_param( para, charp, 0644 );
module_param_array( intarray, int, &intarraycount, 0644 );
module_param_string( optionname, string, sizeof(string), 0644 );

static int __init mod_init(void)
{
  if (para ) {
    printk("para = %s\n", para );
  }
  pr_info("intarraycount=%d\n", intarraycount);
  for( ; intarraycount; intarraycount-- ) {
    pr_info("%d: %d\n", intarraycount, intarray[intarraycount-1] );
  }
  if ( string[0] )
    pr_info("string: %s\n", string);
  return 0;
}

static void __exit mod_exit(void)
{
}

module_init( mod_init );
module_exit( mod_exit );
MODULE_LICENSE("GPL");
