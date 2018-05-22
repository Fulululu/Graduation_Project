#include"iocc2530.h"
#include"OnBoard.h"
#include "ds18b20.h" 
#include <stdio.h>

#define Ds18b20IO P0_5       //温度传感器引脚


void DS18B20Delay_us(unsigned int k);
void Ds18b20InputInitial(void);
void Ds18b20OutputInitial(void);
unsigned char Ds18b20Initial(void);
void Ds18b20Write(unsigned char infor);
unsigned char Ds18b20Read(void);


//时钟频率为32M  不同的厂家延时可能需要调整
void DS18B20Delay_us(unsigned int k)
{
    Onboard_wait(k);
}

void Ds18b20InputInitial(void)//设置端口为输入
{
    P0DIR &= 0xDF;//P05
}

void Ds18b20OutputInitial(void)//设置端口为输出
{
    P0DIR |= 0x20;
}

//ds18b20初始化  初始化成功返回0x00，失败返回0x01
unsigned char Ds18b20Initial(void)
{
    unsigned char Status = 0x00;
    unsigned int count = 0;
    unsigned char Flag_1 = 0;
    Ds18b20OutputInitial();
    //Ds18b20IO = 1;      //DQ复位
    //DS18B20Delay_us(260);  //稍做延时
    Ds18b20IO = 0;      //单片机将DQ拉低
    DS18B20Delay_us(750);  //精确延时 大于 480us 小于960us
    Ds18b20IO = 1;      //拉高释放总线
    Ds18b20InputInitial();//设置IO为输入
    while((Ds18b20IO != 0)&&(Flag_1 == 0))//等待ds18b20响应
    {
        count++;
        DS18B20Delay_us(10);
        if(count > 8000) //300us超时，ds18b20内部处理约60us,响应信号时长60-240us
          Flag_1 = 1;
        Status = Ds18b20IO; //0表示ds18b20响应拉低总线
    }
    Ds18b20OutputInitial();
    Ds18b20IO = 1;
    DS18B20Delay_us(100);
    return Status;       //返回初始化状态
}

void Ds18b20Write(unsigned char infor)
{
    unsigned int i;
    Ds18b20OutputInitial();
    for(i=0;i<8;i++)
    {
        if((infor & 0x01))
        {
            Ds18b20IO = 0;
            DS18B20Delay_us(1);
            Ds18b20IO = 1;
            DS18B20Delay_us(60);
        }
        else
        {
            Ds18b20IO = 0;
            DS18B20Delay_us(1);
            DS18B20Delay_us(60);
            Ds18b20IO = 1;
            DS18B20Delay_us(1);
        }
        infor >>= 1;
    }
}

unsigned char Ds18b20Read(void)
{
    unsigned char Value = 0x00;
    unsigned int i;
    
    for(i=0;i<8;i++)
    {   
        Ds18b20OutputInitial();
        Ds18b20IO = 0;      //发出读信号
        DS18B20Delay_us(1); //读信号维持时间
        Ds18b20IO = 1;      //释放总线
        Ds18b20InputInitial();
        
        Value >>= 1;
        if(Ds18b20IO == 1)
        {
          Value |=0x80;
        }
        else
        {
          Value |=0x00;
        }
        DS18B20Delay_us(15);
    } 
    return Value;
}

//温度读取函数
unsigned char ReadDs18B20(void) 
{
    unsigned char V1,V2;   //定义高低8位 缓冲
    unsigned char temp;    //定义温度缓冲寄存器
    
    Ds18b20Initial();
    Ds18b20Write(0xcc);    // 跳过读序号列号的操作
    Ds18b20Write(0x44);    // 启动温度转换
    
    Ds18b20Initial();
    Ds18b20Write(0xcc);    //跳过读序号列号的操作 
    Ds18b20Write(0xbe);    //读取温度寄存器等（共可读9个寄存器） 前两个就是温度
    
    V1 = Ds18b20Read();    //低位
    V2 = Ds18b20Read();    //高位
    temp = ((V1 >> 4)+((V2 & 0x07)*16)); //转换数据 

    return temp;
}

//温度读取函数 带1位小数位
unsigned char Ds18B20_Readfloat(unsigned char *buf)
{
    unsigned char V1,V2;   //定义高低8位 缓冲
    unsigned int temp;     //定义温度缓冲寄存器
    float fValue;
    union f2byte{
        float f;
        unsigned char bytes[4];
    }instance;

    if(!Ds18b20Initial())
    {
      Ds18b20Write(0xcc);    // 跳过读序号列号的操作
      Ds18b20Write(0x44);    // 启动温度转换
      
      Ds18b20Initial();
      Ds18b20Write(0xcc);    //跳过读序号列号的操作 
      Ds18b20Write(0xbe);    //读取温度寄存器等（共可读9个寄存器） 前两个就是温度
      
      V1 = Ds18b20Read();    //低位
      V2 = Ds18b20Read();    //高位
      //temp = ((V1 >> 4)+((V2 & 0x07)*16)); //转换数据 
      temp=V2*0xFF+V1;
      fValue = temp*0.0625-4;
      
      instance.f = fValue;
      buf[0] = (unsigned char)instance.bytes[3];
      buf[1] = (unsigned char)instance.bytes[2];
      buf[2] = (unsigned char)instance.bytes[1];
      buf[3] = (unsigned char)instance.bytes[0];
      
      return 0;
    }
    else
      return 1;
}

unsigned char Ds18B20_Readfloat_Str(unsigned char *buf)
{
    unsigned char low,high;   //定义高低8位 缓冲
    unsigned int temp;     //定义温度缓冲寄存器
    float fValue;
    if(!Ds18b20Initial())
    {
      Ds18b20Write(0xcc);    // 跳过读序号列号的操作
      Ds18b20Write(0x44);    // 启动温度转换
      
      Ds18b20Initial();
      Ds18b20Write(0xcc);    //跳过读序号列号的操作 
      Ds18b20Write(0xbe);    //读取温度寄存器等（共可读9个寄存器） 前两个就是温度
      
      low = Ds18b20Read();    //低位
      high = Ds18b20Read();    //高位
      //temp = ((V1 >> 4)+((V2 & 0x07)*16)); //转换数据 
      temp=high*0xFF+low;
      fValue = temp*0.0625-4;
      sprintf((char *)buf, "%4.1f", fValue);
      
      return 0;
    }
    else
      return 1;
}