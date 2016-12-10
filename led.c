/*-------------------------------------------------------------------------
 * Filename:      gpio_drv.c
 * Copyright:     Copyright (C) 2006, dmatek
 * Author:        chris_zhang <www.dmatek.com>
 * Description:   gpio driver
 * Created at:     Aug 11 2006
 *-----------------------------------------------------------------------*/

#include <linux/types.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/string.h>
#include <linux/ioport.h>
#include <asm/uaccess.h>
#include <mach/hardware.h>
#include <asm/io.h>
#include <linux/miscdevice.h>
#include <asm/delay.h>
#include <mach/regs-mem.h>
#include <plat/regs-gpio.h>
#include <mach/hardware.h>
#include <mach/gpio.h>
//#include <plat/gpio-cfg.h>
#include <linux/gpio.h>

#include <linux/platform_device.h>
#include <plat/regs-gpio.h>
#include <plat/gpio-cfg.h>



#define LED_ON  1
#define LED_OFF 2
#define LED_TEST 3


#define LED_MAJOR 230
#define led_name "led"


//void s3c_gpio_cfgpin(unsigned int pin, unsigned int function);

typedef struct _LED_PARAM{
		unsigned int num;
	}LED_PARAM;

typedef struct _KEY_PARAM{
		unsigned int state;
	}KEY_PARAM;


#define MS_DELAY(s)   Delay(s<<2)

void Delay(int time)
{
#define S3C2443A_FCLK 400000000
	volatile int i, j = 0;
	volatile static int loop = S3C2443A_FCLK/100000; //4000
	
	for(;time > 0;time--)
		for(i=0;i < loop; i++) { 
			j++; 
		}
}

static void led_off(int led_num)
{

	int gpndat;
	
	gpndat = __raw_readl(S3C64XX_GPNDAT);//S3C_GPBDAT
	switch(led_num)
	{
		case 1://led1
			gpndat &=~(1<<14);//gpg9
			break;
		case 2:
			gpndat &=~(1<<15);//ok
			break;
		default:
			break;
	}
	__raw_writel(gpndat,S3C64XX_GPNDAT);

}

static void led_on(int led_num)
{

	int gpndat;
	
	gpndat = __raw_readl(S3C64XX_GPNDAT);//S3C_GPBDAT
	switch(led_num)
	{
		case 1://led1
			gpndat |= (1<<14);//gpg9
			break;
		case 2:
			gpndat |= (1<<15);//ok
			break;
		default:
			break;
	}
	__raw_writel(gpndat,S3C64XX_GPNDAT);
}

void led_test(void)
{
	int i ;
	
	for(i=1;i<5;i++)
	{
		led_off(i+1);
		//udelay(1000);
		MS_DELAY(2) ;
		led_on(i);
		//udelay(1000);
		MS_DELAY(2) ;
		led_on(i+1);
		//udelay(1000);
		MS_DELAY(2) ;
		led_off(i);
		//udelay(1000);
		MS_DELAY(2) ;
	}
}

static ssize_t led_read(struct file *filp,char *buf,size_t count,loff_t *l)
{
	return count;
}

static ssize_t led_write(struct file *filp,const char *buf,size_t count,loff_t *f_ops)
{
	return count;
}

static int led_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	int num;

	switch(cmd)
	{
	
	/*-------------------*/
	/* 		led test	 	  */
	/*-------------------*/
	case LED_ON:
		ret = copy_from_user(&num, (int *)arg, sizeof(int));//arg = 3-->num = 3
		if (ret != 0)
		{
			printk("gpio_ioctl: copy_from_user failed\n");
			return(-EFAULT);
		}
		printk("num = %d\n",num);
		led_on(num);
		break;
	case LED_OFF:
		ret = copy_from_user(&num, (int*)arg, sizeof(int));
		if (ret != 0)
		{
			printk("gpio_ioctl: copy_from_user failed\n");
			return(-EFAULT);
		}
		printk("num = %d\n",num);
		led_off(num);
		break;	
	case LED_TEST:
		led_test();
		break;		
	}
	return 0;
}

static int led_open(struct inode *inode, struct file *filp)
{	/*³õÊ¼»¯GPIOÒý½Å*/
	//s3c_gpio_cfgpin(S3C64XX_GPN(14), 1);//03
	//s3c_gpio_cfgpin(S3C64XX_GPN(15), 1);//02
	
	unsigned int val;
	val = readl(S3C64XX_GPNCON);
	val &= ~ (0xf <<28);
	val |= (1 << 28)|(1<<30);
	writel(val, S3C64XX_GPNCON);

	val = readl(S3C64XX_GPNDAT);
	val &= ~ ((1 <<14)|(1<<15));
	//val |= ~((1 <<14)|(1<<15));
	writel(val, S3C64XX_GPNDAT);
	
	return 0;
}

static int led_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static struct file_operations led_fops = 
{
	owner	:	THIS_MODULE,
	read	:	led_read,
	write	:	led_write,
	ioctl	:	led_ioctl,
	open	:	led_open,
	release	:	led_release,
};


static int __init led_init(void)
{
	int retval;
	
	retval = register_chrdev(LED_MAJOR,led_name,&led_fops);
	if(retval < 0)
	{
		printk(KERN_WARNING "Can't get major %d\n",LED_MAJOR);
		return retval;
	}

	printk("LED driver register success!\n");
	return 0;
}
static void __exit led_exit(void)
{
	unregister_chrdev(LED_MAJOR,led_name);
	printk("LED driver release success!\n");

}
module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DMATEK Co., Ltd <http://www.dmatek.com.tw>");
MODULE_DESCRIPTION("led  driver");


