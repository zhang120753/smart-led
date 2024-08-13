#include "data_global.h"

extern int msgid;
extern key_t key;

extern pthread_mutex_t mutex_client_request,
        		mutex_refresh,
        		mutex_sqlite,
	        	mutex_transfer,
	        	mutex_analysis,
	        	mutex_sms,
	        	//mutex_buzzer,
	         	mutex_led,
	         	mutex_camera;

extern pthread_cond_t  cond_client_request,
        		cond_refresh,
        		cond_sqlite,
	        	cond_transfer,
	        	cond_analysis,
	        	cond_sms,
	        	//cond_buzzer,
	         	cond_led,
	         	cond_camera;

extern char recive_phone[12] ;
extern char center_phone[12] ;

//extern unsigned char cmd_beep;
extern unsigned char cmd_led;

struct msg msgbuf;


//:A9LED模块线程.
void *pthread_led(void *arg)
{
	printf("pthread_led\n");
	
	int led_fd = open(LED_DEV, O_RDWR);
	
	if (0 > led_fd) 
	{
                perror("open error");
                exit(-1);
    }
	
	while(1){
		pthread_mutex_lock(&mutex_led);
        pthread_cond_wait(&cond_led,&mutex_led);
		printf("IN main cmd_led = %d", cmd_led);
        if(cmd_led != 0){
        	printf("open led\n");
       	 	write(led_fd, &cmd_led, 1);
		}
        if(cmd_led == 0){
        	printf("close led\n");
        	write(led_fd, &cmd_led, 1);
        }
        pthread_mutex_unlock(&mutex_led);

	}

	close(led_fd);

#if 0
	5.	open(dev_led,  )
	6.	pthread_cond_wait (cond_led,  );
	7.	获取dev_led_mask（控制标志）
	8.	通过ioctl（）控制led
#endif 
}

