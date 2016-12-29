/*==================================================
	System Name	:	PMSM_Control
	File Name	:	User_Define.h
	Description	:	
	Originator	:	Ding
==================================================*/
//
#define	DSPEDITION	102	//

#define	PI	3.1415932654	//

#if CPU_FRQ_150MHZ
	#define DSP_FREQ	 	150				
#else
	#define DSP_FREQ	 	100	
#endif

#define SWTICKS 22776

//一区地址为0x004000-0x004FFF
//CPLD只读区域   0x004000-0x00400F
#define PWMFRQS			10		//Khz
//#define PWMFRQS			4		//Khz
#define PWMPERIOD		(long)DSP_FREQ * 1000/(2 * PWMFRQS)
#define ONEPERIOD		10/PWMFRQS	//ms

#define PWM_DBRED  300 // deadtime     4us

#define SPEEDPERIOD		1	//speed loop T

#define OVERLORDERR		18000
#define CURLIMIT		((Uint32)PWMPERIOD * 768)
#define VOLLIMIT		((Uint32)PWMPERIOD * 3/2)


#define	Fibra_In_Address								(volatile unsigned int *)0x004003		//R
#define	Optocoupler_In_Address							(volatile unsigned int *)0x004004		//R



//只写区域 0x004010-0x00401F
#define	Led_Out_Address									(volatile unsigned int *)0x004010		//W
#define	Fibra_Out_Address								(volatile unsigned int *)0x004011		//W
#define	AD2S1200_Disable_Address						(volatile unsigned int *)0x004012		//W
#define	PWMEN_Address									(volatile unsigned int *)0x004013		//W


//读写区域 0x004020-0x00402F
#define	AD2S1200_Config_Mode_Address					(volatile unsigned int *)0x004023		//W
#define	AD2S1200_PositionOut_Mode_Address				(volatile unsigned int *)0x004020		//R
#define	AD2S1200_SpeedOut_Mode_Address					(volatile unsigned int *)0x004022		//R




//数据定义

#define	AD2S1200_POSITIONMSB							0x80
#define	AD2S1200_POSITIONLSB							0x81
#define	AD2S1200_VELOCITYMSB							0x82
#define	AD2S1200_VELOCITYLSB							0x83
#define	AD2S1200_LOSTHRES								0x88
#define	AD2S1200_DOSORTHRES								0x89
#define	AD2S1200_AD2S1200_DOSMISTHRES					0x8A
#define	AD2S1200_DOSRSTMXTHRES							0x8B
#define	AD2S1200_DOSRSTMITHRES							0x8C
#define	AD2S1200_LOTHITHRES								0x8D
#define	AD2S1200_LOTLOTHRES								0x8E
#define	AD2S1200_Set_Frequency							0x91
#define	AD2S1200_Control_Register						0x92
#define	AD2S1200_Soft_Reset								0xF0
#define	AD2S1200_Fault_Register							0xFF
#define AD2S1200_POS_VEL								0x00  //void register for normal read address


/*关于IO口德定义*/

#define Set_SAMPLE_DSP         		 GpioDataRegs.GPBSET.bit.GPIO49=1
#define Clear_SAMPLE_DSP 	    	 GpioDataRegs.GPBCLEAR.bit.GPIO49=1

#define Data74245_Write_Direction 	  GpioDataRegs.GPBSET.bit.GPIO48=1
#define Data74245_Read_Direction 	   GpioDataRegs.GPBCLEAR.bit.GPIO48=1

#define Data74245_Enable 	  		GpioDataRegs.GPBCLEAR.bit.GPIO50=1
#define Data74245_Disable 	        GpioDataRegs.GPBSET.bit.GPIO50=1

#define PWM_Enable 	  			*PWMEN_Address=0
#define PWM_Disable 	        *PWMEN_Address=1



/*=================end===============================*/

