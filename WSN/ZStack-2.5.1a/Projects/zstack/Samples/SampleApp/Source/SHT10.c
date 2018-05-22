/*
* Copyright (c) 2017,FAFU
* All rights reserved.
*
* File Name：SHT10.c
* Abstract：SHT10 driver for CC2530
*
* Current Version：1.0
* Author：ygf
* Finish Date：20171028
*
* Superseded Version：1.0
* Authorship ：ygf
* Finish Date：20171028
*/ 

#include "SHT10.h"

//A0------GND

/*Private functions*/
/******************************************************************* 
      writes a byte on the Sensibus and checks the acknowledge
 *******************************************************************/
static char s_write_byte(unsigned char value)
{
  unsigned char i,error=1;//error=1 in case of no acknowledge
  
  SDAOUT;
  asm("NOP");
  for (i=0x80;i>0;i/=2) //shift bit for masking
  { 
    if (i & value) 
      SDA=1; //masking value with i , write to SENSI-BUS
    else 
      SDA=0;
    asm("NOP"); //observe setup time
    SCL=1; //clk for SENSI-BUS
    Delay_us(5); //pulswith approx. 5 us
    SCL=0;
    asm("NOP"); //observe hold time
  }
  SDA=1; //release SDA-line
  
  SDAIN;
  asm("NOP"); //observe setup time
  SCL=1; //clk #9 for ack 
  asm("NOP");
  error=SDA; //check ack (SDA will be pulled down by SHT11)
  Delay_us(3);
  
  SDAOUT;
  asm("NOP");
  SDA=1;
  SCL=0;
  
  return error; //error=1 in case of no acknowledge
}
/******************************************************************* 
      reads a byte form the Sensibus and gives an acknowledge in case of "ack=1"
 *******************************************************************/
static char s_read_byte(unsigned char ack)
{
  unsigned char i,val=0;
  
  SDAOUT;
  asm("NOP");
  SDA=1; //release SDA-line
  
  SDAIN;
  asm("NOP");
  for (i=0x80;i>0;i/=2) //shift bit for masking
  { 
    SCL=1; //clk for SENSI-BUS
    if (SDA) 
      val=(val | i); //read bit
    else
      val=(val |0x00);
    SCL=0;
    Delay_us(5);
  }
  
  SDAOUT;
  asm("NOP");
  SDA=!ack; //in case of "ack==1" pull down SDA-Line
  asm("NOP"); //observe setup time
  SCL=1; //clk #9 for ack
  Delay_us(5); //pulswith approx. 5 us
  SCL=0;
  asm("NOP"); //observe hold time
  SDA=1; //release SDA-line
  return val;
}

/******************************************************************* 
// generates a transmission start
// _____ ________
// SDA: |_______|
// ___ ___
// SCL : ___| |___| |______
 *******************************************************************/
static void s_transstart(void)
{
  SDAOUT;
  asm("NOP");
  SDA=1; 
  SCL=0; //Initial state
  Delay_us(2);
  SCL=1;
  Delay_us(2);
  SDA=0;
  Delay_us(2);
  SCL=0;
  Delay_us(5);
  SCL=1;
  Delay_us(2);
  SDA=1;
  Delay_us(2);
  SCL=0;
  Delay_us(2);
}

/******************************************************************* 
// communication reset: SDA-line=1 and at least 9 SCL cycles followed by transstart
// _____________________________________________________ ________
// SDA: |_______|
// _ _ _ _ _ _ _ _ _ ___ ___
// SCL : __| |__| |__| |__| |__| |__| |__| |__| |__| |______| |___| |______
 *******************************************************************/
static void s_connectionreset(void)
{
  unsigned char i;
  
  SDAOUT;
  asm("NOP");
  SDA=1; 
  SCL=0; //Initial state
  for(i=0;i<9;i++) //9 SCL cycles
  { 
    SCL=1;
    Delay_us(2);
    SCL=0;
    Delay_us(2);
  }
  s_transstart(); //transmission start
}

/******************************************************************* 
            resets the sensor by a softreset
 *******************************************************************/
char s_softreset(void)
{
  unsigned char error=0;
  s_connectionreset(); //reset communication
  error+=s_write_byte(SOFTRESET); //send RESET-command to sensor
  return error; //error=1 in case of no response form the sensor
}

/******************************************************************* 
      makes a measurement (humidity/temperature) with checksum
 *******************************************************************/
static char s_measure(unsigned int *p_value, unsigned char *p_checksum, unsigned char mode)
{
  unsigned char error=0;
  unsigned int i,value_H,value_L;
  s_transstart(); //transmission start
  switch(mode)
  { //send command to sensor
    case SHT10TEMP : error+=s_write_byte(MEASURE_TEMP); break;
    case SHT10HUMI : error+=s_write_byte(MEASURE_HUMI); break;
    default : break;
  }
  
  SDAIN;
  asm("NOP");
  for (i=0;i<65535;i++) 
  {
    if(SDA==0) break; //wait until sensor has finished the measurement
    Delay_us(2);
  }
  if(SDA) 
    error+=1; // or timeout (~2 sec.) is reached
  
  value_H =s_read_byte(ACK); //read the first byte (MSB)
  value_L=s_read_byte(ACK); //read the second byte (LSB)
  *p_checksum =s_read_byte(noACK); //read checksum
  *p_value = (value_H << 8) | value_L; 
  
  return error;
}

