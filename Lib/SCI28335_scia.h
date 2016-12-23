#include "DSP2833x_Device.h"
/*
SCIStart˫��RAM�б�ʶ2407�Ѿ����ܵ�PC���Ĵ����жϣ�1������ܵ������жϣ�2������Ҫ��ɲ����������޸�
Add_Edit	�����޸Ĳ�����ַ
Add_Val		�����޸Ĳ���ֵ
Add_Channel	4��ͨ���ĵ�ַ�����δ洢
Val_Channel	4��ͨ����ֵ�����δ洢
*/

/*#define SCIStart 	(unsigned int *)0x8020
#define Add_Edit 	(unsigned int *)0x8021
#define Add_Val 	(unsigned int *)0x8022
#define Add_Channel 	(unsigned int *)0x8023
#define Val_Channel 	(unsigned int *)0x8033
*/
unsigned int SCIStart;
unsigned int *Add_Edit;
int Add_Val = 0;
unsigned int Add_Channel[4];
unsigned int Val_Channel[4];
Uint16 *ExRamZ2Start=(Uint16*)0x230000;
int break_time=0,k1 = 1,kk1 = 0;   //�ֺ�բ�Ĵ���


/*
�����ж��б�������
*/
long tmp1_wr;
unsigned  int	buffer_length = 24120;//24120;
unsigned  int	BufPointer_PWM;				/*savebuffer��ʹ�ã���ʶ�����ĸ�ͨ��������BUFFERDATA��λ��*/
unsigned  int	BufPointer_SCI;				/*TXINT�б�ʶ����������BUFFERDATA��λ��*/
unsigned  int	FramePointer;				/*��ʶÿ֡�������ݷ��͵�λ��*/
unsigned  int	EnableWrite;				/*��ʶ�Ƿ��ĸ�ͨ����ǰֵ����BUFFERDATA��*/
  int	send_char;				/*�洢��Ҫ���͵���*/
unsigned  int	received_char;				/*�洢���յ���*/
unsigned  int	in_frame;				/*��ʶ�Ƿ���յ�����ͷ��1 ������յ�*/
unsigned  int	bytecount;				/*��ʶ�Ѿ����յ�PC��һ�����ݵ�λ��*/
unsigned  int	ch1_addr;				/*Channel 1 ��ַ*/
unsigned  int	ch2_addr;				/*Channel 2 ��ַ*/
unsigned  int	ch3_addr;				/*Channel 3 ��ַ*/
unsigned  int	ch4_addr;				/*Channel 4 ��ַ*/
unsigned  int	edit_addr;				/*���յ��ر����������޸ĵر����ص�ַ*/
  int	edit_val;				/*���յ��±����������޸ı������޸�ֵ*/
unsigned  int	buffer[24];				/*�洢���յ���PC����һ������*/
unsigned  int	addr_last;				/*�洢�ϴ��޸ĵر����ص�ַ*/
  int	val_last;				/*�洢�ϴ��޸ĵر�����ֵ*/
unsigned  int	SampleCounter;				/*��ʶ����������֮��������*/
unsigned  int	Clock;					/*��ʶʱ�䣬ʹ���ڼ��㵥�δ�����ʼʱ��*/
unsigned  int	SampleTime;				/*��������*/
unsigned  int	MultiSingle;				/*��ʶ��Ρ����δ�����1 ������*/
unsigned  int	StartTime;				/*��ʶ���δ�����ʼʱ��*/
//unsigned  int	BUFFERDATA[160];			/*�ĸ�ͨ��ֵ�洢����*/
unsigned  int	change_online;				/*�����޸ı���ʹ�ã���ʶ�Ѿ�֪ͨC33 �б����޸ı���ʱ�䣬2���ж�*/
unsigned  int	S01_K = 1000;				/*���δ������㿪ʼʱ��ʹ�ã�����0.1s*/
unsigned  int	S01;					/*���δ������㿪ʼʱ��*/  
unsigned  int   storePIVR;

//unsigned  int   timess;                  /*calculat interrupt times*/

#pragma DATA_SECTION(BUFFERDATA,"data");
//unsigned  int   BUFFERDATA[24120];
unsigned  int   BUFFERDATA[24];

extern int Flag_Scia_interrupt;
/*
����Ϊ2407�����
*/

/*====================================���ڳ�ʼ������=========================================*/
/*		���ļ�DSP28Sci.c���ļ�interrupt.c��ʵ��,���ļ�aci_time.c�����ú͵���	     */
/*===========================================================================================*/


