#ifndef __DATA_GLOBAL__
#define __DATA_GLOBAL__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <termios.h>
#include <syscall.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>

#define MONITOR_NUM   1
#define QUEUE_MSG_LEN  32

typedef  unsigned char uint8_t;
typedef  unsigned short uint16_t;
typedef  unsigned int uint32_t;

#define DEV_GPRS 			"/dev/s3c2410_serial1"
#define DEV_LED				"/dev/myled"
#define DEV_INFRARED		"/dev/key"



//考虑到内存对齐的问题
struct makeru_zigbee_info{
	uint8_t head[3]; //标识位: 'm' 's' 'm'  makeru-security-monitor  
	uint8_t type;	 //数据类型  'z'---zigbee  'a'---a9
	float temperature; //温度
	float humidity;  //湿度
	float tempMIN;//温度下限
	float tempMAX;//温度上限 
	float humidityMIN;   //湿度下限
	float humidityMAX;   //湿度上限
	uint32_t reserved[2]; //保留扩展位，默认填充0
};

struct makeru_a9_info{
	uint8_t head[3]; //标识位: 'm' 's' 'm'  makeru-security-monitor  
	uint8_t type;	 //数据类型  'z'---zigbee  'a'---a9
	float adc;
	short gyrox;   //陀螺仪数据
	short gyroy;
	short gyroz;
	short  aacx;  //加速计数据
	short  aacy;
	short aacz;
	uint32_t reserved[2]; //保留扩展位，默认填充0
};

struct makeru_env_data{
	struct makeru_a9_info       a9_info;    
	struct makeru_zigbee_info   zigbee_info;
	uint32_t reserved[2]; //保留扩展位，默认填充0
};

//所有监控区域的信息结构体
struct env_info_client_addr
{
	struct makeru_env_data  monitor_no[MONITOR_NUM];	//数组  老家---新家
};

extern void *pthread_sqlite (void *);			//数据库线程
extern void *pthread_analysis (void *);			//数据解析线程
extern void *pthread_transfer (void *);			//数据接收线程
extern void *pthread_uart_cmd (void *);			//命令发送线程
extern void *pthread_client_request (void *);	//接收CGI，QT请求
extern void *pthread_infrared (void *);			//红外线程，用按键模拟
extern void *pthread_led (void *);				//LED控制线程
extern void *pthread_sms (void *);				//发送短信线程
extern void *pthread_refresh (void *);	//共享内存数据刷新线程

struct rfid_info
{
	unsigned char rfid_type;	
	unsigned char goods_no;
	unsigned char goods_count;
};

struct data_all
{
	unsigned char dev_no;
	unsigned char msg_type;
	float temperature;
	float humidity;
	float illumination;
	struct rfid_info goods_info;
	short crc;
};

struct storage_goods_info
{
	unsigned char goods_type;
	unsigned int goods_count;
};

/*struct storage_info
{
	unsigned char storage_status;				// 0:open 1:close
	unsigned char led_status;
	unsigned char fan_status;
	unsigned char seg_status;
	signed char x;
	signed char y;
	signed char z;
	char samplingTime[20];
	float temperature;
	float temperatureMIN;
	float temperatureMAX;
	float humidity;
	float humidityMIN;
	float humidityMAX;
	float illumination;
	float illuminationMIN;
	float illuminationMAX;
	float battery;
	float adc;
	float adcMIN;
	struct storage_goods_info goods_info[GOODS_NUM];
};

struct env_info_client_addr
{
	struct storage_info storage_no[STORAGE_NUM];	
};

struct sqlite_operation
{
	int table_select_mask;
	int env_operation_mask;
	int table_operation_mask;
	int goods_operation_mask;
};*/

extern int send_msg_queue(long type,unsigned char text);

struct msg
{
	long type;
	long msgtype;
	unsigned char text[QUEUE_MSG_LEN];
};

#endif
