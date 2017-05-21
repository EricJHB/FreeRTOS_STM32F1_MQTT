#ifndef _ONENET_H_
#define _ONENET_H_


#include "mqtt_app.h"




extern struct MqttSampleContext ctx[1];

typedef struct
{

    char devID[15];
    char apiKey[35];
	
	char proID[10];
	char auif[50];
	
	char ip[16];
	char port[8];
	
	unsigned char netWork : 1;		//0-����ģʽ(AP)		1-������ģʽ(STA)
	unsigned char sendData : 3;		//���ݷ���
	unsigned char errCount : 3;		//�������
	unsigned char heartBeat : 1;

} ONETNET_INFO;

extern ONETNET_INFO oneNetInfo;

typedef enum
{

	TYPE_BOOL = 0,
	
	TYPE_CHAR,
	TYPE_UCHAR,
	
	TYPE_SHORT,
	TYPE_USHORT,
	
	TYPE_INT,
	TYPE_UINT,
	
	TYPE_LONG,
	TYPE_ULONG,
	
	TYPE_FLOAT,
	TYPE_DOUBLE,
	
	TYPE_GPS,

} DATA_TYPE;

typedef struct
{

	char *name;
	void *data;
	DATA_TYPE dataType;
	_Bool flag;

} DATA_STREAM;


#define SEND_BUF_SIZE  1024


#define CHECK_CONNECTED			0	//������
#define CHECK_CLOSED			1	//�ѶϿ�
#define CHECK_GOT_IP			2	//�ѻ�ȡ��IP
#define CHECK_NO_DEVICE			3	//���豸
#define CHECK_INITIAL			4	//��ʼ��״̬
#define CHECK_NO_CARD			5	//û��sim��
#define CHECK_NO_ERR			255 //

#define DEVICE_CMD_MODE			0 	//AT����ģʽ
#define DEVICE_DATA_MODE		1 	//ƽ̨�����·�ģʽ
#define DEVICE_HEART_MODE		2 	//��������ģʽ





void OneNet_DevLink(const char* devid, const char* auth_key);

_Bool OneNet_SendData(unsigned char len);

_Bool OneNet_Publish(void);

_Bool OneNet_Subscribe(void);

_Bool OneNet_UnSubscribe(void);

_Bool OneNet_HeartBeat(void);

void OneNet_App(char *cmd);


#endif
