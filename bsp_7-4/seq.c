#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/seq_file.h>

static struct proc_dir_entry *proc_file;

static void *iterator_start(struct seq_file *m, loff_t *index)
{
  int i;
  struct task_struct *tptr = current;

  for (i=(int)*index; i && tptr!=tptr->parent; i--)
    tptr = tptr->parent;
  if (tptr==tptr->parent)
    return NULL;
  return (void *)tptr;
}

static void iterator_stop(struct seq_file *m, void *obj_ident)
{
  return;
}

static void *iterator_next(struct seq_file *m, void *obj_ident, loff_t *index)
{
  struct task_struct *tptr = (struct task_struct *)obj_ident;

  if (tptr == tptr->parent) {
    return NULL;
  }
  (*index)++;
  return (void *) tptr->parent;
}

static int sf_show(struct seq_file *m, void *obj_ident)
{
  struct task_struct *tptr = (struct task_struct *)obj_ident;

  seq_printf(m, "Prozess PID: %d\n", tptr->pid);
  seq_printf(m, " Eltern: %p\n", tptr->parent );
  return 0;
}


static struct seq_operations sops = {
  .start = iterator_start,
  .next = iterator_next,
  .stop = iterator_stop,
  .show = sf_show,
};

static int proc_open( struct inode *geraete_datei, struct file *instanz )
{
  return seq_open( instanz, &sops );
}

static struct file_operations fops = {
  .owner = THIS_MODULE,
  .open = proc_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = seq_release,
};

static int __init mod_init(void)
{
  proc_file=proc_create_data( "SequenceFileTest", S_IRUGO, NULL,
			      &fops, NULL );
  if (!proc_file)
    return -ENOMEM;
  return 0;
}

static void __exit mod_exit(void)
{
  if (proc_file) remove_proc_entry( "SwquenceFileTest", NULL );
}

module_init( mod_init );
module_exit( mod_exit );
MODULE_LICENSE("GPL");
