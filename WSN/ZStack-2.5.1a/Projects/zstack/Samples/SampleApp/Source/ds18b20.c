#include"iocc2530.h"
#include"OnBoard.h"
#include "ds18b20.h" 
#include <stdio.h>

#define Ds18b20IO P0_5       //�¶ȴ���������


void DS18B20Delay_us(unsigned int k);
void Ds18b20InputInitial(void);
void Ds18b20OutputInitial(void);
unsigned char Ds18b20Initial(void);
void Ds18b20Write(unsigned char infor);
unsigned char Ds18b20Read(void);


//ʱ��Ƶ��Ϊ32M  ��ͬ�ĳ�����ʱ������Ҫ����
void DS18B20Delay_us(unsigned int k)
{
    Onboard_wait(k);
}

void Ds18b20InputInitial(void)//���ö˿�Ϊ����
{
    P0DIR &= 0xDF;//P05
}

void Ds18b20OutputInitial(void)//���ö˿�Ϊ���
{
    P0DIR |= 0x20;
}

//ds18b20��ʼ��  ��ʼ���ɹ�����0x00��ʧ�ܷ���0x01
unsigned char Ds18b20Initial(void)
{
    unsigned char Status = 0x00;
    unsigned int count = 0;
    unsigned char Flag_1 = 0;
    Ds18b20OutputInitial();
    //Ds18b20IO = 1;      //DQ��λ
    //DS18B20Delay_us(260);  //������ʱ
    Ds18b20IO = 0;      //��Ƭ����DQ����
    DS18B20Delay_us(750);  //��ȷ��ʱ ���� 480us С��960us
    Ds18b20IO = 1;      //�����ͷ�����
    Ds18b20InputInitial();//����IOΪ����
    while((Ds18b20IO != 0)&&(Flag_1 == 0))//�ȴ�ds18b20��Ӧ
    {
        count++;
        DS18B20Delay_us(10);
        if(count > 8000) //300us��ʱ��ds18b20�ڲ�����Լ60us,��Ӧ�ź�ʱ��60-240us
          Flag_1 = 1;
        Status = Ds18b20IO; //0��ʾds18b20��Ӧ��������
    }
    Ds18b20OutputInitial();
    Ds18b20IO = 1;
    DS18B20Delay_us(100);
    return Status;       //���س�ʼ��״̬
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
        Ds18b20IO = 0;      //�������ź�
        DS18B20Delay_us(1); //���ź�ά��ʱ��
        Ds18b20IO = 1;      //�ͷ�����
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

//�¶ȶ�ȡ����
unsigned char ReadDs18B20(void) 
{
    unsigned char V1,V2;   //����ߵ�8λ ����
    unsigned char temp;    //�����¶Ȼ���Ĵ���
    
    Ds18b20Initial();
    Ds18b20Write(0xcc);    // ����������кŵĲ���
    Ds18b20Write(0x44);    // �����¶�ת��
    
    Ds18b20Initial();
    Ds18b20Write(0xcc);    //����������кŵĲ��� 
    Ds18b20Write(0xbe);    //��ȡ�¶ȼĴ����ȣ����ɶ�9���Ĵ����� ǰ���������¶�
    
    V1 = Ds18b20Read();    //��λ
    V2 = Ds18b20Read();    //��λ
    temp = ((V1 >> 4)+((V2 & 0x07)*16)); //ת������ 

    return temp;
}

//�¶ȶ�ȡ���� ��1λС��λ
unsigned char Ds18B20_Readfloat(unsigned char *buf)
{
    unsigned char V1,V2;   //����ߵ�8λ ����
    unsigned int temp;     //�����¶Ȼ���Ĵ���
    float fValue;
    union f2byte{
        float f;
        unsigned char bytes[4];
    }instance;

    if(!Ds18b20Initial())
    {
      Ds18b20Write(0xcc);    // ����������кŵĲ���
      Ds18b20Write(0x44);    // �����¶�ת��
      
      Ds18b20Initial();
      Ds18b20Write(0xcc);    //����������кŵĲ��� 
      Ds18b20Write(0xbe);    //��ȡ�¶ȼĴ����ȣ����ɶ�9���Ĵ����� ǰ���������¶�
      
      V1 = Ds18b20Read();    //��λ
      V2 = Ds18b20Read();    //��λ
      //temp = ((V1 >> 4)+((V2 & 0x07)*16)); //ת������ 
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
    unsigned char low,high;   //����ߵ�8λ ����
    unsigned int temp;     //�����¶Ȼ���Ĵ���
    float fValue;
    if(!Ds18b20Initial())
    {
      Ds18b20Write(0xcc);    // ����������кŵĲ���
      Ds18b20Write(0x44);    // �����¶�ת��
      
      Ds18b20Initial();
      Ds18b20Write(0xcc);    //����������кŵĲ��� 
      Ds18b20Write(0xbe);    //��ȡ�¶ȼĴ����ȣ����ɶ�9���Ĵ����� ǰ���������¶�
      
      low = Ds18b20Read();    //��λ
      high = Ds18b20Read();    //��λ
      //temp = ((V1 >> 4)+((V2 & 0x07)*16)); //ת������ 
      temp=high*0xFF+low;
      fValue = temp*0.0625-4;
      sprintf((char *)buf, "%4.1f", fValue);
      
      return 0;
    }
    else
      return 1;
}