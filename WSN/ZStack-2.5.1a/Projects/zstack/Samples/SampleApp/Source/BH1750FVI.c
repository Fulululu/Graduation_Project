/*
* Copyright (c) 2017,FAFU
* All rights reserved.
*
* File Name£ºBH1750FVI.c
* Abstract£ºBH1750 driver for CC2530
*
* Current Version£º1.0
* Author£ºygf
* Finish Date£º20171024
*
* Superseded Version£º1.0
* Authorship £ºygf
* Finish Date£º20171024
*/ 

#include "BH1750FVI.h"

//A0------GND

/*Private functions*/

/*Exported functions*/
void BH1750_Init(void)
{
   I2C_ISendByte(BH1750ADDR,POWER); 
   I2C_ISendByte(BH1750ADDR,CHMODE);
   Delay_ms(180);
}

unsigned char BH1750_Read_Str(unsigned char *buf)
{
   unsigned char I2C_buf[2];
   unsigned int bh1750sum = 0;
   unsigned char timeout = 0;
   while(I2C_IRcvStrExt(0x46,I2C_buf,2) == 0)      //16bit data in total
   {
      if(++timeout >= 10) return 1;              //timeout,sensor no response
   }
   Delay_ms(180);
   bh1750sum=I2C_buf[0];                      //data conbine
   bh1750sum=(bh1750sum<<8)+I2C_buf[1];
   
   buf[0]=bh1750sum/10000+0x30;               //to string
   bh1750sum=bh1750sum%10000;
   buf[1]=bh1750sum/1000+0x30;
   bh1750sum=bh1750sum%1000;
   buf[2]=bh1750sum/100+0x30;
   bh1750sum=bh1750sum%100;
   buf[3]=bh1750sum/10+0x30;
   buf[4]=bh1750sum%10+0x30;
   Delay_ms(180);
   return 0;
}

unsigned char BH1750_Read(unsigned char buf[])
{
   unsigned char I2C_buf[2];
   unsigned char timeout = 0;
   while(I2C_IRcvStrExt(0x46,I2C_buf,2) == 0)      //16bit data in total
   {
      if(++timeout >= 10) return 1;              //timeout,sensor no response
   }
   Delay_ms(180);
   buf[0]=I2C_buf[0];                      //data conbine
   buf[1]=I2C_buf[1];

   Delay_ms(180);
   return 0;
}