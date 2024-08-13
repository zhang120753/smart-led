#include "data_global.h"

extern int msgid;
extern key_t key;
//extern unsigned char cmd_beep;

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


struct msg msgbuf;




//:处理消息队列里请求的线程
void *pthread_client_request(void *arg)
{
	if((key = ftok("/tmp",'g')) < 0){
		perror("ftok failed .\n");
		exit(-1);
	}

	msgid = msgget(key,IPC_CREAT|IPC_EXCL|0666);
	if(msgid == -1)	{
		if(errno == EEXIST){
			msgid = msgget(key,0777);
		}else{
			perror("fail to msgget");
			exit(1);
		}
	}
	printf("pthread_client_request\n");
	
	while(1){
		bzero(&msgbuf,sizeof(msgbuf));
		printf("wait form client request...\n");
		msgrcv (msgid, &msgbuf, sizeof (msgbuf) - sizeof (long), 1L, 0);
		printf ("Get %ldL msg\n", msgbuf.msgtype);
		printf ("text[0] = %#x\n", msgbuf.text[0]);

		//1L~5L为线程分类的消息类型
		switch(msgbuf.msgtype){
			case 1L:
				printf("hello led\n");
				pthread_mutex_lock(&mutex_led);
				cmd_led = msgbuf.text[0];
				pthread_mutex_unlock(&mutex_led);
				pthread_cond_signal(&cond_led);	
				break;
			case 2L:
				/*
				printf("hello beep\n");
				pthread_mutex_lock(&mutex_buzzer);
				cmd_beep = msgbuf.text[0];
				pthread_mutex_unlock(&mutex_buzzer);
				pthread_cond_signal(&cond_buzzer);
				*/
				break;
			case 3L:
				pthread_mutex_lock(&mutex_led);
				printf("hello seg\n");
				cmd_seg = msgbuf.text[0];
				pthread_mutex_unlock(&mutex_led);
				pthread_cond_signal(&cond_led);
				break;
			case 4L:
					printf("hello fan\n");
				break;
			
			case 5L:
					printf("set env data\n");
					printf("temMAX: %d\n",*((int *)&msgbuf.text[1]));
					printf("temMIN: %d\n",*((int *)&msgbuf.text[5]));
					printf("humMAX: %d\n",*((int *)&msgbuf.text[9]));
					printf("humMAX: %d\n",*((int *)&msgbuf.text[13]));
					printf("illMAX: %d\n",*((int *)&msgbuf.text[17]));
					printf("illMAX: %d\n",*((int *)&msgbuf.text[21]));

				break;
			case 6L:
			case 7L:
			case 8L:
			case 9L:
					printf("浣犲彲浠ュ皢杩欎簺浣滀负鎵╁睍鏉ュ涔狅紝鍔犳补.\n");
				break;
			case 10L:
				{
					int i = 0 , j = 0 ;
					for(i = 0 ; i < 11; i++){
						recive_phone[i] = msgbuf.text[i]; 	
					}
					recive_phone[i] = '\0';
					printf("recive:%s\n",recive_phone);
					for(j = 0 ;msgbuf.text[i] != '\0' && j < 12; i++, j++)
					{
						center_phone[j] =  msgbuf.text[i];
					}
					center_phone[j] = '\0';
					printf("center:%s\n",center_phone);
					#if 0
						pthread_mutex_lock (&mutex_slinklist);
						sqlite_InsertLinknode (ENV_UPDATE, all_info_RT, sto_no, 0);//0,0閸掑棗鍩嗛弰顖欑波鎼存挸褰块崪宀冩彛閻椻晝顫掔猾璇插娇
						pthread_mutex_unlock (&mutex_slinklist);
						pthread_cond_signal (&cond_sqlite);
					#endif 
				 }
				break;
			default:
				break;
				
		}
	}

}


#if 0

		long msgtype;//鍏蜂綋鐨勬秷鎭被鍨?
		娑堟伅绫诲瀷鐨勫垎閰嶏細
			1L: 		LED鎺у埗
			2L:			铚傞福鍣ㄦ帶鍒?
			3L:			鍥涜矾LED鐏ā鎷熺殑鏁扮爜绠?
			4L:			椋庢墖
			5L:			娓╂箍搴︽渶鍊艰缃?
			6L-7L-8L-9L,鐢ㄤ簬涓汉鐨勬墿灞?
			10L: 		3G閫氫俊妯″潡-GPRS 
		switch(msgbuf.msgtype){
			case 1L: ...  break;
			....
			default ....  break;
		}
#endif 





