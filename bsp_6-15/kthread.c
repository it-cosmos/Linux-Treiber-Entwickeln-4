#include <linux/module.h>
#include <linux/kthread.h>

static struct task_struct *thread_id;
static wait_queue_head_t wq;
static DECLARE_COMPLETION( on_exit );

static int thread_code( void *data)
{
  unsigned long timeout;
  int i;

  allow_signal( SIGTERM );
  for (i = 0; i<5 && (kthread_should_stop()==0);i++) {
    timeout=HZ; /* wait 1 secod */
    timeout=wait_event_interruptible_timeout(wq, (timeout=0), timeout);
    printk("thread_function: woke up ... %ld\n", timeout);
    if ( timeout == -ERESTARTSYS ) {
      printk("got signal, break\n");
      break;
    }
  }
  complete_and_exit( &on_exit, 0);
}

static int __init kthread_init(void)
{
  init_waitqueue_head(&wq);
  thread_id=kthread_create(thread_code, NULL, "mykthread");
  if (thread_id == 0)
    return -EIO;
  wake_up_process( thread_id);
  return 0;
}

static void __exit kthread_exit(void)
{
  kill_pid(task_pid(thread_id), SIGTERM, 1);
  wait_for_completion( &on_exit);
}

module_init( kthread_init);
module_exit( kthread_exit);

MODULE_LICENSE("GPL");
