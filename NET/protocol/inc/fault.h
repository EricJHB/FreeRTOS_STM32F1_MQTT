#ifndef _FAULT_H_
#define _FAULT_H_







typedef enum
{

	FAULT_NONE = 0,		//�޴���
	FAULT_REBOOT,		//������������
	FAULT_EDP,			//EDPЭ�����
	FAULT_NODEVICE,		//Ӳ�����ߴ��󣬱���8266����6311�Ӵ�������sim���Ӵ�������Ӳ��ԭ������Ĵ���

} FAULT_TYPE;

extern unsigned char faultType;

extern unsigned char faultTypeReport;




void Fault_Process(void);


#endif
