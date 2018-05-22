/*
* Copyright (c) 2017,FAFU
* All rights reserved.
*
* File Name��BH1750FVI.h
* Abstract��BH1750 driver for CC2530
*
* Current Version��1.0
* Author��ygf
* Finish Date��20171024
*
* Superseded Version��1.0
* Authorship ��ygf
* Finish Date��20171024
*/ 

#ifndef BH1750FVI_H
#define BH1750FVI_H
/*************************************
            INCLUDE FILE
**************************************/
#include <iocc2530.h>
#include "IIC.h"

/*************************************
            MACRO DEFINITION
**************************************/
#define DPOWR  0X00         //�ر�
#define POWER  0X01         //����
#define RESET    0X07       //����
#define CHMODE  0X10        //�����߷ֱ��ʼ��ģʽ
#define CHMODE2 0X11        //�����߷ֱ��ʼ��ģʽ2
#define CLMODE   0X13       //�����ͷֱ���ģʽ
#define H1MODE   0X20       //���θ߷ֱ���ģʽ
#define H1MODE2 0X21        //���θ߷ֱ��ʼ��ģʽ2
#define L1MODE    0X23      //���εͷֱ���ģʽ
#define SEN100H   0x42      //������100%��λ
#define SEN100L   0x65      //������100%��λ
#define SEN50H    0x44      //������50%��λ
#define SEN50L    0x6A      //������50%��λ
#define SEN200H   0x41      //������200%��λ
#define SEN200L   0x73      //������200%��λ

#define BH1750ADDR   0x46 //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
                              //ALT  ADDRESS���Žӵ�ʱ��ַΪ0xA6���ӵ�Դʱ��ַΪ0x3A
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
void BH1750_Init(void);
unsigned char BH1750_Read_Str(unsigned char *buf);
unsigned char BH1750_Read(unsigned char *buf);

#endif