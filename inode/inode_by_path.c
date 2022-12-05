#include <linux/string.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>

#include <linux/mount.h>
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/fs.h>
#include <linux/printk.h>


MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("OS. Lab2. debugfs: indode");
MODULE_AUTHOR("Pavel Vakhovskiy");

static char data[PAGE_SIZE]; // variable to store data
static int data_len = 0;

static struct dentry *subdir;
static struct dentry *path_file;
static struct dentry *inode_file;

//static struct inode* get_inode(char *);
static ssize_t data_read(struct file *, char *, size_t, loff_t *);
static ssize_t data_write(struct file *, const char *, size_t, loff_t *);

static struct inode* get_inode(char * path_name){
    static struct path path;
    kern_path(path_name, LOOKUP_FOLLOW, &path);
    return path.dentry->d_inode;
}

static void print_inode(struct seq_file *m, struct inode * inode){
    seq_printf(m, "Inode number: %lu\n", inode->i_ino);
    seq_printf(m, "umode: %hu\n", inode->i_mode);
    seq_printf(m, "mtime(seconds): %ld\n", inode->i_mtime.tv_sec);
    seq_printf(m, "atime(seconds): %ld\n", inode->i_atime.tv_sec);
    seq_printf(m, "ctime(seconds): %ld\n", inode->i_ctime.tv_sec);
}

static int show(struct seq_file *m, void *v)
{
    char * path_name = data;
    pr_info("inode_mod: path name is %s\n", path_name);
    struct inode* inode = get_inode(path_name);
    if(!inode){
        pr_info("inode_mod: inode was not find\n");
        seq_printf(m, "inode by path %s was not find", path_name);
        return 0;
    }
    pr_info("inode_mod: inode number is %lu\n", inode->i_ino);
    print_inode(m, inode);
    return 0;
}

static int open(struct inode *inode, struct  file *file)
{
    return single_open(file, show, NULL);
}

static const struct file_operations inode_file_ops = {
        .llseek = seq_lseek,
        .open = open,
        .owner = THIS_MODULE,
        .read = seq_read,
        .release = single_release,
};

static ssize_t data_write(struct file *f, const char *buffer,
                          size_t len, loff_t *offset)
{
    ssize_t ret;
    ret = simple_write_to_buffer(data, PAGE_SIZE, offset, buffer, len);
    if (ret < 0){
        pr_info("inode_mod: path_name writing broke.\n");
	return ret;
    }

    data_len = ret;
    return len;
}

static ssize_t data_read(struct file *f, char *buffer,
                         size_t len, loff_t *offset)
{
    return simple_read_from_buffer(buffer, len, offset, data, data_len);
}

const struct file_operations path_file_ops = {
        .owner = THIS_MODULE,
        .write = data_write,
        .read = data_read,
};

static int debugfs_init(void)
{
    subdir = debugfs_create_dir("inode", NULL);
    
    path_file = debugfs_create_file("path_here", 0222, subdir, NULL,
                                    &path_file_ops);

    inode_file = debugfs_create_file("inode_output", 0622, subdir, NULL,
                                     &inode_file_ops);

    return 0;
}

static void debugfs_exit(void)
{
    debugfs_remove_recursive(subdir);
}


module_init(debugfs_init);
module_exit(debugfs_exit);

