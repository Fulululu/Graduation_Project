/*
* Copyright (c) 2017,FAFU
* All rights reserved.
*
* File Name：BH1750FVI.h
* Abstract：BH1750 driver for CC2530
*
* Current Version：1.0
* Author：ygf
* Finish Date：20171024
*
* Superseded Version：1.0
* Authorship ：ygf
* Finish Date：20171024
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
#define DPOWR  0X00         //关闭
#define POWER  0X01         //启动
#define RESET    0X07       //重置
#define CHMODE  0X10        //连续高分辨率检测模式
#define CHMODE2 0X11        //连续高分辨率检测模式2
#define CLMODE   0X13       //连续低分辨检测模式
#define H1MODE   0X20       //单次高分辨率模式
#define H1MODE2 0X21        //单次高分辨率检测模式2
#define L1MODE    0X23      //单次低分辨率模式
#define SEN100H   0x42      //灵敏度100%高位
#define SEN100L   0x65      //灵敏度100%低位
#define SEN50H    0x44      //灵敏度50%高位
#define SEN50L    0x6A      //灵敏度50%低位
#define SEN200H   0x41      //灵敏度200%高位
#define SEN200L   0x73      //灵敏度200%低位

#define BH1750ADDR   0x46 //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
                              //ALT  ADDRESS引脚接地时地址为0xA6，接电源时地址为0x3A
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