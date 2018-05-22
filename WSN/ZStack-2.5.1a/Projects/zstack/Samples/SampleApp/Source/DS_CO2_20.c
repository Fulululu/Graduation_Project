//=====================================================================
//文件名称：DS_CO2_20.c
//功能概要：CO2传感器DS_CO2_20构件源文件
//=====================================================================

#include "DS_CO2_20.h"

unsigned char frame[] = {0x42, 0x4D, 0xE3, 0x00, 0x00 , 0x01, 0x72};

//RST------3.3V

//=====================================================================
//传感器命令协议：
//特征字节  特征字节  指令字节  状态字节1 状态字节2 校验字节1 校验字节2
// 0x42      0x4d      CMD     DATAH      DATAL    LRCH       LRCL
//指令及特征字节的定义：
//CMD  DATAH  DATAL      			说明
//0xe3   x      x        		 被动读数
//校验字生成：
//从特征字开始所有字节累加和
//=====================================================================

//=========================================================================
//函数名称：DS_CO2_20_init
//函数参数：无
//函数返回：无
//功能概要：初始化SET、RST引脚
//======================================================================
void DS_CO2_20_Init(void)
{
    P0SEL &= ~0x30;                 //设置P0.4 P0.5为普通IO口
    P0DIR |= 0x30;                 //P0.4 P0.5定义为输出口
    //DS_CO2_20_rest();
}

//=========================================================================
//函数名称：DS_CO2_20_control
//函数参数：cmd:控制命令
//函数返回：命令发送成功：0；命令发送失败：0x11
//功能概要：通过命令控制传感器工作状态
//======================================================================
unsigned char DS_CO2_20_control(void)
{
	unsigned char flag = 0;
        
	flag = HalUARTWrite(0, frame, DS_CO2_20_LENTH_CMD);
        
	//如果发送命令错误，返回错误码
	if(!flag)
	{
		return DS_CO2_20_ERR_CMDFAIL;
	}

	return 0;
}

//=========================================================================
//函数名称：DS_CO2_20_rest
//函数参数：无
//函数返回：无
//功能概要：通过RST引脚复位传感器
//======================================================================
void DS_CO2_20_rest(void)
{
	DS_CO2_20_RST = 0;
        for(int i=0;i<500;i++);
        DS_CO2_20_RST = 1;
}

//======================================================================
//函数名称：DS_CO2_20_datarec
//函数参数：databuf:数据接收缓存
//函数返回：接收成功：0 ；接收失败：0x12
//功能概要：接收传感器原始读数
//======================================================================
unsigned char DS_CO2_20_recdata(unsigned char *databuf)
{
	unsigned char lrcl, lrch, i;
	unsigned int lrcsum = 0, datasum = 0;
        DS_CO2_20_control();
       
	//接收传感器的数据
        i = HalUARTRead(0, databuf, DS_CO2_20_LENTH_DATA);

	//计算校验和
	for(i = 0; i < (DS_CO2_20_LENTH_DATA -2) ; i++)
	{
		datasum += databuf[i];
	}
	lrcl = databuf[DS_CO2_20_LENTH_DATA-1];
	lrch = databuf[DS_CO2_20_LENTH_DATA-2];
	lrcsum = (lrch<<8)+lrcl;
	//如果校验错误，返回错误码
	if(datasum != lrcsum)
        {
		return DS_CO2_20_ERR_RECFAIL;
        }

	return 0;
}

unsigned char DS_CO2_20_Read_Str(unsigned char *buf)
{
  unsigned char tmp[DS_CO2_20_LENTH_DATA];
  unsigned int co2, para1, para2;         //单位ppm; para unuse
  unsigned int cnt = 0;

  while(DS_CO2_20_recdata(tmp) == DS_CO2_20_ERR_RECFAIL)
  {
    if(++cnt > 1000)
      return 1;         //timeout,sensor no response
  }
  co2 = (tmp[4]<<8) + tmp[5];
  para1 = (tmp[6]<<8) + tmp[7];
  para2 = (tmp[8]<<8) + tmp[9];
  (void)para1;
  (void)para2;
      
  if(co2 < 3000)
  {
    sprintf((char *)buf, "%4d", co2);
    return 0;
  }
  return 1;              //should not arrive here
}

unsigned char DS_CO2_20_Read(unsigned char *buf)
{
  unsigned char tmp[DS_CO2_20_LENTH_DATA];
  unsigned int co2, para1, para2;         //单位ppm; para unuse
  unsigned int cnt = 0;

  while(DS_CO2_20_recdata(tmp) == DS_CO2_20_ERR_RECFAIL)
  {
    if(++cnt > 1000)
      return 1;         //timeout,sensor no response
  }
  co2 = (tmp[4]<<8) + tmp[5];
  para1 = (tmp[6]<<8) + tmp[7];
  para2 = (tmp[8]<<8) + tmp[9];
  (void)para1;
  (void)para2;
      
  if(co2 < 3000)
  {
    buf[0] = tmp[4];
    buf[1] = tmp[5];
    return 0;
  }
  return 1;              //should not arrive here
}
