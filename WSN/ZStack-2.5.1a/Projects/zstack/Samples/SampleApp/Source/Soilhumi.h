/*
* Copyright (c) 2017,FAFU
* All rights reserved.
*
* File Name��Soilhumi.h
* Abstract��Soilhumi driver for CC2530
*
* Current Version��1.0
* Author��ygf
* Finish Date��20171202
*
* Superseded Version��1.0
* Authorship ��ygf
* Finish Date��20171202
*/ 

#ifndef SOILHUMI_H
#define SOILHUMI_H
/*************************************
            INCLUDE FILE
**************************************/
#include <iocc2530.h>
#include "hal_adc.h"
#include <stdio.h>

/*************************************
            MACRO DEFINITION
**************************************/

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
void Soilhumi_Init(void);
unsigned char Soilhumi_Read_Str(unsigned char *buf);
unsigned char Soilhumi_Read(unsigned char *buf);

#endif