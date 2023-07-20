/****************************************************************************************
 * Include Library
 ****************************************************************************************/
#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/i2c.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>

/* Metadata */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("TuanVA");
MODULE_DESCRIPTION("Device driver for comunicate with i2c eeprom at24c32");
MODULE_VERSION("1.0");

/* Global struct for stored i2c client *********************************************/
static struct i2c_client *myat_client;

/* Declare for matching device tree ************************************************/
static struct of_device_id my_driver_ids[] = {
	{
		.compatible = "myeeprom,at24c32ic",
	}, { /* sentinel */ }
};

static struct i2c_device_id at24_idtable[] = {
    { "myat24c32drv", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, at24_idtable);

/* AT24c32 Internal command ********************************************************/
static void eeprom_write(uint16_t addr, uint8_t *dat, uint32_t siz)
{
    uint8_t *pdata;
    struct i2c_msg msgs[2];

    pdata = kmalloc(siz + 2, GFP_KERNEL);

    pdata[0] = (uint8_t)(addr >> 8);
    pdata[1] = (uint8_t)(addr >> 0);
    memcpy(&pdata[2], dat, siz);

    msgs[0].addr = myat_client->addr;
    msgs[0].len = siz + 2;
    msgs[0].buf = pdata; // pre-filled
    msgs[0].flags = 0; // Write transaction by default

    msgs[1].addr = myat_client->addr;
    msgs[1].len = 0;
    msgs[1].buf = pdata;
    msgs[1].flags = I2C_M_STOP; // Read transaction

    i2c_transfer(myat_client->adapter, msgs, ARRAY_SIZE(msgs));
    kfree(pdata);

}

static void eeprom_read(uint16_t addr, uint8_t *dat, uint32_t siz)
{
    uint8_t bufaddr[2];
    struct i2c_msg msgs[2];

    bufaddr[0] = (uint8_t)(addr >> 8);
    bufaddr[1] = (uint8_t)(addr >> 0);

    msgs[0].addr = myat_client->addr;
    msgs[0].len = sizeof(bufaddr);
    msgs[0].buf = bufaddr; // pre-filled
    msgs[0].flags = 0; // Write transaction by default

    msgs[1].addr = myat_client->addr;
    msgs[1].len = siz;
    msgs[1].buf = dat;
    msgs[1].flags = I2C_M_RD; // Read transaction

    i2c_transfer(myat_client->adapter, msgs, ARRAY_SIZE(msgs));

}

/* CHARACTER DEVICE ****************************************************************/
int misc_open(struct inode *node, struct file *filep)
{
    pr_info("%s, %d\n", __func__, __LINE__);

    return 0;
}


int misc_release(struct inode *node, struct file *filep)
{
    pr_info("%s, %d\n", __func__, __LINE__);

    return 0;
}

static ssize_t misc_read(struct file *filp, char __user *buf, size_t count,
                         loff_t *f_pos)
{
    int max = 0;

    pr_info("%s, %d\n", __func__, __LINE__);

    return max;
}

// Ham write: ghi du lieu vao vung storage
static ssize_t misc_write(struct file *filp, const char __user *buf,
                          size_t count, loff_t *f_pos)
{
    int n = 0;

    return n;
}

/// @brief Handler when have write acction on devide file.
/// @param filep
/// @param cmd
/// @param arg
/// @return
static long misc_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{

    return 0;
}

/* declear for character device */
struct file_operations misc_fops = {
    .owner = THIS_MODULE,
    .open = misc_open,
    .release = misc_release,
    .read = misc_read,
    .write = misc_write,
    .unlocked_ioctl = misc_ioctl,
};

static struct miscdevice misc_example = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "misc_at24c32",
    .fops = &misc_fops,
};


static int at24_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    char bufin[10] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09};
    char buf[10];

    memset(buf, 0, 10);
    printk("TuanVA - Probe Function With client id: 0x%x!\n", client->addr);

    myat_client = client;

    // i2c_master_send(myat_client, buf, 10);

    // i2c_master_send(myat_client, buf, 2);

    // i2c_master_recv(myat_client, buf, 2);

    eeprom_write(0, bufin, 10);

    eeprom_read(0, buf, 10);
    printk("TuanVA - DATA %x %x %x %x %x %x %x %x %x %x \n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9]);

    memset(buf, 0, 10);
    eeprom_read(1, buf, 10);
    printk("TuanVA - DATA %x %x %x %x %x %x %x %x %x %x \n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9]);

    memset(buf, 0, 10);
    eeprom_read(2, buf, 10);
    printk("TuanVA - DATA %x %x %x %x %x %x %x %x %x %x \n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9]);

    memset(buf, 0, 10);
    eeprom_read(6, buf, 10);
    printk("TuanVA - DATA %x %x %x %x %x %x %x %x %x %x \n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9]);

    misc_register(&misc_example);

    return 0;
}

static int at24_remove(struct i2c_client *client)
{
    printk("TuanVA - Remove Function!\n\n");

	misc_deregister(&misc_example);

    return 0;
}

static struct i2c_driver at24_driver = {
	.driver = {
		.name	= "myat24c32drv",
        .of_match_table = my_driver_ids,
	},

	.id_table	= at24_idtable,
	.probe		= at24_probe,
	.remove		= at24_remove,
};

module_i2c_driver(at24_driver);


