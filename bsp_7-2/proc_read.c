#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>

static int sf_show(struct seq_file *m, void *v)
{
  struct task_struct *tptr = current;

  seq_printf(m, "Prozess PID: %d\n", tptr->pid );
  seq_printf(m, " Eltern: %p\n", tptr->parent);
  return 0;
}

static int call_seq_open( struct inode *geraete_datai, struct file *instanz )
{
  return single_open( instanz, sf_show, NULL);
}

static struct file_operations proc_fops = {
  .owner = THIS_MODULE,
  .open = call_seq_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = single_release,
};

static int __init mod_init(void)
{
  static struct proc_dir_entry *procdirentry;

  procdirentry = proc_create_data( "myproc_file", S_IRUGO, NULL,
				   &proc_fops, NULL);
  return 0;
}

static void __exit mod_exit(void)
{
  remove_proc_entry( "my_proc_file", NULL);
}

module_init( mod_init);
module_exit( mod_exit);
MODULE_LICENSE("GPL");

