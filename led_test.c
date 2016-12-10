
#include <stdio.h>
#include <stdlib.h>	//system
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
//#include <linux/delay.h>
#include <sys/ioctl.h>
#include "led.h"
#define LED_TEST 3

#define DEVICE_BLTEST "/dev/led"      //设备节点

int main(int argc, char *argv[])
{
		int fd;
		int val = -1;
		int num = 1;
		int m = 0;
		int i = 0;		
		fd= open(DEVICE_BLTEST,O_RDONLY); //打开设备
		if(fd<0)
		{
			perror("can not open device");
			exit(1);
		}
	
		while(1)
		{
			printf("please select number to control led\n");
			printf("1:led:D608 2:led2 :D609 3:marquee 4:exit : ->");
			m = 0;
			scanf("%d", &val);	
			printf("\n");
			if(val == 1 || val == 2){
				printf("select led on or off: 1:ON  2:OFF: ->");
				scanf("%d", &num);
				printf("\n");
				}
			switch(val)
				{
				 case 1: 
				 	if(num == LED_ON)
						ioctl(fd,LED_ON,&val);
					else if(num == LED_OFF)
						ioctl(fd,LED_OFF,&val);

					break;
				 case 2:
				 	if(num == LED_ON)
						ioctl(fd,LED_ON,&val);
					else if(num == LED_OFF)
						ioctl(fd,LED_OFF,&val);
					break;
				 case 3:
					for (m = 0;m < 10;m++){
					    val = 1;
					    ioctl(fd,LED_ON,&val);
					    sleep(1);
 					    ioctl(fd,LED_OFF,&val);
                                            val = 2;
					    ioctl(fd,LED_ON,&val);
					    sleep(1);
					    ioctl(fd,LED_OFF,&val);
					}
					break;
				 case 4:
					close(fd);
				default:
					return 0;

			}

		}
		//close(fd);
		
		return 0;
}
