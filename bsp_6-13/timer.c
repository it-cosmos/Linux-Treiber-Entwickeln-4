#include <linux/module.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

struct hrtimer hrt_monotonic, hrt_realtime;

static enum hrtimer_restart timer_function( struct hrtimer *hrt )
{
  printk("%ld: timer_function( %p )\n", jiffies, hrt );
  return HRTIMER_NORESTART;
}

static int __init mod_init(void)
{
  struct timespec tp;
  ktime_t tim, absolut;

  printk("mod_init called\n");
  hrtimer_get_res( CLOCK_MONOTONIC, &tp);
  printk("monotonic(%p): %ld, %ld\n",
	 &hrt_monotonic, tp.tv_sec, tp.tv_nsec);
  hrtimer_get_res( CLOCK_REALTIME, &tp);
  printk("realtime(%p): %ld, %ld\n",
	 &hrt_realtime, tp.tv_sec, tp.tv_nsec);

  hrtimer_init( &hrt_monotonic, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
  hrtimer_init( &hrt_realtime, CLOCK_REALTIME, HRTIMER_MODE_ABS);
  hrt_monotonic.function = timer_function;
  hrt_realtime.function = timer_function;
  tim = ktime_set( 15, 0);
  hrtimer_start( &hrt_monotonic, tim, HRTIMER_MODE_REL);
  ktime_get_real_ts( &tp);
  absolut = timespec_to_ktime(tp);
  tim = ktime_add( tim, absolut);
  hrtimer_start( &hrt_realtime, tim, HRTIMER_MODE_ABS);
  printk("Timer activated at %ld jiffies\n", jiffies);
  return 0;
}

static void __exit mod_exit(void)
{
  hrtimer_cancel( &hrt_monotonic);
  hrtimer_cancel( &hrt_realtime);
  printk("mod_exit called\n");
}

module_init(mod_init);
module_exit(mod_exit);
MODULE_LICENSE("GPL");
