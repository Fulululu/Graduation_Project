#ifndef __DS18B20_H__
#define __DS18B20_H__

extern unsigned char Ds18b20Initial(void);
extern unsigned char ReadDs18B20(void);
extern unsigned char Ds18B20_Readfloat(unsigned char *buf);
unsigned char Ds18B20_Readfloat_Str(unsigned char *buf);
#endif