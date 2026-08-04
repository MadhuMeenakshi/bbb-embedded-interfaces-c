#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Madhu");
MODULE_DESCRIPTION("Interactive Calculator Driver");

#define DEVICE_NAME "calculator"
#define CLASS_NAME  "calc_class"
#define BUF_SIZE    100

static dev_t dev_num;
static struct cdev calc_cdev;
static struct class *calc_class;
static struct device *calc_device;

static char *input_buffer;
static char *output_buffer;

/*-----------------------------------------------------------*/

static int calc_open(struct inode *inode, struct file *file)
{
    pr_info("Calculator Opened\n");
    return 0;
}

/*-----------------------------------------------------------*/

static int calc_release(struct inode *inode, struct file *file)
{
    pr_info("Calculator Closed\n");
    return 0;
}

/*-----------------------------------------------------------*/

static ssize_t calc_write(struct file *file,
                          const char __user *buf,
                          size_t len,
                          loff_t *off)
{
    int num1, num2;
    char op;
    int result;

    if (len >= BUF_SIZE)
        len = BUF_SIZE - 1;

    if (copy_from_user(input_buffer, buf, len))
        return -EFAULT;

    input_buffer[len] = '\0';

    pr_info("Received : %s\n", input_buffer);

    if (sscanf(input_buffer, "%d %c %d", &num1, &op, &num2) != 3)
    {
        snprintf(output_buffer, BUF_SIZE, "Invalid Input\n");
        return len;
    }

    switch (op)
    {
        case '+':
            result = num1 + num2;
            break;

        case '-':
            result = num1 - num2;
            break;

        case '*':
            result = num1 * num2;
            break;

        case '/':

            if (num2 == 0)
            {
                snprintf(output_buffer, BUF_SIZE,
                         "Division by Zero\n");
                return len;
            }

            result = num1 / num2;
            break;

        case '%':

            if (num2 == 0)
            {
                snprintf(output_buffer, BUF_SIZE,
                         "Division by Zero\n");
                return len;
            }

            result = num1 % num2;
            break;

        default:

            snprintf(output_buffer,
                     BUF_SIZE,
                     "Invalid Operator\n");

            return len;
    }

    snprintf(output_buffer, BUF_SIZE, "%d\n", result);

    return len;
}

/*-----------------------------------------------------------*/

static ssize_t calc_read(struct file *file,
                         char __user *buf,
                         size_t len,
                         loff_t *off)
{
    int bytes;

    bytes = strlen(output_buffer);

    if (*off >= bytes)
        return 0;

    if (len > bytes - *off)
        len = bytes - *off;

    if (copy_to_user(buf, output_buffer + *off, len))
        return -EFAULT;

    *off += len;

    return len;
}

/*-----------------------------------------------------------*/

static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = calc_open,
    .read = calc_read,
    .write = calc_write,
    .release = calc_release,
};

/*-----------------------------------------------------------*/

static int __init calc_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&dev_num,
                              0,
                              1,
                              DEVICE_NAME);

    if (ret)
    {
        pr_err("Device Number Allocation Failed\n");
        return ret;
    }

    cdev_init(&calc_cdev, &fops);

    ret = cdev_add(&calc_cdev,
                   dev_num,
                   1);

    if (ret)
    {
        unregister_chrdev_region(dev_num,1);
        return ret;
    }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,4,0)
    calc_class = class_create(CLASS_NAME);
#else
    calc_class = class_create(THIS_MODULE, CLASS_NAME);
#endif

    if (IS_ERR(calc_class))
    {
        cdev_del(&calc_cdev);
        unregister_chrdev_region(dev_num,1);
        return PTR_ERR(calc_class);
    }

    calc_device = device_create(calc_class,
                                NULL,
                                dev_num,
                                NULL,
                                DEVICE_NAME);

    if (IS_ERR(calc_device))
    {
        class_destroy(calc_class);
        cdev_del(&calc_cdev);
        unregister_chrdev_region(dev_num,1);
        return PTR_ERR(calc_device);
    }

    input_buffer = kmalloc(BUF_SIZE, GFP_KERNEL);
    output_buffer = kmalloc(BUF_SIZE, GFP_KERNEL);

    if (!input_buffer || !output_buffer)
    {
        device_destroy(calc_class, dev_num);
        class_destroy(calc_class);
        cdev_del(&calc_cdev);
        unregister_chrdev_region(dev_num,1);

        kfree(input_buffer);
        kfree(output_buffer);

        return -ENOMEM;
    }

    memset(input_buffer,0,BUF_SIZE);
    memset(output_buffer,0,BUF_SIZE);

    pr_info("---------------------------------\n");
    pr_info("Calculator Driver Loaded\n");
    pr_info("Major=%d Minor=%d\n",
            MAJOR(dev_num),
            MINOR(dev_num));
    pr_info("---------------------------------\n");

    return 0;
}

/*-----------------------------------------------------------*/

static void __exit calc_exit(void)
{
    kfree(input_buffer);
    kfree(output_buffer);

    device_destroy(calc_class, dev_num);

    class_destroy(calc_class);

    cdev_del(&calc_cdev);

    unregister_chrdev_region(dev_num,1);

    pr_info("Calculator Driver Removed\n");
}

/*-----------------------------------------------------------*/

module_init(calc_init);
module_exit(calc_exit);