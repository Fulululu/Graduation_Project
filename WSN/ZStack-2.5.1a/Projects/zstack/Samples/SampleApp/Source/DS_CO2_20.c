//=====================================================================
//�ļ����ƣ�DS_CO2_20.c
//���ܸ�Ҫ��CO2������DS_CO2_20����Դ�ļ�
//=====================================================================

#include "DS_CO2_20.h"

unsigned char frame[] = {0x42, 0x4D, 0xE3, 0x00, 0x00 , 0x01, 0x72};

//RST------3.3V

//=====================================================================
//����������Э�飺
//�����ֽ�  �����ֽ�  ָ���ֽ�  ״̬�ֽ�1 ״̬�ֽ�2 У���ֽ�1 У���ֽ�2
// 0x42      0x4d      CMD     DATAH      DATAL    LRCH       LRCL
//ָ������ֽڵĶ��壺
//CMD  DATAH  DATAL      			˵��
//0xe3   x      x        		 ��������
//У�������ɣ�
//�������ֿ�ʼ�����ֽ��ۼӺ�
//=====================================================================

//=========================================================================
//�������ƣ�DS_CO2_20_init
//������������
//�������أ���
//���ܸ�Ҫ����ʼ��SET��RST����
//======================================================================
void DS_CO2_20_Init(void)
{
    P0SEL &= ~0x30;                 //����P0.4 P0.5Ϊ��ͨIO��
    P0DIR |= 0x30;                 //P0.4 P0.5����Ϊ�����
    //DS_CO2_20_rest();
}

//=========================================================================
//�������ƣ�DS_CO2_20_control
//����������cmd:��������
//�������أ�����ͳɹ���0�������ʧ�ܣ�0x11
//���ܸ�Ҫ��ͨ��������ƴ���������״̬
//======================================================================
unsigned char DS_CO2_20_control(void)
{
	unsigned char flag = 0;
        
	flag = HalUARTWrite(0, frame, DS_CO2_20_LENTH_CMD);
        
	//�������������󣬷��ش�����
	if(!flag)
	{
		return DS_CO2_20_ERR_CMDFAIL;
	}

	return 0;
}

//=========================================================================
//�������ƣ�DS_CO2_20_rest
//������������
//�������أ���
//���ܸ�Ҫ��ͨ��RST���Ÿ�λ������
//======================================================================
void DS_CO2_20_rest(void)
{
	DS_CO2_20_RST = 0;
        for(int i=0;i<500;i++);
        DS_CO2_20_RST = 1;
}

//======================================================================
//�������ƣ�DS_CO2_20_datarec
//����������databuf:���ݽ��ջ���
//�������أ����ճɹ���0 ������ʧ�ܣ�0x12
//���ܸ�Ҫ�����մ�����ԭʼ����
//======================================================================
unsigned char DS_CO2_20_recdata(unsigned char *databuf)
{
	unsigned char lrcl, lrch, i;
	unsigned int lrcsum = 0, datasum = 0;
        DS_CO2_20_control();
       
	//���մ�����������
        i = HalUARTRead(0, databuf, DS_CO2_20_LENTH_DATA);

	//����У���
	for(i = 0; i < (DS_CO2_20_LENTH_DATA -2) ; i++)
	{
		datasum += databuf[i];
	}
	lrcl = databuf[DS_CO2_20_LENTH_DATA-1];
	lrch = databuf[DS_CO2_20_LENTH_DATA-2];
	lrcsum = (lrch<<8)+lrcl;
	//���У����󣬷��ش�����
	if(datasum != lrcsum)
        {
		return DS_CO2_20_ERR_RECFAIL;
        }

	return 0;
}

unsigned char DS_CO2_20_Read_Str(unsigned char *buf)
{
  unsigned char tmp[DS_CO2_20_LENTH_DATA];
  unsigned int co2, para1, para2;         //��λppm; para unuse
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
  unsigned int co2, para1, para2;         //��λppm; para unuse
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
