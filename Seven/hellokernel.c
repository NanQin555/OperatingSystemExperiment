#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");
#define DEVICE_NAME "my_char_dev"
#define CLASS_NAME "my_char_class"
#define MAX 102400
#define PROCESS_INFO 1
#define OFFSET_SAVE_CUR 2
#define OFFSET_SET_SAVED 3
#define OFFSET_READ 1
#define OFFSET_WRITE 2
static int major_number;
static struct cdev char_dev;
static struct class *char_class;
static struct device *char_device;
off_t offset_read;
off_t offset_write;
off_t offset_saved;
char buffer_dev[MAX];

// 驱动操作函数
static int char_dev_open(struct inode *, struct file *);
static int char_dev_release(struct inode *, struct file *);
static ssize_t char_dev_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t char_dev_write(struct file *, const char __user *, size_t, loff_t *);
static long char_dev_ioctl(struct file *, unsigned int, unsigned long);
static void process_info(void);
static off_t save_current_offset(int);
static off_t set_saved_offset(int);
static struct file_operations fops = {
    .owner = &__this_module,
    .open = char_dev_open,
    .read = char_dev_read,
    .write = char_dev_write,
    .release = char_dev_release,
    .unlocked_ioctl = char_dev_ioctl,
};

// 设备号分配
static int __init hello_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register char device\n");
        return major_number;
    }

    printk(KERN_INFO "Char device registered with major number %d\n", major_number);

    cdev_init(&char_dev, &fops);
    if (cdev_add(&char_dev, MKDEV(major_number, 0), 1) == -1) {
        printk(KERN_ALERT "Failed to add char device\n");
        unregister_chrdev(major_number, DEVICE_NAME);
        return -EFAULT;
    }

    char_class = class_create(CLASS_NAME);
    if (IS_ERR(char_class)) {
        printk(KERN_ALERT "Failed to create device class\n");
        cdev_del(&char_dev);
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(char_class);
    }

    char_device = device_create(char_class, NULL, MKDEV(major_number, 0), NULL, "%s", DEVICE_NAME);
    if (IS_ERR(char_device)) {
        printk(KERN_ALERT "Failed to create device\n");
        class_destroy(char_class);
        cdev_del(&char_dev);
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(char_device);
    }

    void* buffer_dev = kmalloc(sizeof(buffer_dev), GFP_KERNEL);
    if(!buffer_dev) {
        printk(KERN_ALERT "Failed to allocate shared memory\n");
        return -EFAULT;
    }
    memset(buffer_dev, 0, sizeof(buffer_dev));
    return 0;
}

static void __exit hello_exit(void) {
    kfree((void*)buffer_dev);
    device_destroy(char_class, MKDEV(major_number, 0));
    // class_unregister(char_class);
    class_destroy(char_class);
    cdev_del(&char_dev);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Char device unregistered\n");
}

module_init(hello_init);
module_exit(hello_exit);

// open
static int char_dev_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Char device opened\n");
    offset_read = 0;
    offset_write = 0;
    offset_saved = 0;
    file->private_data = buffer_dev;
    return 0;
}

// release
static int char_dev_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Char device released\n");
    offset_read = 0;
    offset_write = 0;
    offset_saved = 0;
    return 0;
}

// read
static ssize_t char_dev_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "Reading from char device\n");
    if(copy_to_user(buffer, buffer_dev + offset_read, len)) {
        printk(KERN_ALERT "Read Fault");
        return -EFAULT;
    }
    printk(KERN_INFO "Read Success: %.*s", len, buffer_dev + offset_read);
    offset_read += len; 
    return len;
}

// write
static ssize_t char_dev_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "Writing to char device\n");
    if(copy_from_user(buffer_dev + offset_write, buffer, len)) {
        printk(KERN_ALERT "Write Fault");
        return -EFAULT;
    }
    printk(KERN_INFO "Write Success: %.*s", len, buffer_dev+offset_write);
    offset_write += len;
    return len;
}

static long char_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    printk(KERN_INFO "IOCTL operation on char device\n");

    if(cmd == PROCESS_INFO) {
        printk(KERN_INFO "IOCTL operation: Process_info\n");
        process_info();
    }
    else if(cmd == OFFSET_SAVE_CUR) {
        printk(KERN_INFO "IOCTL operation: Offset_save_cur\n");
        save_current_offset(arg);
    }
    else if(cmd == OFFSET_SET_SAVED) {
        printk(KERN_INFO "IOCTL operation: Offset_set_saved\n");
        set_saved_offset(arg);
    }
    else return -EFAULT;
    return 0;
}

static void process_info() {
    struct task_struct *p;
    p = &init_task;
    int i=0;
    int written = 0;
    int buffersize = sizeof(buffer_dev);
    char* begin = buffer_dev;
    for_each_process(p) {
        if(i>=MAX) break;
        printk(KERN_INFO "PID: %d, Stats: %d, Priority: %d, Parent PID: %d\n",\
        p->pid, p->stats, p->prio, p->parent->pid);
        buffersize-=written;
        begin+=written;
        written = snprintf(begin, buffersize, \
        "PID: %d, Stats: %d, Priority: %d, Parent PID: %d\n",\
        p->pid, p->stats, p->prio, p->parent->pid);
        if(written >= buffersize) {
            printk(KERN_ALERT "The buffer is small\n");
        }
    }
}
static off_t save_current_offset(int arg) {
    if(arg == OFFSET_READ)
        offset_saved = offset_read;
    else if(arg == OFFSET_WRITE)
        offset_saved = offset_write;
    else return -EFAULT;
    return offset_saved;
}
static off_t set_saved_offset(int arg) {
    if(arg == OFFSET_READ)
        offset_read = offset_saved;
    else if(arg == OFFSET_WRITE)
        offset_write = offset_saved;
    else return -EFAULT;
    return offset_saved;
}