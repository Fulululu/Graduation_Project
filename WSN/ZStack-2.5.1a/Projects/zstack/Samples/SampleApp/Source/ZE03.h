//======================================================================
//文件名称：ZE03.h
//功能概要：氧气浓度传感器ZE03构件头文件
//制作单位：福建农林大学物联网与嵌入式实验室
//更新记录：2017-12-03 V1.0
//======================================================================

#ifndef _ZE03_H
#define _ZE03_H

#include <iocc2530.h>
#include <hal_uart.h>
#include <stdio.h>

//引脚定义
#define ZE03_BAUD	9600

//命令/模式定义
#define ZE03_MODE_ACTIVE				0x03
#define ZE03_MODE_PASSTIVE			        0x04
#define ZE03_CMD_MODE_CHANGE			        0x78
#define ZE03_CMD_READ             		        0x86

//错误代码
#define ZE03_ERR_CMDSENDFAIL				0x41
#define ZE03_ERR_NOACK                                  0x42
#define ZE03_ERR_ACKCHECKFAIL				0x43
#define ZE03_ERR_CHECKFAIL				0x44
#define ZE03_ERR_READFAIL                               0x45

//命令长度
#define ZE03_LENTH_CMD  				9
//数据长度
#define ZE03_LENTH_DATA 				9

//=================接口函数声明=======================================
//=========================================================================
//函数名称：ZE03_init
//函数参数：无
//函数返回：无
//功能概要：初始化Sensor
//======================================================================
void ZE03_Init(void);

//check ack after cmd sent
unsigned char ZE03_CheckACK(void);

//=========================================================================
//函数名称：ZE03_control
//函数参数：cmd:控制命令
//函数返回：命令发送成功：0；命令发送失败：-1
//功能概要：通过命令控制传感器工作状态
//======================================================================
unsigned char ZE03_control(unsigned int cmd);

//=========================================================================
//函数名称：ZE03_buildframe
//函数参数：cmd:控制命令
//		 *frame：命令帧数组
//函数返回：无
//功能概要：将命令组成命令帧
//======================================================================
void ZE03_buildframe(unsigned int cmd,unsigned char *frame);

//=========================================================================
//函数名称：ZE03_datarec
//函数参数：databuf:数据接收缓存
//函数返回：接收成功：0 ；接收失败： 0x12
//功能概要：接收传感器原始读数
//======================================================================
unsigned char ZE03_recdata(unsigned char *databuf);

//读取并处理
unsigned char ZE03_Read_Str(unsigned char *buf);
unsigned char ZE03_Read(unsigned char *buf);

#endif
