#ifndef _SELFCHECK_H_
#define _SELFCHECK_H_







typedef struct
{

	unsigned short SHT20_OK : 1;		//��ʪ�ȴ�����������־λ
	unsigned short ADXL345_OK : 1;		//���ᴫ����������־λ
	unsigned short EEPROM_OK : 1;		//�洢��������־λ
	unsigned short OLED_OK : 1;			//OLE������־λ
	
	unsigned short NET_DEVICE_OK : 1;	//�����豸������־λ

} CHECK_INFO;

#define DEV_OK		1
#define DEV_ERR		0

extern CHECK_INFO checkInfo;



void Check_PowerOn(void);


#endif
