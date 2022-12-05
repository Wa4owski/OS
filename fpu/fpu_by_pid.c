#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/debugfs.h>
#include <linux/printk.h> /* pr_info */
#include <linux/seq_file.h>

#include <linux/pid.h>
#include <linux/sched.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("OS. Lab2. debugfs: fpu_state");
MODULE_AUTHOR("Pavel Vakhovskiy");


static struct dentry *dir;
static struct dentry *pid_file;
static struct dentry *fpu_file;

static const int ST_SIZE = 20;
static u32 pid = -1;

static int pid_write_op(void *data, u64 value){
    pid = value;
    pr_info("fpu: input pid is %d.\n", pid);
    return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(pid_ops, NULL, pid_write_op, "%llu\n");


static void print_fpu_state(struct seq_file *m, struct task_struct *ts){
        union fpregs_state fp_state = ts->thread.fpu.state;
        seq_printf(m, "fsave registers:\n");
	seq_printf(m, " cwd %d // FPU Control Word\n", fp_state.fsave.cwd);
        seq_printf(m, " swd %d // FPU Status Word\n", fp_state.fsave.swd);
        seq_printf(m, " twd %d // FPU Tag Word\n", fp_state.fsave.twd);
        seq_printf(m, " fip %d // FPU IP Offset\n", fp_state.fsave.fip);
        seq_printf(m, " fcs %d // FPU IP Selector\n", fp_state.fsave.fcs);
        seq_printf(m, " foo %d // FPU Operand Pointer Offset\n", fp_state.fsave.foo);
	seq_printf(m, " fos %d // FPU Operand Pointer Selector\n", fp_state.fsave.fos);
        seq_printf(m, "st_space:\n");
        size_t count = 0;
        while(count++ < ST_SIZE){
            seq_printf(m, "%d\n", fp_state.fsave.st_space[count]);
        }
}
static int show(struct seq_file *m, void *v)
{
    static struct task_struct *ts;
    ts = get_pid_task(find_get_pid(pid), PIDTYPE_PID);
    if(!ts){
        pr_info("fpu_mod: task_struct with pid %d has not been founded.\n", pid);
	seq_printf(m, "task_struct with pid %d has not been founded.\n", pid);
        return 0;
    }
    print_fpu_state(m, ts);
    return 0;
}

static int open(struct inode *inode, struct  file *file)
{
    return single_open(file, show, NULL);
}

static const struct file_operations fpu_ops = {
        .llseek = seq_lseek,
        .open = open,
        .owner = THIS_MODULE,
        .read = seq_read,
        .release = single_release,
};


static int kmod_init(void) {
    pr_info("fpu_mod: module loading.\n");

    dir = debugfs_create_dir("fpu", NULL);
    pid_file = debugfs_create_file("pid_here", 0222, dir, NULL, &pid_ops);
    fpu_file = debugfs_create_file("fpu_output", 0622, dir, NULL, &fpu_ops);

    return 0;
}

static void kmod_exit(void) {
    debugfs_remove_recursive(dir);
    pr_info("fpu: module unloaded\n");
}

module_init(kmod_init);
module_exit(kmod_exit);

