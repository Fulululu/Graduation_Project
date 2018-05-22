//=====================================================================
//文件名称：PMS7003.c
//功能概要：空气颗粒物传感器（PM2.5）PMS7003构件源文件
//=====================================================================

#include "PMS7003.h"

//SET------3.3V
//RST------3.3V

//=====================================================================
//传感器命令协议：
//特征字节  特征字节  指令字节  状态字节1 状态字节2 校验字节1 校验字节2
// 0x42      0x4d      CMD      DATAH     DATAL    LRCH      LRCL
//指令及特征字节的定义：
//CMD  DATAH  DATAL      	  说明
//0xe2   x      x        	 被动读数
//0xe1   x    00H-被动式         状态切换
//            01H-主动式
//0xe4   x    00H-待机模式       待机控制
//       x    01H-正常模式
//校验字生成：
//从特征字开始所有字节累加和
//=====================================================================

//=========================================================================
//函数名称：PMS7003_init
//函数参数：无
//函数返回：无
//功能概要：初始化SET、RST引脚
//======================================================================
void PMS7003_Init(void)
{
    P0SEL &= ~0x30;                 //设置P0.4 P0.5为普通IO口
    P0DIR |= 0x30;                 //P0.4 P0.5定义为输出口
    //PMS7003_rest();
    //PMS7003_SET = 1;
    PMS7003_control(PMS7003_CMD_READSTATE_PASSTIVE);
}

//=========================================================================
//函数名称：PMS7003_control
//函数参数：cmd:控制命令
//函数返回：命令发送成功：0；命令发送失败：0x11
//功能概要：通过命令控制传感器工作状态
//======================================================================
unsigned char PMS7003_control(unsigned int cmd)
{
	unsigned char flag;
	//初始化命令帧
	unsigned char command[PMS7003_LENTH_CMD] = {0};

	PMS7003_buildframe(cmd,command);
	flag = HalUARTWrite(0, command, PMS7003_LENTH_CMD);

	//如果发送命令错误，返回错误码
	if(!flag)
	{
		return PMS7003_ERR_CMDFAIL;
	}

	return 0;
}

//=========================================================================
//函数名称：PMS7003_buildframe
//函数参数：cmd:控制命令
//		 *frame：命令帧数组
//函数返回：无
//功能概要：将命令组成命令帧
//======================================================================
void PMS7003_buildframe(unsigned int cmd_state,unsigned char *frame)
{
	unsigned char cmd;
	unsigned char statechar;
	unsigned char lrcl;
	unsigned char lrch;
	unsigned int lrc = 0;

	//计算命令和状态字节
	cmd = cmd_state >> 8;
	statechar = cmd_state;

	//组帧
	frame[0] = 0x42;
	frame[1] = 0x4d;
	frame[2] = cmd;
	frame[3] = 0x00;
	frame[4] = statechar;

	//计算校验字 节
	unsigned char i;
	for(i = 0 ; i < (PMS7003_LENTH_CMD-2) ; i++)
	{
		lrc += frame[i];
	}
	lrch = lrc >> 8;
	lrcl = lrc;
	//组帧
	frame[5] = lrcl;
	frame[6] = lrch;
}

//=========================================================================
//函数名称：PMS7003_rest
//函数参数：无
//函数返回：无
//功能概要：通过RST引脚复位传感器
//======================================================================
void PMS7003_rest(void)
{
	PMS7003_RST = 0;
        for(int i=0;i<500;i++);
        PMS7003_RST = 1;
}

//=========================================================================
//函数名称：PMS7003_sleep
//函数参数：无
//函数返回：无
//功能概要：通过SET引脚休眠传感器
//======================================================================
void PMS7003_sleep(void)
{
	PMS7003_SET = 0;
}

//======================================================================
//函数名称：PMS7003_datarec
//函数参数：databuf:数据接收缓存
//函数返回：接收成功：0 ；接收失败：0x12
//功能概要：接收传感器原始读数
//======================================================================
unsigned char PMS7003_recdata(unsigned char *databuf)
{
	unsigned char lrcl, lrch, i;
	unsigned int lrcsum, datasum = 0;
        PMS7003_control(PMS7003_CMD_PSSTIVEREAD);
	//接收传感器的数据
        i=HalUARTRead(0, databuf, PMS7003_LENTH_DATA);

	//计算校验和
	for(i = 0; i < (PMS7003_LENTH_DATA -2) ; i++)
	{
		datasum += databuf[i];
	}
	lrcl = databuf[PMS7003_LENTH_DATA-1];
	lrch = databuf[PMS7003_LENTH_DATA-2];
	lrcsum = (lrch<<8)+lrcl;
	//如果校验错误，返回错误码
	if(datasum != lrcsum)
        {
		return PMS7003_ERR_RECFAIL;
        }

	return 0;
}

unsigned char PMS7003_Read_Str(unsigned char *buf)
{
  unsigned char tmp[PMS7003_LENTH_DATA];
  unsigned int pm1, pm25, pm10;//CF=1，标准颗粒物，单位ug/m3
  unsigned int cnt = 0;
  while(PMS7003_recdata(tmp) == PMS7003_ERR_RECFAIL)
  {
    if(++cnt > 1000)
      return 1;         //timeout,sensor no response
  }
  pm1 = (tmp[4]<<8) + tmp[5];
  pm25 = (tmp[6]<<8) + tmp[7];
  pm10 = (tmp[8]<<8) + tmp[9];
      
  if(pm1<1000 && pm25<1000 && pm10<1000)
  {
    sprintf((char*)buf, "%3d %3d %3d", pm1, pm25, pm10);
    return 0;
  }
  return 1;              //should not arrive here
}

unsigned char PMS7003_Read(unsigned char *buf)
{
  unsigned char tmp[PMS7003_LENTH_DATA];
  unsigned int pm1, pm25, pm10;//CF=1，标准颗粒物，单位ug/m3
  unsigned int cnt = 0;
  while(PMS7003_recdata(tmp) == PMS7003_ERR_RECFAIL)
  {
    if(++cnt > 1000)
      return 1;         //timeout,sensor no response
  }
  pm1 = (tmp[4]<<8) + tmp[5];
  pm25 = (tmp[6]<<8) + tmp[7];
  pm10 = (tmp[8]<<8) + tmp[9];
      
  if(pm1<1000 && pm25<1000 && pm10<1000)
  {
    buf[0] = tmp[4];    //pm1 H
    buf[1] = tmp[5];    //pm1 L
    
    buf[2] = tmp[6];
    buf[3] = tmp[7];
    
    buf[4] = tmp[8];
    buf[5] = tmp[9];
    return 0;
  }
  return 1;              //should not arrive here
}

