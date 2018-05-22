/*
* Copyright (c) 2017,FAFU
* All rights reserved.
*
* File Name：Soilhumi.c
* Abstract：Soilhumi driver for CC2530
*
* Current Version：1.0
* Author：ygf
* Finish Date：20171202
*
* Superseded Version：1.0
* Authorship ：ygf
* Finish Date：20171202
*/ 

#include "Soilhumi.h"
#include "OnBoard.h"

unsigned int pos_zero=0;
unsigned int pos_100 = 4300;
void Soilhumi_Init(void)
{
  unsigned char i;
  unsigned long temp=0;
  pos_zero = 0;
  for(i=0; i<100; i++)
  {
    temp += HalAdcRead(HAL_ADC_CHANNEL_4, HAL_ADC_RESOLUTION_14);
  }
  pos_zero = temp/100;
}

unsigned char Soilhumi_Read_Str(unsigned char *buf)
{
  unsigned int humi = 0, cnt=0;
  unsigned long sum = 0;
  double p1=-0.000006681,p2=0.01309,p3=-8.528,p4=1863;//为计算时减少计算量，拟合时输入缩小了10倍
  
  do
  {
    humi = HalAdcRead(HAL_ADC_CHANNEL_4, HAL_ADC_RESOLUTION_14); //ref Vol = AVDD5(3.3V)
    sum += humi;
    cnt++;
  }while(cnt < 100);
  
  if(cnt == 100)
  {
    sum/=100;
    if(sum > pos_zero)
      humi = 0;
    else
    {
      sum/=10;             //相应的计算时也要缩小10倍
      humi = (unsigned int)(p1*sum*sum*sum + p2*sum*sum + p3*sum + p4);
    }
    if(humi > 100)
      humi = 100;
    
    sprintf((char *)buf, "%3d", humi);
    return 0;
  }
  return 1;
}

unsigned char Soilhumi_Read(unsigned char *buf)
{
  unsigned int humi = 0, cnt=0;
  unsigned long sum = 0;
  double p1=-0.000006681,p2=0.01309,p3=-8.528,p4=1863;//为计算时减少计算量，拟合时输入缩小了10倍
  
  do
  {
    humi = HalAdcRead(HAL_ADC_CHANNEL_4, HAL_ADC_RESOLUTION_14); //ref Vol = AVDD5(3.3V)
    sum += humi;
    cnt++;
  }while(cnt < 100);
  
  if(cnt == 100)
  {
    sum/=100;
    if(sum > pos_zero)
      humi = 0;
    else
    {
      sum/=10;             //相应的计算时也要缩小10倍
      humi = (unsigned int)(p1*sum*sum*sum + p2*sum*sum + p3*sum + p4);
    }
    if(humi > 100)
      humi = 100;
    
    buf[0] = (unsigned char)humi;
    return 0;
  }
  return 1;
}