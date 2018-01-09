#include <linux/module.h>
#include <linux/init.h>

static char *textbuf = "The answer: 42";

void log_hello( char *string )
{
  if (string ) {
    printk("%s\n", string);
  } else {
    printk("lib_hello: %p is not valid\n", string);
  }
}

static int __init driver_init(void)
{
  printk("%s\n", textbuf);
  return 0;
}

static void __exit driver_exit(void)
{
}

module_init( driver_init );
module_exit( driver_exit );

EXPORT_SYMBOL_GPL( log_hello );
EXPORT_SYMBOL_GPL( textbuf );

MODULE_LICENSE("GPL");
