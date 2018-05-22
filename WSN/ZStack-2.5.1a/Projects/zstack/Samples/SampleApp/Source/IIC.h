/*
* Copyright (c) 2017,FAFU
* All rights reserved.
*
* File Name��IIC.h
* Abstract��software IIC for CC2530
*
* Current Version��1.0
* Author��ygf
* Finish Date��20171024
*
* Superseded Version��1.0
* Authorship ��ygf
* Finish Date��20171024
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
#define  SCL P0_6       //IICʱ�����Ŷ���
#define  SDA P0_7       //IIC�������Ŷ���
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
void I2C_Start(void);                  //��ʼ�ź�
void I2C_Stop(void);                    //ֹͣ�ź�
void SendACK(void);       //Ӧ��ACK
void SendNCK(void) ;      //��Ӧ��ACK
unsigned char I2C_RcvByte(void);
void I2C_SendByte(unsigned char c);  //IIC�����ֽ�д
unsigned char I2C_ISendByte(unsigned char sla,unsigned char c);//
void Delay_us(unsigned int timeout);
void Delay_ms(unsigned int Time);//n ms��ʱ
unsigned char I2C_IRcvStrExt(unsigned char sla,unsigned char *s,unsigned char no);

#endif