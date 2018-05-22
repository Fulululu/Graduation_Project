//=====================================================================
//�ļ����ƣ�PMS7003.c
//���ܸ�Ҫ�����������ﴫ������PM2.5��PMS7003����Դ�ļ�
//=====================================================================

#include "PMS7003.h"

//SET------3.3V
//RST------3.3V

//=====================================================================
//����������Э�飺
//�����ֽ�  �����ֽ�  ָ���ֽ�  ״̬�ֽ�1 ״̬�ֽ�2 У���ֽ�1 У���ֽ�2
// 0x42      0x4d      CMD      DATAH     DATAL    LRCH      LRCL
//ָ������ֽڵĶ��壺
//CMD  DATAH  DATAL      	  ˵��
//0xe2   x      x        	 ��������
//0xe1   x    00H-����ʽ         ״̬�л�
//            01H-����ʽ
//0xe4   x    00H-����ģʽ       ��������
//       x    01H-����ģʽ
//У�������ɣ�
//�������ֿ�ʼ�����ֽ��ۼӺ�
//=====================================================================

//=========================================================================
//�������ƣ�PMS7003_init
//������������
//�������أ���
//���ܸ�Ҫ����ʼ��SET��RST����
//======================================================================
void PMS7003_Init(void)
{
    P0SEL &= ~0x30;                 //����P0.4 P0.5Ϊ��ͨIO��
    P0DIR |= 0x30;                 //P0.4 P0.5����Ϊ�����
    //PMS7003_rest();
    //PMS7003_SET = 1;
    PMS7003_control(PMS7003_CMD_READSTATE_PASSTIVE);
}

//=========================================================================
//�������ƣ�PMS7003_control
//����������cmd:��������
//�������أ�����ͳɹ���0�������ʧ�ܣ�0x11
//���ܸ�Ҫ��ͨ��������ƴ���������״̬
//======================================================================
unsigned char PMS7003_control(unsigned int cmd)
{
	unsigned char flag;
	//��ʼ������֡
	unsigned char command[PMS7003_LENTH_CMD] = {0};

	PMS7003_buildframe(cmd,command);
	flag = HalUARTWrite(0, command, PMS7003_LENTH_CMD);

	//�������������󣬷��ش�����
	if(!flag)
	{
		return PMS7003_ERR_CMDFAIL;
	}

	return 0;
}

//=========================================================================
//�������ƣ�PMS7003_buildframe
//����������cmd:��������
//		 *frame������֡����
//�������أ���
//���ܸ�Ҫ���������������֡
//======================================================================
void PMS7003_buildframe(unsigned int cmd_state,unsigned char *frame)
{
	unsigned char cmd;
	unsigned char statechar;
	unsigned char lrcl;
	unsigned char lrch;
	unsigned int lrc = 0;

	//���������״̬�ֽ�
	cmd = cmd_state >> 8;
	statechar = cmd_state;

	//��֡
	frame[0] = 0x42;
	frame[1] = 0x4d;
	frame[2] = cmd;
	frame[3] = 0x00;
	frame[4] = statechar;

	//����У���� ��
	unsigned char i;
	for(i = 0 ; i < (PMS7003_LENTH_CMD-2) ; i++)
	{
		lrc += frame[i];
	}
	lrch = lrc >> 8;
	lrcl = lrc;
	//��֡
	frame[5] = lrcl;
	frame[6] = lrch;
}

//=========================================================================
//�������ƣ�PMS7003_rest
//������������
//�������أ���
//���ܸ�Ҫ��ͨ��RST���Ÿ�λ������
//======================================================================
void PMS7003_rest(void)
{
	PMS7003_RST = 0;
        for(int i=0;i<500;i++);
        PMS7003_RST = 1;
}

//=========================================================================
//�������ƣ�PMS7003_sleep
//������������
//�������أ���
//���ܸ�Ҫ��ͨ��SET�������ߴ�����
//======================================================================
void PMS7003_sleep(void)
{
	PMS7003_SET = 0;
}

//======================================================================
//�������ƣ�PMS7003_datarec
//����������databuf:���ݽ��ջ���
//�������أ����ճɹ���0 ������ʧ�ܣ�0x12
//���ܸ�Ҫ�����մ�����ԭʼ����
//======================================================================
unsigned char PMS7003_recdata(unsigned char *databuf)
{
	unsigned char lrcl, lrch, i;
	unsigned int lrcsum, datasum = 0;
        PMS7003_control(PMS7003_CMD_PSSTIVEREAD);
	//���մ�����������
        i=HalUARTRead(0, databuf, PMS7003_LENTH_DATA);

	//����У���
	for(i = 0; i < (PMS7003_LENTH_DATA -2) ; i++)
	{
		datasum += databuf[i];
	}
	lrcl = databuf[PMS7003_LENTH_DATA-1];
	lrch = databuf[PMS7003_LENTH_DATA-2];
	lrcsum = (lrch<<8)+lrcl;
	//���У����󣬷��ش�����
	if(datasum != lrcsum)
        {
		return PMS7003_ERR_RECFAIL;
        }

	return 0;
}

unsigned char PMS7003_Read_Str(unsigned char *buf)
{
  unsigned char tmp[PMS7003_LENTH_DATA];
  unsigned int pm1, pm25, pm10;//CF=1����׼�������λug/m3
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
  unsigned int pm1, pm25, pm10;//CF=1����׼�������λug/m3
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

