// TI File $Revision: /main/9 $
// Checkin $Date: April 21, 2008   15:43:19 $
//###########################################################################

//###########################################################################
// $TI Release: DSP2833x/DSP2823x C/C++ Header Files V1.31 $
// $Release Date: August 4, 2009 $
//###########################################################################

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "Svpwm.h"

#define ADCONST 0.00073242
// Prototype statements for functions found within this file.
Uint16 cpld_data[15];

interrupt void MainISR(void);

extern EPwmSetup();
extern void InitXintf(void);

//=======================================================================================//
float EXT_position;


int A,B;
int IO1,IO2;
int commond_I1= 0;
int commond_I2=0;
int commond_cnt=0;
int commond = 0;
int compare = 0;
int compare_error_cnt = 0;
int pulsecount=0;
int breakopen = 0;
int breakclose = 0;
int IO_error_count = 0;
int IO_error = 0;
int I1=0;
int I2=0;
int I3=0;
int I4=0;
int I1_Old=0;
int I2_Old=0;
int I3_Old=0;
int I4_Old=0;
int O1;
int O2;
int EXT_DIN = 0;
int EXT_DIN_old =0;
int EXT_DIN_cnt=0;
int EXT_DIN_time[60];
int brake_cnt = 0;

int wave_flag=1;    //录波标志
int transfer_signal;	//数据从缓冲区开始转移到录波区的标志
int ad_signal;			//地址传递标志
long break_address;   //分（合）闸时刻指针所在位置

int interrupt_time=0;	//进入主中断的次数
int start_address;
int kk=0;
int j,k=0;
long iw;

int year=0;
int month=0;
int day=0;
int hour=0;
int minute=0;
int second=0;
int millisecond=0;
int action_type;
int filled_read=00;
//int lmm;


