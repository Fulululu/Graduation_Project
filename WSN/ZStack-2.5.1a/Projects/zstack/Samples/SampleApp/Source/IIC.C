/*
* Copyright (c) 2017,FAFU
* All rights reserved.
*
* File Name：IIC.c
* Abstract：software IIC for CC2530
*
* Current Version：1.0
* Author：ygf
* Finish Date：20171024
*
* Superseded Version：1.0
* Authorship ：ygf
* Finish Date：20171024
*/ 

#include "IIC.h"

unsigned char ack;

/*Private functions*/

/*Exported functions*/
/****************************
        延时函数
*****************************/
void Delay_us(unsigned int timeout) //1 us延时unsigned int16 timeout )
{
    while (timeout--)
    {
      asm("NOP");
      asm("NOP");
      asm("NOP");
    } 
}


void Delay_ms(unsigned int Time)//n ms延时
{
  unsigned char i;
  while(Time--)
  {
    for(i=0;i<100;i++)
     Delay_us(10);
  }
}
//*********************************************************
void I2C_Init()
{
    P0SEL &= ~0xC0;                //设置P0.6 P0_7为普通IO口
    P0DIR |= 0xC0;                 //P0.6 P0.7定义为输出口
}
/**************************************
起始信号
**************************************/
void I2C_Start()
{
    P0DIR |= 0x80;
    SDA = 1;                    //拉高数据线
    SCL = 1;                    //拉高时钟线
    Delay_us(5);                //延时
    SDA = 0;                    //产生下降沿
    Delay_us(5);                //延时
    SCL = 0;                    //拉低时钟线
}


/**************************************
停止信号
**************************************/
void  I2C_Stop()
{
    P0DIR|=0x80;
    SDA = 0;                    //拉低数据线
    SCL = 1;                    //拉高时钟线
    Delay_us(5);                //延时
    SDA = 1;                    //产生上升沿
    Delay_us(5);                //延时
}

/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void SendACK()
{
    P0DIR|=0x80;
    SDA = 0;                    //写应答信号
    SCL = 1;                    //拉高时钟线
    Delay_us(5);                //延时
    SCL = 0;                    //拉低时钟线
    SDA = 1;  
}

void SendNCK()
{
    P0DIR|=0x80;
    SDA = 1;                    //写应答信号
    SCL = 1;                    //拉高时钟线
    Delay_us(5);                //延时
    SCL = 0;                    //拉低时钟线
    SDA = 0; 
}

/*----------------------------------------------------------------              
I2C写入一个8位二进制数，高位在前低位在后
------------------------------------------------------------------*/
void  I2C_SendByte(unsigned char c)
{
  unsigned char BitCnt;
  P0DIR|=0x80;
  for(BitCnt=0;BitCnt<8;BitCnt++)  //要传送的数据长度为8位
    {
     
      if((c<<BitCnt)&0x80) SDA=1;   //判断发送位
       else  SDA=0;                
     SCL=1;               //置时钟线为高，通知被控器开始接收数据位
     Delay_us(5);         //保证时钟高电平周期大于4μ      
     SCL=0; 
    }  
     SDA=1;
    Delay_us(5);
    P0DIR&=~0x80;          //8位发送完后释放数据线，准备接收应答位
    SCL=1;
    Delay_us(5);
    if(SDA==1)ack=0;     
       else ack=1;        //判断是否接收到应答信号，“SDA=1”：没有
    SCL=0;
}

/*******************************************************************
                 向无子地址器件发送字节数据函数               
函数原型: bit  I2C_ISendByte(unsigned char sla,ucahr c);  
功能:     从启动总线到发送地址，数据，结束总线的全过程,从器件地址sla.
          如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
********************************************************************/
unsigned char I2C_ISendByte(unsigned char sla,unsigned char c)
{
   I2C_Start();               /*启动总线*/
   I2C_SendByte(sla);             /*发送器件地址*/
   if(ack==0)return(0);
   I2C_SendByte(c);               /*发送数据*/
   if(ack==0)return(0);
   I2C_Stop();                /*结束总线*/ 
   return(1);
}

/*******************************************************************
I2C读取一个8位二进制数，也是高位在前低位在后  
****************************************************************/	
unsigned char I2C_RcvByte()
{
  unsigned char retc;
  unsigned char BitCnt;
  retc=0; 
  P0DIR &= ~0x80;         //置数据线为输入方式
  for(BitCnt=0;BitCnt<8;BitCnt++)
  {       
    SCL=0;      
    Delay_us(5); //时钟低电平周期大于4.7us
    if(SDA==1) retc=retc+1; //读数据位,接收的数据位放入retc中
    SCL=1;
    retc=retc<<1;
    Delay_us(5);  
  }   
  SCL=0; 
  return(retc);
}

//*********************************************************
//从sla连续读出no个数据到*s
//*********************************************************
unsigned char I2C_IRcvStrExt(unsigned char sla,unsigned char *s,unsigned char no)
{   
    unsigned char i;	
    I2C_Start();                         //起始信号
    I2C_SendByte(sla+1);        //发送设备地址+读信号
    if(ack==0)return(0);
      for (i=0; i<no-1; i++)                      //连续读取6个地址数据，存储中BUF
      {
        *s=I2C_RcvByte();      
        SendACK();                //回应ACK
        s++;
      }		
        *s=I2C_RcvByte();
        SendNCK();      //最后一个数据需要回NOACK             
        I2C_Stop();   
        return(1);
}