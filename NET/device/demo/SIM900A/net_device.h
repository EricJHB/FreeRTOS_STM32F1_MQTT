#ifndef _NET_DEVICE_H_
#define _NET_DEVICE_H_







typedef struct
{
	
	unsigned short err : 2; 		//错误类型
	unsigned short initStep : 4;	//初始化步骤
	unsigned char cardType : 3;		//手机卡为1，物联卡为5
	unsigned short dataType : 4;	//设定数据返回类型--16种
	unsigned short reboot : 1;		//死机重启标志
	unsigned short reverse : 2;		//预留

	unsigned short ipdBytes;

} NET_DEVICE_INFO;

extern NET_DEVICE_INFO netDeviceInfo;



#define NET_DEVICE_PWRK_ON		GPIO_ResetBits(GPIOC, GPIO_Pin_4)
#define NET_DEVICE_PWRK_OFF		GPIO_SetBits(GPIOC, GPIO_Pin_4)

#define NET_DEVICE_TRANS		0 //1-使能透传模式		0-失能透传模式




void NET_DEVICE_IO_Init(void);

_Bool NET_DEVICE_Exist(void);

_Bool NET_DEVICE_Init(char *ip, char *port);

void NET_DEVICE_Reset(void);

_Bool NET_DEVICE_ReLink(char *ip, char *port);

_Bool NET_DEVICE_SendCmd(char *cmd, char *res, _Bool mode);

void NET_DEVICE_SendData(unsigned char *data, unsigned short len);

unsigned char *NET_DEVICE_GetIPD(unsigned short timeOut);

void NET_DEVICE_ClrData(void);

unsigned char NET_DEVICE_Check(void);

void NET_DEVICE_ReConfig(unsigned char step);

void NET_DEVICE_Set_DataMode(unsigned char mode);

unsigned char NET_DEVICE_Get_DataMode(void);

#endif