void	DATATODUALRAM(void)				/*��RXINT���յ�ȫ�����ݺ���ã��������͸�C33*/
{
	*Add_Channel = ch1_addr;
	*(Add_Channel+1) = ch2_addr;
	*(Add_Channel+2) = ch3_addr;
	*(Add_Channel+3) = ch4_addr; 
//	timess = timess + 1;				/*calculate interrupt times*/
}

void	SCOPE_INT(void)
{
	edit_addr = 0x0000;
	addr_last = 0x0000;
	edit_val = 0x0000;
	val_last = 0x0000;
	ch1_addr = 0x0000;
	ch2_addr = 0x0000;
	ch3_addr = 0x0000;
	ch4_addr = 0x0000;
	Clock = 0x0000;					
	MultiSingle = 0x0001;
	SampleTime = 0x0001;
	StartTime = 0x0001;
	SCIStart = 1;
//	timess = 0;                          /*time*/
}

void 	SCOPE_RST(void)
{
	BufPointer_SCI = 0x0000;			/*��ʶSCITX�з���λ��*/
	FramePointer = 0x0000;
	EnableWrite = 0x0001;
	SampleCounter = 0x0000;
	in_frame = 0x0000;
	bytecount = 0x0000;
	change_online = 0x0000;
	BufPointer_PWM = 0x0000;
	S01 = 0;
}

/*================================Scope���н����ӳ���=======================================*/

interrupt void scia_rx_isr(void)
{
	received_char = SciaRegs.SCIRXBUF.all;
	if(received_char == 0x00FA)			/*�ж�����ͷ*/
	{
		in_frame = 0x0001;
		bytecount = 0x0000;
	}
	else
	{
		if(in_frame == 0x0000)			/*�ڽ��յ�����ͷ��ʱ���ִ��else�еس���*/
		{}
		else
		{
			buffer[bytecount] = received_char;
			bytecount = bytecount + 0x0001;
		}
		
		if(bytecount == 24)				/*����س�����Ҫȷ��PC�������������ݸ�ʽ����ȷ��*/
		{
			asm(" CLRC SXM");
			in_frame = 0x0000;
			ch1_addr = buffer[0];
			ch1_addr = ch1_addr + (buffer[1]<<4);
			ch1_addr = ch1_addr + (buffer[2]<<8);
			ch1_addr = ch1_addr + (buffer[3]<<12);
			
			ch2_addr = buffer[4];
			ch2_addr = ch2_addr + (buffer[5]<<4);
			ch2_addr = ch2_addr + (buffer[6]<<8);
			ch2_addr = ch2_addr + (buffer[7]<<12);
			
			ch3_addr = buffer[8];
			ch3_addr = ch3_addr + (buffer[9]<<4);
			ch3_addr = ch3_addr + (buffer[10]<<8);
			ch3_addr = ch3_addr + (buffer[11]<<12);
			
			ch4_addr = buffer[12];
			ch4_addr = ch4_addr + (buffer[13]<<4);
			ch4_addr = ch4_addr + (buffer[14]<<8);
			ch4_addr = ch4_addr + (buffer[15]<<12);
			
			addr_last = edit_addr;
			
			edit_addr = buffer[16];
			edit_addr = edit_addr + (buffer[17]<<4);
			edit_addr = edit_addr + (buffer[18]<<8);
			edit_addr = edit_addr + (buffer[19]<<12);

			val_last = edit_val;
			
			edit_val = buffer[20];
			edit_val = edit_val + (buffer[21]<<4);
			edit_val = edit_val + (buffer[22]<<8);
			edit_val = edit_val + (buffer[23]<<12);
			
			if(edit_addr == 0xFF2A)							/*Ӧ�öԷ���������Ϊ����ʱ����һ���Ĵ���*/
			{
				if(addr_last != edit_addr || val_last != edit_val)
				{
					SampleTime = edit_val;
					SCOPE_RST();
					Clock = 0;
				}
			}
			else
			{
				if(edit_addr == 0xFF2B)
				{
					if(addr_last != edit_addr || val_last != edit_val)
					{
						MultiSingle = edit_val;
						SCOPE_RST();
						Clock = 0;
					}
				}
				else
				{
					if(edit_addr == 0xFF2C)
					{
						if(addr_last != edit_addr || val_last != edit_val)
						{
							StartTime = edit_val;
							SCOPE_RST();
							Clock = 0;
						}
					}
					else
					{
						if(addr_last != edit_addr || val_last != edit_val)		/*��Ҫ���SCIStart �� 2�����������ڵر�ʶ����*/
						{   
							if(edit_addr == 0 && edit_val == 0)
							{}
							else
							{
								*Add_Edit = edit_addr;
								Add_Val = edit_val;
								SCIStart = 2;
								change_online = 2;
								}
						}
					}
				}
			}

			
			asm(" SETC SXM");
			
			DATATODUALRAM();				/*���ĸ�ͨ���ص�ַ�͵�C33*/
		}
		else
		{}
	}
	 PieCtrlRegs.PIEACK.bit.ACK9=1;
}

