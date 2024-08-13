#include "data_global.h"


extern unsigned char cmd_beep;
extern pthread_mutex_t mutex_buzzer;
extern pthread_cond_t cond_buzzer;

//:A9蜂鸣器控制线程.
void *pthread_buzzer(void *arg)
{
	printf("pthread_buzzer\n");

	/*打开触发模式文件*/
	int beeep_fd = open("/sys/class/leds/beep/trigger", O_RDWR);
        if (0 > beeep_fd) {
                perror("open error");
                exit(-1);
        }


	/*打开蜂鸣器文件*/
	int beep_fd = open("/sys/class/leds/beep/brightness", O_RDWR);
	if(beep_fd < 0){
		printf("file %s open failed!\r\n");
		return -1;
	}
        
        /*设置触发模式为none*/
        write(beeep_fd, "none", 4);



        while(1){
        	pthread_mutex_lock(&mutex_buzzer);
        	pthread_cond_wait(&cond_buzzer,&mutex_buzzer);
        	if(cmd_beep != 0){
       	 		printf("open beep\n");
        		write(beep_fd, "1", 1);
			}else{
        		printf("close beep\n");
        		write(beep_fd, "0", 1);
        	}
        	pthread_mutex_unlock(&mutex_buzzer);
        }

	close(beep_fd);
        cloese(beeep_fd);

#if 0	
	1.	open(dev_buzzer,  )
	2.	pthread_cond_wait (cond_buzzer,  );
	3.	获取dev_buzzer_mask(控制标志)
	4.	通过ioctl（）控制buzzer
#endif 
}



