//=====================================================================
//文件名称：ZE03.c
//功能概要：氧气浓度传感器ZE03构件源文件
//=====================================================================

#include "ZE03.h"

//SET------3.3V
//RST------3.3V

//=====================================================================
//传感器命令协议：
//特征字节    地址    指令字节  通讯模式            校验字节
// 0xFF      0x01      CMD      STATE    0 0 0 0    0x84
//=====================================================================

//=========================================================================
//函数名称：ZE03_init
//函数参数：无
//函数返回：无
//功能概要：初始化SET、RST引脚
//======================================================================
void ZE03_Init(void)
{
    ZE03_control(ZE03_MODE_PASSTIVE);
}

unsigned char ZE03_CheckACK(void)
{
  unsigned char flag = 0, timeout;
  unsigned char ackbuf[ZE03_LENTH_DATA];
  
  for(timeout=0;timeout<10;timeout++)
  {
    flag = HalUARTRead(0, ackbuf, ZE03_LENTH_DATA);
    if(flag != 0)
    {
      break;
    }
  }
  
  if(timeout >= 10)
  {
    return ZE03_ERR_NOACK;
  }
  else
  {
    if(ackbuf[0]==0xff && ackbuf[1]==0x78 && ackbuf[0]==0x01 && ackbuf[8]==0x84)
    {
      return 0;
    }
    else
    {
      return ZE03_ERR_ACKCHECKFAIL;
    }
  }
  
}

//=========================================================================
//函数名称：ZE03_control
//函数参数：cmd:控制命令
//函数返回：命令发送成功：0；命令发送失败：0x11
//功能概要：通过命令控制传感器工作状态
//======================================================================
unsigned char ZE03_control(unsigned int cmd)
{
	unsigned char flag;
	//初始化命令帧
	unsigned char command[ZE03_LENTH_CMD] = {0};

	ZE03_buildframe(cmd,command);
	flag = HalUARTWrite(0, command, ZE03_LENTH_CMD);

	//如果发送命令错误，返回错误码
	if(!flag)
	{
		return ZE03_ERR_CMDSENDFAIL;
	}
        
        flag = ZE03_CheckACK();
        if(flag!=ZE03_ERR_NOACK || flag!=ZE03_ERR_ACKCHECKFAIL)
        {
          return ZE03_ERR_CHECKFAIL;
        }
        
	return 0;
}

//=========================================================================
//函数名称：ZE03_buildframe
//函数参数：cmd:控制命令
//		 *frame：命令帧数组
//函数返回：无
//功能概要：将命令组成命令帧
//======================================================================
void ZE03_buildframe(unsigned int cmd,unsigned char *frame)
{
	switch(cmd)
	{
	case ZE03_MODE_ACTIVE:
		frame[0] = 0xff;
		frame[1] = 0x01;
		frame[2] = ZE03_CMD_MODE_CHANGE;
		frame[3] = ZE03_MODE_ACTIVE;
		frame[4] = 0x00;
		frame[5] = 0x00;
		frame[6] = 0x00;
		frame[7] = 0x00;
		frame[8] = 0x84;
		break;
	case ZE03_MODE_PASSTIVE:
		frame[0] = 0xff;
		frame[1] = 0x01;
		frame[2] = ZE03_CMD_MODE_CHANGE;
		frame[3] = ZE03_MODE_PASSTIVE;
		frame[4] = 0x00;
		frame[5] = 0x00;
		frame[6] = 0x00;
		frame[7] = 0x00;
		frame[8] = 0x83;
		break;
	case ZE03_CMD_READ:
		frame[0] = 0xff;
		frame[1] = 0x01;
		frame[2] = ZE03_CMD_READ;
		frame[3] = 0x00;
		frame[4] = 0x00;
		frame[5] = 0x00;
		frame[6] = 0x00;
		frame[7] = 0x00;
		frame[8] = 0x79;
		break;
	default:
		break;
	}
}

//======================================================================
//函数名称：ZE03_datarec
//函数参数：databuf:数据接收缓存
//函数返回：接收成功：0 ；接收失败：0x12
//功能概要：接收传感器原始读数
//======================================================================
unsigned char ZE03_recdata(unsigned char *databuf)
{
	unsigned char lrc, i;
	unsigned int datasum = 0;
        ZE03_control(ZE03_CMD_READ);
	//接收传感器的数据
        i=HalUARTRead(0, databuf, ZE03_LENTH_DATA);

	//计算校验和
	for(i = 0; i < (ZE03_LENTH_DATA -1) ; i++)
	{
		datasum += databuf[i];
	}
        
        lrc = databuf[i];
	//如果校验错误，返回错误码
	if(datasum != lrc)
        {
		return ZE03_ERR_READFAIL;
        }

	return 0;
}

unsigned char ZE03_Read_Str(unsigned char *buf)
{
  unsigned char tmp[ZE03_LENTH_DATA];
  unsigned int o2;//浓度百分比
  unsigned int cnt = 0;
  while(ZE03_recdata(tmp) == ZE03_ERR_READFAIL)
  {
    if(++cnt > 100)
      return 1;         //timeout,sensor no response
  }
  o2 = (tmp[2]<<8) + tmp[3];
      
  if(o2>0 && o2<25)     //0%~25%
  {
    sprintf((char*)buf, "%3d", o2);
    return 0;
  }
  return 1;              //should not arrive here
}

unsigned char ZE03_Read(unsigned char *buf)
{
  unsigned char tmp[ZE03_LENTH_DATA];
  unsigned int o2;//浓度百分比
  unsigned int cnt = 0;
  while(ZE03_recdata(tmp) == ZE03_ERR_READFAIL)
  {
    if(++cnt > 100)
      return 1;         //timeout,sensor no response
  }
  o2 = (tmp[2]<<8) + tmp[3];
      
  if(o2>0 && o2<25)     //0%~25%
  {
    buf[0]=o2 >> 8;
    buf[1]=o2;
    return 0;
  }
  return 1;              //should not arrive here
}