/*=====================Scope���з����ӳ���======================================*/	
/*Frame�ṹ:	head;   (data1;data2;data3;data4);(data1;data2;data3;data4);	*/
/*			(data1;data2;data3;data4);			 	*/
/*		��26���ֽ�.�ֱ��ӦFramePointer��0��25.			 	*/
/*==============================================================================*/

interrupt void scia_tx_isr(void)
{

	if(BufPointer_SCI == buffer_length && FramePointer%2 == 0)			/*FramePointer = 26*/
	{
		if(MultiSingle == 1)
		{
			EnableWrite = 0x0001;
		}
		BufPointer_SCI = 0x0000;

	}
	else
	{
		if(FramePointer >= 26)							/*֡������FramePointer = 0*/
		{
			FramePointer = 0;
		}
		
		if(FramePointer < 2)							/*��������ͷ*/
		{
			if(FramePointer == 0)
			{

				SciaRegs.SCITXBUF = 0x00FF;	//
			}
			if(FramePointer == 1)
			{
				
				SciaRegs.SCITXBUF = 0x007F;//
			}
			FramePointer = FramePointer + 1;
		}
		else
		{
			if(FramePointer % 2 == 0)
			{
				send_char = *(ExRamZ2Start+tmp1_wr+Val_Channel[kk1]);
				//send_char = 10;/////////for test
				BufPointer_SCI = BufPointer_SCI + 1;
				kk1++;
				if(kk1 == 4)
				{
					kk1 = 0;
					k1++;
					if(k1 >= 2000) {k1=1;}
					tmp1_wr=10*k1+(long)(break_time)*(long)20000;
				}
				SciaRegs.SCITXBUF = send_char&0x00FF;				/*�������ݵ�λ�ֽں͸�λ�ֽ�Ҫ����д��ȷ��һ��*/

			}
			else
			{
			
				SciaRegs.SCITXBUF = (send_char&0xFF00)>>8;			/*���͸�λ�ֽ�*/

			}
			FramePointer = FramePointer + 1;
				
		}
		
	}                              
	 PieCtrlRegs.PIEACK.bit.ACK9=1;
	 
/*	SciaRegs.SCITXBUF = 0x00FF;             */
}

/*============================================scope��ʼ������=======================================*/

void SCOPEINITNOLOOP(void)
{
 SCOPE_INT();
 SCOPE_RST();
}

/*====================================2407�����ĸ�ͨ�������ݵ�BUFFERDATA============================*/

void SAVETOBUFFER(void)/*�����ĸ�ͨ�����ݣ���TIMER��ʱ�ж��е��ì������Դ��C33��ͨ����DUAL RAM*/
{
 if(change_online > 0)/*��Ҫ����ȷ��change_online����1����2������Ӧ����1��������2*/
 {
  change_online = change_online - 1;
 }
 else
 {
  SCIStart = 1;
 }
 if(MultiSingle == 1 || (MultiSingle == 0 && Clock >= StartTime))      /*Clockһֱ�����ӣ��ʲ���>=*/
 {
  if(EnableWrite == 1)
  {
    SampleCounter = SampleCounter + 1;
    if(SampleCounter == SampleTime)
    {
     SampleCounter = 0x0000;
    /* BUFFERDATA[BufPointer_PWM] = *Val_Channel;
     BufPointer_PWM = BufPointer_PWM + 1;
     BUFFERDATA[BufPointer_PWM] = *(Val_Channel + 1);
     BufPointer_PWM = BufPointer_PWM + 1;
     BUFFERDATA[BufPointer_PWM] = *(Val_Channel + 2);
     BufPointer_PWM = BufPointer_PWM + 1;
     BUFFERDATA[BufPointer_PWM] = *(Val_Channel + 3);*/
     BufPointer_PWM = BufPointer_PWM + 4;

     if(BufPointer_PWM <= buffer_length - 1)/*��һ������δ����ʱ��������else�еı�����λ*/
      {}
     else
      {
       BufPointer_PWM = 0x0000;
       EnableWrite = 0x0000;
       SciaRegs.SCITXBUF = 0x00FF;
      }
    }
   }
  }
  else
  {
   SCOPE_RST();
  }
}

/*===================================���δ����п�ʼ�ɼ����ݼ�ʱ����======================================*/

void INC_CLOCK(void)
{
 S01 = S01 + 1;
 if(S01 == S01_K)
 {
  S01 = 0;
  Clock = Clock + 1;
 }
}