/***********************************************************************
// calculates temperature [°C] and humidity [%RH]
// input : humi [Ticks] (12 bit)
// temp [Ticks] (14 bit)
// output: humi [%RH]
// temp [°C]
 ***********************************************************************/
static void calc_sth10(unsigned int *p_temp ,unsigned int *p_humi,float *p_temp_f,float *p_humi_f)
{ 
    const float d1 = -44;  
    const float d2 = +0.01;  
    const float C1 = -2.0468;  
    const float C2 = +0.0367;  
    const float C3 = -0.0000015955;   
    const float T1 = +0.01;  
    const float T2 = +0.00008;  
  
    unsigned int t=*p_temp;
    unsigned int rh=*p_humi;
    float RH_Lin;                                       //RH线性值   
    float RH_Ture;                                      //RH真实值  
    float temp_C;  
  
    temp_C = d1 + d2 * t;                               //计算温度值   
    RH_Lin = C1 + C2 * rh + C3 * rh * rh;               //计算湿度值  
    RH_Ture = (temp_C -25) * (T1 + T2 * rh) + RH_Lin;   //湿度的温度补偿，计算实际的湿度值  
  
    if(RH_Ture > 100)                                    //设置湿度值上限  
        RH_Ture = 100;  
    if(RH_Ture < 0.1)  
        RH_Ture = 0.1;                                  //设置湿度值下限  
  
    *p_humi_f = RH_Ture;  
    *p_temp_f = temp_C; 
}

/********************************************************************
// calculates dew point
// input: humidity [%RH], temperature [°C]
// output: dew point [°C]
 ********************************************************************/
float calc_dewpoint(float h,float t)
{ 
  float k,dew_point ;
  k = (log10(h)-2)/0.4343 + (17.62*t)/(243.12+t);
  dew_point = 243.12*k/(17.62-k);
  return dew_point;
}

/*Exported functions*/

void SHT10_Init()
{
  s_connectionreset();
}

unsigned char SHT10_Read_Str(unsigned char *buf)
{
  unsigned int humi_val,temp_val;
  float humi_val_real,temp_val_real;
  //float dew_point;
  unsigned char error,checksum,cnt,timeout=0;
  
  do
  {
    error=0;
    error+=s_measure(&temp_val,&checksum,SHT10TEMP); //measure temperature
    error+=s_measure(&humi_val,&checksum,SHT10HUMI); //measure humidity
    if(error !=0 ) 
    {
      s_connectionreset(); //in case of an error: connection reset
      if(++timeout >= 10)
      {
        return 1;         //timeout,sensor no response
      }
    }
  }while(error != 0);
  
  calc_sth10(&temp_val,&humi_val,&temp_val_real,&humi_val_real); //calculate humidity,temperature
  //dew_point=calc_dewpoint(humi_val.f,temp_val.f); //calculate dew point
  cnt = sprintf((char*)buf, "%2.1f %2.1f", temp_val_real, humi_val_real);
  if(cnt == 9) buf[9]=' ';
  return 0;
}

unsigned char SHT10_Read(unsigned char *buf)
{
  unsigned int humi_val,temp_val;
  float humi_val_real,temp_val_real;
  //float dew_point;
  unsigned char error,checksum,timeout=0;
  union f2byte{
    float f;
    unsigned char bytes[4];
  }instance;
  
  do
  {
    error=0;
    error+=s_measure(&temp_val,&checksum,SHT10TEMP); //measure temperature
    error+=s_measure(&humi_val,&checksum,SHT10HUMI); //measure humidity
    if(error !=0 ) 
    {
      s_connectionreset(); //in case of an error: connection reset
      if(++timeout >= 10)
      {
        return 1;         //timeout,sensor no response
      }
    }
  }while(error != 0);
  
  calc_sth10(&temp_val,&humi_val,&temp_val_real,&humi_val_real); //calculate humidity,temperature
  //dew_point=calc_dewpoint(humi_val.f,temp_val.f); //calculate dew point
  instance.f = temp_val_real;
  buf[0] = (unsigned char)instance.bytes[3];
  buf[1] = (unsigned char)instance.bytes[2];
  buf[2] = (unsigned char)instance.bytes[1];
  buf[3] = (unsigned char)instance.bytes[0];
 
  buf[4] = (unsigned char)humi_val_real;
  //buf[4] = (unsigned long)humi_val_real >> 24;
  //buf[5] = (unsigned long)humi_val_real >> 16;
  //buf[6] = (unsigned long)humi_val_real >> 8;
  //buf[7] = (unsigned long)humi_val_real;
  
  return 0;
}