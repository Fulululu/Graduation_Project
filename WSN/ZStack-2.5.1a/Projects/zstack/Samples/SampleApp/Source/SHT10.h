/*
* Copyright (c) 2017,FAFU
* All rights reserved.
*
* File Name£ºSHT10.h
* Abstract£ºSHT10 driver for CC2530
*
* Current Version£º1.0
* Author£ºygf
* Finish Date£º20171028
*
* Superseded Version£º1.0
* Authorship £ºygf
* Finish Date£º20171028
*/ 

#ifndef SHT10_H
#define SHT10_H
/*************************************
            INCLUDE FILE
**************************************/
#include <iocc2530.h>
#include <stdio.h>
#include <math.h>
#include "IIC.h"

/*************************************
            MACRO DEFINITION
**************************************/
#define TEMP  0
#define HUMI  1
#define noACK 0
#define ACK   1
                                //address command r/w
#define STATUS_REG_W 0x06       //  000     0011   0
#define STATUS_REG_R 0x07       //  000     0011   1
#define MEASURE_TEMP 0x03       //  000     0001   1
#define MEASURE_HUMI 0x05       //  000     0010   1
#define SOFTRESET    0x1e       //  000     1111   0

/*************************************
            CUSTOM TYPE
**************************************/
enum {SHT10TEMP, SHT10HUMI};

/*************************************
            GLOBAL VARIABLES
**************************************/

/*Private Variables*/

/*Exported Variables*/

/*********************************************************
                  FUNCTION DELARATION
**********************************************************/
/*Private functions*/
static char s_write_byte(unsigned char value);
static char s_read_byte(unsigned char ack);
static void s_transstart(void);
static void s_connectionreset(void);
char s_softreset(void);
static char s_read_statusreg(unsigned char *p_value, unsigned char *p_checksum);
static char s_write_statusreg(unsigned char *p_value);
static char s_measure(unsigned int *p_value, unsigned char *p_checksum, unsigned char mode);
static void calc_sth10(unsigned int *p_temp ,unsigned int *p_humi,float *p_temp_f,float *p_humi_f);
float calc_dewpoint(float h,float t);
/*Exported functions*/
void SHT10_Init(void);
unsigned char SHT10_Read_Str(unsigned char *buf);
unsigned char SHT10_Read(unsigned char *buf);

#endif