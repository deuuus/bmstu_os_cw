#include <linux/delay.h>
#include <linux/init.h>
#include <linux/init_task.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/kthread.h>

#define PROC_FS_NAME "bmstu_coursework"
#define DELIMITER "+++: "

#define TEMP_BUF_SIZE 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sirotkina Polina");
MODULE_DESCRIPTION("Process planning monitoring");

#define REPEAT 5
#define DELAY_MS 10 * 1000

static struct proc_dir_entry *procFile;

static struct task_struct *kthread;

#define BUF_SIZE 362144
static char out_str[BUF_SIZE] = { 0 };

static int check_buf(char *str_1, char *str_2, int max_len)
{
    if ((strlen(str_1) + strlen(str_2)) >= max_len)
    {
        printk(KERN_ERR "%s check_buf failed.\n", DELIMITER);
        return -ENOMEM;
    }
    return 0;
}

static int print_info(void *arg)
{
    struct task_struct *task;

    int i;
    char cur_buf[TEMP_BUF_SIZE];

    for (i = 0; i < REPEAT; i++) 
    {
        task = &init_task;

        memset(cur_buf, 0, TEMP_BUF_SIZE);
        snprintf(cur_buf, TEMP_BUF_SIZE, "===================== ITERATION #%d =====================\n", i+1);

        check_buf(cur_buf, out_str, BUF_SIZE);
        strcat(out_str, cur_buf);

        for_each_process(task) {
            memset(cur_buf, 0, TEMP_BUF_SIZE);
            snprintf(cur_buf, TEMP_BUF_SIZE, 
                "PID: %d;\n"
                "Process comm: %s;\n"
                "Policy: %d;\n"
                "Prio: %d\n"
                "Static_prio: %d;\n"
                "Normal_prio: %d;\n"
                "Rt_prio: %d;\n"
                "Delay: %lld;\n"
                "Calls: %ld;\n"
                "Time of last run: %lld;\n"
                "Last queued: %lld;\n\n\n",
                task->pid, task->comm, 
                task->policy,
                task->prio, task->static_prio, task->normal_prio, task->rt_priority,
                task->sched_info.run_delay, 
                task->sched_info.pcount,
                task->sched_info.last_arrival, 
                task->sched_info.last_queued);

            check_buf(cur_buf, out_str, BUF_SIZE);
            strcat(out_str, cur_buf);
        }
        mdelay(DELAY_MS);
    }
    return 0;
}

static int my_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "%s my_open called.\n", DELIMITER);

    if (!try_module_get(THIS_MODULE)) 
    {
        printk(KERN_INFO "%s error while try_module_get().\n", DELIMITER);
        return -EFAULT;
    }

    return 0;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *offp)
{
    ssize_t out_str_len = strlen(out_str);

    printk(KERN_INFO "%s my_read called.\n", DELIMITER);

    if (copy_to_user(buf, out_str, strlen(out_str)))
    {
        printk(KERN_ERR "%s error while copy_to_user().\n", DELIMITER);
        return -EFAULT;
    }

    memset(out_str, 0, BUF_SIZE);

    return out_str_len;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offp)
{
    printk(KERN_INFO "%s my_write called.\n", DELIMITER);
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "%s my_release called.\n", DELIMITER);
    module_put(THIS_MODULE);
    return 0;
}

static struct proc_ops my_ops = {
    proc_read: my_read, 
    proc_write: my_write, 
    proc_open: my_open, 
    proc_release: my_release
};

static int __init md_init(void)
{
    if (!(procFile = proc_create(PROC_FS_NAME, 0666, NULL, &my_ops)))
    {
        printk(KERN_ERR "%s error while proc_create.\n", DELIMITER);

        return -EFAULT;
    }

    kthread = kthread_run(print_info, NULL, "print_info_thread");
    if (IS_ERR(kthread))
    {
        printk(KERN_ERR "%s error while kthread_run().\n", DELIMITER);

        return -EFAULT;
    }

    printk(KERN_INFO "%s module loaded.\n", DELIMITER);

    return 0;
}

static void __exit md_exit(void)
{
    remove_proc_entry(PROC_FS_NAME, NULL);
    printk(KERN_INFO "%s Module removed. Monitoring stopped.\n", DELIMITER);
}

module_init(md_init);
module_exit(md_exit);