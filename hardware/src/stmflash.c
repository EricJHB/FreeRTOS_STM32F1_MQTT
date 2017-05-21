#include "stmflash.h"

#include "stm32f10x.h"









//���ܼ򵥣��ɵ�ַ��Ϊָ��ȥ��ȡ��ߵ����ݼ��ɣ�ʵ�����벻�漰��flash�ı����Ĳ�����������ȡ��ַ��ߵ�ֵ����
void Flash_Read(unsigned int addr, char *rBuf, unsigned short len)
{

	unsigned short lenCount = 0;
	unsigned char charBuf = 0;
	
	for(; lenCount < len; lenCount++)
	{
		charBuf = *(volatile unsigned char *)addr; //תΪuchar����
		if(charBuf == 0xff) //���������0xFF����������ݶ����ˣ���Ϊ�����˵�ssid��password���Ȳ�ͬ
			break;
		
		rBuf[lenCount] = (char)charBuf; //ת��һ�£���ֵ
		
		addr += 2; //����ĵ�ַ������2�ı���
	}

}

//�ж��Ƿ���Ҫ����
_Bool Flash_NeedErase(void) //1-��Ҫ����(������)		0-����Ҫ����(������)
{

	unsigned short rCount = 0;
	
	unsigned int addr = SSID_ADDR; //ҳ��ʼ��ַ
	
	for(; rCount < 1024; rCount++) //����2KB����		��һ�������ǰ��֣�����������1024
	{
		if(*(volatile unsigned short *)addr != 0xffff)
			return 1;
		
		addr += 2;
	}
	
	return 0;

}

//д�Ƚϸ��ӵ㣬ע�����㣬1.������2.ֻ�е�ַ��ߵ�ֵΪ0xFFFFʱ�ܹ���д���������ݣ�������дssid��pswdǰ��Ҫ����һ�Ρ�3.д��������
void Flash_Write(unsigned int addr, char *wBuf, unsigned short len)
{
	
	unsigned short lenCount = 0;
	
	FLASH_Unlock();	//����

	//֮ǰ������sb�ˣ����Ƿ�����д��ssid��pswd�ģ����ûע�����ssid�������ˣ������˴���졣������ȥ
//	if(Flash_NeedErase()) //��Ҫ����
//	{
//		FLASH_ErasePage(SSID_ADDRESS);
//	}
	
	for(; lenCount < len; lenCount++)
	{
		//FLASH_ProgramOptionByteData(addr, wBuf[lenCount]); //д�� //���ַ�ʽд����û�����������������϶���д���֣�����������ķ�ʽ
		FLASH_ProgramHalfWord(addr, (unsigned short)wBuf[lenCount]); //д��

		addr += 2; //��ַ������2�ı���
	}
	
	FLASH_Lock(); //����

}
