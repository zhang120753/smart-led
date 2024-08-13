#include "data_global.h"
#include "sem.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <fcntl.h>

#define AP3216C_ADDR 0x1e

static int fd;
int ap3216c_init(void)
{

	char buf[100];
	//int err;
	struct i2c_rdwr_ioctl_data rdwr;
	struct i2c_msg msgs[1];
	int nmsgs_sent;
	fd=open("/dev/i2c-0",O_RDWR);//打开设备，读写
	if(fd<0)
	{
		//printf("can not open i2c bus  %d\n",I2C_BUS);
		printf("can not open i2c bus\n");
		return 0;
	} 
    msgs[0].addr  = AP3216C_ADDR;  //ap3216c的地址
	msgs[0].flags = 0;      // 写数据:0，读数据：1 
	msgs[0].len   = 2;      //buf长度这里为2
	msgs[0].buf   = buf;
	buf[0] = 0;  //第一字节为寄存器地址
	buf[1] = 4;  //第二字节是数据
	rdwr.msgs = msgs;
	rdwr.nmsgs = 1;//msgs的数量
	nmsgs_sent = ioctl(fd, I2C_RDWR, &rdwr);
	if (nmsgs_sent != 1)
	{
		printf("can not reset ap3216c\n");
		return -1;  /* err */
	}

	/* enable ap3216c */
	msgs[0].addr  = AP3216C_ADDR;
	msgs[0].flags = 0;      /* 写数据:0，读数据：1 */
	msgs[0].len   = 2;
	msgs[0].buf   = buf;
	buf[0] = 0;  //第一字节为寄存器地址
	buf[1] = 3;  //第二字节是数据

	rdwr.msgs = msgs;
	rdwr.nmsgs = 1;
	nmsgs_sent = ioctl(fd, I2C_RDWR, &rdwr);//调用驱动层代码，将数据传递
	if (nmsgs_sent != 1)
	{
		printf("can not enable ap3216c\n");
		return -1;  /* err */
	}
	
	return 0;

}


int  ap3216c_read_light(void)
{
	struct i2c_rdwr_ioctl_data rdwr;
	struct i2c_msg msgs[2];
	int  nmsgs_sent;
	char buf_tx[1];
    char buf_rx[2];
	int light;
	msgs[0].addr=AP3216C_ADDR;
	msgs[0].flags=0;//写数据
    msgs[0].len=1;
	msgs[0].buf=buf_tx;
	buf_tx[0]=0xc; //寄存器地址

	msgs[1].addr  = AP3216C_ADDR;
	msgs[1].flags = I2C_M_RD;	   /* 读数据:1, I2C_M_RD */
	msgs[1].len   = 2; //读取长度2字节
	msgs[1].buf   = buf_rx;
	rdwr.msgs = msgs;
	rdwr.nmsgs = 2;
	nmsgs_sent = ioctl(fd, I2C_RDWR, &rdwr);//调用驱动层代码
	if (nmsgs_sent != 2)
	{
		printf("can not read ap3216c light\n");
		return -1;	/* err */
	}
   light = (buf_rx[1]<<8) | buf_rx[0];
	
	return light;

}

int ap3216c_read_distance(void)
{
	struct i2c_rdwr_ioctl_data rdwr;
	struct i2c_msg msgs[2];
	int nmsgs_sent;

	char buf_tx[1];
	char buf_rx[2];

	int distance;

	/* 发送寄存器地址 */
	msgs[0].addr  = AP3216C_ADDR;
	msgs[0].flags = 0;		/* 写:0*/
	msgs[0].len   = 1;
	msgs[0].buf   = buf_tx;
	buf_tx[0] = 0xe; //寄存器地址

	msgs[1].addr  = AP3216C_ADDR;
	msgs[1].flags = I2C_M_RD;	   /* read:1, 读:I2C_M_RD */
	msgs[1].len   = 2;
	msgs[1].buf   = buf_rx;

	rdwr.msgs = msgs;
	rdwr.nmsgs = 2;
	nmsgs_sent = ioctl(fd, I2C_RDWR, &rdwr);
	if (nmsgs_sent != 2)
	{
		printf("can not read ap3216c light\n");
		return -1;	/* err */
	}

	distance = ((buf_rx[1] & 0x3F)<<4) | (buf_rx[0] & 0xf);
	
	return distance;
}

#define N 1024  //for share memory

extern int shmid;
extern int msgid;
extern int semid;

extern key_t shm_key;
extern key_t sem_key;
extern key_t key; //msg_key