int scope_wave[50];
int	scope_index0=0;
int	scope_index1=1;
int	scope_index2=2;
int	scope_index3=3;
int i_cnt = 0;
int stop=0;
int connect=0;
int brake_rdy = 1;
int brake_rdy_cnt = 0;
int close_open_cnt = 0;
float brake_speed_test;
//=======================================================================================//
float Id_Ref=0;
float Iq_Ref=0;
//==================================================================================//
float Vdc;
float Iu=0, Iv=0, Iw=0;
Uint16 AD2S_Position=0;
Uint16 AD2S_Position_Fil=0;
Uint16 AD2S_Position_Ini=0;
Uint16 AD2S_Position_Pre=0;
Uint16 AD2S_Position_Pre_Old=0;
Uint16 AD2S_Position_Old[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
Uint16 AD2S_Position_H=0;
Uint16 AD2S_Position_L=0;
int16 AD2S_Speed=0;
int16 AD2S_Speed_H=0;
int16 AD2S_Speed_L=0;
float P_Speed=0;//根据位置计算得到的电机转速
float Speed=0;//根据旋变直接读出的速度
float Speed_old=0;
float Position=0;
float Position_old=0;
float Position_lpf=0;
float Position_lpf_old=0;
float ElePosition=0;

float TargetPositionOld=0;//上一时刻位置给定

float PositionRefOld[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//上一时刻给定位置，用于计算前馈速度
//=======================================================================================//
//	for internal AD converter
int AD_chan0 = 0;
int AD_chan1 = 0;
int AD_chan2 = 0;
int AD_chan3 = 0;
int AD_chan4 = 0;
int AD_chan5 = 0;
int AD_chan6 = 0;
int AD_chan7 = 0;

int AD_chan8 = 0;
int AD_chan9 = 0;
int AD_chan10 = 0;
int AD_chan11 = 0;
int AD_chan12 = 0;
int AD_chan13 = 0;
int AD_chan14 = 0;
int AD_chan15 = 0;

	//用于电流采样的校正
Uint32 AD_chan4_zero = 0;
Uint32 AD_chan5_zero = 0;
float AD_chan4_zero_float = 0;
float AD_chan5_zero_float = 0;

/*宏定义SRAM四个区域的起始地址*/
Uint16 *ExRamZ1Start=(Uint16*)0x210000;
//Uint16 *ExRamZ2Start=(Uint16*)0x230000;
//Uint16 *ExRamZ3Start=(Uint16*)0x230000;
//Uint16 *ExRamZ4Start=(Uint16*)0x240000;


void main(void)
  {

// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the DSP2833x_SysCtrl.c file.
   InitSysCtrl();

////////////////////////////////////////////////////////////////////////////////////
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
	InitFlash();

// Step 2. Initalize GPIO:


// This example function is found in the DSP2833x_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
// InitGpio(); //Skipped for this example

// For this example, only init the pins for the SCI-A port.
// This function is found in the DSP2833x_Sci.c file.

// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
   DINT;

// Initialize PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the DSP2833x_PieCtrl.c file.
   InitPieCtrl();

// Disable CPU interrupts and clear all CPU interrupt flags:
   IER = 0x0000;
   IFR = 0x0000;

// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in DSP2833x_DefaultIsr.c.
// This function is found in DSP2833x_PieVect.c.
   InitPieVectTable();

	EALLOW;	// Allow access to EALLOW protected registers
	PieVectTable.EPWM1_INT 		= &MainISR;		//PWM1
//	PieVectTable.XINT2 			= &INDEXISR;	//Z


//    PieVectTable.XINT5=&INT5_chafen_isr;  //将中断服务子函数的地址存放到相应的向量地址中
//	PieVectTable.WAKEINT 		= &PseudoWatchdogInterrupt;//WD
	EDIS;   // Disable access to EALLOW protected registers

	//EINT;  //使能全局中断
	//ERTM;

    InitXintf();

    InitSpi();

    //Timer0 Config
    InitCpuTimers();
    ConfigCpuTimer(&CpuTimer0, 150, 1000);
    CpuTimer0Regs.TCR.bit.TSS = 0;

    SCOPEINITNOLOOP();

    // This function initializes the Xintf to a known state.

    //configure for the EV module
    EPwmSetup();
    InitAdc();

	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;// Enable PWM1 int:INT3

	PieCtrlRegs.PIEACK.all = 0xFFFF;// Enables PIE to drive a pulse into the CPU

//	PieCtrlRegs.PIEIER12.bit.INTx3=1;  //使能PIE组5中与INT5_chafen对应的中断使能位XINT3	换一个中断向量
//	IER|=M_INT12;  //使能CPU级的中断信号

	IER |= M_INT9; // for the scia interrupt
	IER |= M_INT3; // for the PWM1 interrupt
	IER |= M_INT2; // for the PWM1 TZINT interrupt

	//*AD2S1200_Config_Mode_Address=0x03;//Configure Out


    EINT;   // Enable Global interrupt INTM
    ERTM;   // Enable Global realtime interrupt DBGM
//    *Fibra_Out_Address = 0x01;

	for(;;)
    {

    }

}

/*================================================*/
/* Description:	P/S/T control program			  */
/*================================================*/
interrupt void MainISR(void)
{
	AD_chan0 = AdcRegs.ADCRESULT0>>4;
	AD_chan1 = AdcRegs.ADCRESULT1>>4;
	AD_chan2 = AdcRegs.ADCRESULT2>>4;
	AD_chan3 = AdcRegs.ADCRESULT3>>4;
	AD_chan4 = AdcRegs.ADCRESULT4>>4;
	AD_chan5 = AdcRegs.ADCRESULT5>>4;
	AD_chan6 = AdcRegs.ADCRESULT6>>4;
	AD_chan7 = AdcRegs.ADCRESULT7>>4;
	AD_chan8 = AdcRegs.ADCRESULT8>>4;
	AD_chan9 = AdcRegs.ADCRESULT9>>4;
	AD_chan10 = AdcRegs.ADCRESULT10>>4;
	AD_chan11 = AdcRegs.ADCRESULT11>>4;
	AD_chan12 = AdcRegs.ADCRESULT12>>4;
	AD_chan13 = AdcRegs.ADCRESULT13>>4;
	AD_chan14 = AdcRegs.ADCRESULT14>>4;
	AD_chan15 = AdcRegs.ADCRESULT15>>4;

	Vdc = AD_chan0*ADCONST;

	//if(Vdc>0.5) EPwm4Regs.CMPA.half.CMPA = 1000;
	//else EPwm4Regs.CMPA.half.CMPA=11000;

	EPwm1Regs.ETCLR.bit.INT = 1;
	PieCtrlRegs.PIEACK.all |= PIEACK_GROUP3;
}

//===========================================================================
// No more.
//===========================================================================

