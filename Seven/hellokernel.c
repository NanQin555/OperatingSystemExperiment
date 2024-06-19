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

static int major_number;
static struct cdev char_dev;
static struct class *char_class;
static struct device *char_device;

// 驱动操作函数
static int char_dev_open(struct inode *, struct file *);
static int char_dev_release(struct inode *, struct file *);
static ssize_t char_dev_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t char_dev_write(struct file *, const char __user *, size_t, loff_t *);
static long char_dev_ioctl(struct file *, unsigned int, unsigned long);

static struct file_operations fops = {
    .owner = THIS_MODULE,
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
        return -1;
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

    return 0;
}

static void __exit hello_exit(void) {
    device_destroy(char_class, MKDEV(major_number, 0));
    class_unregister(char_class);
    class_destroy(char_class);
    cdev_del(&char_dev);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Char device unregistered\n");
}

module_init(hello_init);
module_exit(hello_exit);

// 实现驱动操作函数
static int char_dev_open(struct inode *inode, struct file *file) {
    // 打开设备时执行的代码
    printk(KERN_INFO "Char device opened\n");
    return 0;
}

static int char_dev_release(struct inode *inode, struct file *file) {
    // 设备关闭时执行的代码
    printk(KERN_INFO "Char device released\n");
    return 0;
}

static ssize_t char_dev_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    // 从设备读取数据时执行的代码
    printk(KERN_INFO "Reading from char device\n");
    // 返回读取的字节数
    return len;
}

static ssize_t char_dev_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    // 向设备写入数据时执行的代码
    printk(KERN_INFO "Writing to char device\n");
    // 返回写入的字节数
    return len;
}

static long char_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    // 执行Ioctl操作时的代码
    printk(KERN_INFO "IOCTL operation on char device\n");
    return 0;
}