extern pthread_mutex_t mutex_client_request,
	   mutex_refresh,
	   mutex_sqlite,
	   mutex_transfer,
	   mutex_analysis,
	   mutex_sms,
	   mutex_buzzer,
	   mutex_led,
	   mutex_camera;

extern pthread_cond_t  cond_client_request,
	   cond_refresh,
	   cond_sqlite,
	   cond_transfer,
	   cond_analysis,
	   cond_sms,
	   cond_buzzer,
	   cond_led,
	   cond_camera;


extern struct env_info_client_addr  sm_all_env_info;

struct shm_addr
{
	char shm_status;   //shm_status���Ե���home_id���������ֹ����ڴ�����
	struct env_info_client_addr  sm_all_env_info;
};
struct shm_addr *shm_buf;

int file_env_info_struct(struct env_info_client_addr  *rt_status,int home_id);//ģ������ˢ�µĺ���

void *pthread_refresh(void *arg)
{
	
	//semaphore for access to resource limits
	if((sem_key = ftok("/tmp",'i')) < 0){
		perror("ftok failed .\n");
		exit(-1);
	}

	semid = semget(sem_key,1,IPC_CREAT|IPC_EXCL|0666);
	if(semid == -1)	{
		if(errno == EEXIST){
			semid = semget(sem_key,1,0777);
		}else{
			perror("fail to semget");
			exit(1);
		}
	}else{
		init_sem (semid, 0, 1);
	}

	//share memory for env_info refresh config
	if((shm_key = ftok("/tmp",'i')) < 0){
		perror("ftok failed .\n");
		exit(-1);
	}

	shmid = shmget(shm_key,N,IPC_CREAT|IPC_EXCL|0666);
	if(shmid == -1)	{
		if(errno == EEXIST){
			shmid = shmget(key,N,0777);
		}else{
			perror("fail to shmget");
			exit(1);
		}
	}

	//share memap
	if((shm_buf = (struct shm_addr *)shmat(shmid,NULL,0)) == (void *)-1)
	{
		perror("fail to shmat");
		exit(1);
	}
	printf("pthread_refresh ......>>>>>>>\n");
	bzero (shm_buf, sizeof (struct shm_addr));
	while(1){
		sem_p(semid,0); //P����
		shm_buf->shm_status = 1;
		int home_id = 1;
#if 1
		file_env_info_struct(&shm_buf->sm_all_env_info,shm_buf->shm_status);
		shm_buf->sm_all_env_info.monitor_no[home_id] = sm_all_env_info.monitor_no[home_id];  //��ʵ�����ϴ�
		sleep(3);
//#else
#endif 
		sleep(1);
		sem_v(semid,0); //v����
		pthread_cond_signal(&cond_transfer);
	}


}



int file_env_info_struct(struct env_info_client_addr *rt_status,int home_id)
{
	int  env_info_size = sizeof(struct env_info_client_addr);
	//	printf("env_info_size = %d.\n",env_info_size);
	int err; 
	int light, distance;
	int cnt = 0;

	err = ap3216c_init();
	if (err)
	{
		printf("ap3216c_init err : %d\n", err);
	}
	light = ap3216c_read_light();	
	distance = ap3216c_read_distance();


	rt_status->monitor_no[home_id].zigbee_info.temperature = light;
	rt_status->monitor_no[home_id].zigbee_info.tempMIN = 2.0;
	rt_status->monitor_no[home_id].zigbee_info.tempMAX = 20.0;
	rt_status->monitor_no[home_id].zigbee_info.humidity  = distance;
	rt_status->monitor_no[home_id].zigbee_info.humidityMIN  = 10.0;
	rt_status->monitor_no[home_id].zigbee_info.humidityMAX  = 30.0;
	rt_status->monitor_no[home_id].zigbee_info.reserved[0]  = 0.01;
	rt_status->monitor_no[home_id].zigbee_info.reserved[1]  = -0.01;


	rt_status->monitor_no[home_id].a9_info.adc  = 9.0;
	rt_status->monitor_no[home_id].a9_info.gyrox  = -14.0;
	rt_status->monitor_no[home_id].a9_info.gyroy  = 20.0;
	rt_status->monitor_no[home_id].a9_info.gyroz  = 40.0;
	rt_status->monitor_no[home_id].a9_info.aacx  = 642.0;
	rt_status->monitor_no[home_id].a9_info.aacy  = -34.0;
	rt_status->monitor_no[home_id].a9_info.aacz  = 5002.0;
	rt_status->monitor_no[home_id].a9_info.reserved[0]  = 0.01;
	rt_status->monitor_no[home_id].a9_info.reserved[1]  = -0.01;

	//���stm32���ֵ����ݡ�arduino���ݣ�

	return 0;
}
