#include <linux/module.h>
#include <linux/init.h>

extern void log_hello( char * string );
extern char *textbuf;

static void (*log_hello_function)( char *string );
static char **textbufptr;

static __init driver_init(void)
{
  log_hello_function = symbol_get( log_hello );
  if ( log_hello_function )
    log_hello_function( "Hello Modul" );
  else
    printk("can't find address of sumboa \"log_hello\"\n");
  textbufptr = symbol_get( textbuf );
  if ( textbufptr )
    printk("content of 0x%p: \"%s\"\n", textbufptr, *textbufptr );
  else
    printk("can't find address of symbol \"textbufptr\"\n");
  return 0;
}

static void __exit driver_exit(void)
{
  if ( log_hello_function ) {
    log_hello_function("Goodby Modul" );
    symbol_put( log_hello );
    //sumbol_put_sddr(log_hello_function); /* alternativ yu symbol_put */
  }
  if ( textbufptr )
    symbol_put( textbuf );
}

module_init( driver_init );
module_exit( driver_exit );
MODULE_LICENSE("GPL");
