#ifndef _INFO_H_
#define _INFO_H_


#define DEVID_ADDRESS       120		//起始地址，第一个数据表示长度。最大19字节。
#define AKEY_ADDRESS        140		//起始地址，第一个数据表示长度。最大59字节。

#define PROID_ADDRESS		200		//起始地址，第一个数据表示长度。最大9字节。产品ID
#define AUIF_ADDRESS		210		//起始地址，第一个数据表示长度。最大49字节。自定义鉴权信息





unsigned char Info_Check(void);

unsigned char Info_WifiLen(unsigned char sp);

_Bool Info_Read(void);

_Bool Info_Alter(char *info);


#endif
