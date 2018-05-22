/*
* Copyright (c) 2017,FAFU
* All rights reserved.
*
* File Name：IIC.h
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

#ifndef IIC_H
#define IIC_H
/*************************************
            INCLUDE FILE
**************************************/
#include <iocc2530.h>

/*************************************
            MACRO DEFINITION
**************************************/
#define  SCL P0_6       //IIC时钟引脚定义
#define  SDA P0_7       //IIC数据引脚定义
#define  SDAOUT P0DIR|=0x80
#define  SDAIN  P0DIR&=~0x80
#define  SDA_H  SDA=1
#define  SDA_L  SDA=0
#define  SCL_H  SCL=1
#define  SCL_L  SCL=0

/*************************************
            CUSTOM TYPE
**************************************/

/*************************************
            GLOBAL VARIABLES
**************************************/
/*Private Variables*/

/*Exported Variables*/

/*********************************************************
                  FUNCTION DELARATION
**********************************************************/
/*Private functions*/

/*Exported functions*/
void I2C_Init(void);
void I2C_Start(void);                  //起始信号
void I2C_Stop(void);                    //停止信号
void SendACK(void);       //应答ACK
void SendNCK(void) ;      //无应答ACK
unsigned char I2C_RcvByte(void);
void I2C_SendByte(unsigned char c);  //IIC单个字节写
unsigned char I2C_ISendByte(unsigned char sla,unsigned char c);//
void Delay_us(unsigned int timeout);
void Delay_ms(unsigned int Time);//n ms延时
unsigned char I2C_IRcvStrExt(unsigned char sla,unsigned char *s,unsigned char no);

#endif