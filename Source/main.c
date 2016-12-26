//

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

#define ADCONST 0.00073242
// Prototype statements for functions found within this file.
Uint16 cpld_data[15];

interrupt void MainISR(void);

extern EPwmSetup();
extern void InitXintf(void);

//=======================================================================================//
float Id_Ref=0;
float Iq_Ref=0;
//==================================================================================//
float Vuv = 0,Vvw = 0,Vwu = 0;
float Iu=0, Iv=0, Iw=0;
//=======================================================================================//
//	for internal AD converter
int AD_chan0 = 0;
int AD_chan1 = 0;
int AD_chan2 = 0;
int AD_chan3 = 0;
int AD_chan4 = 0;
int AD_chan5 = 0;

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

    InitXintf();

    //Timer0 Config
    InitCpuTimers();
    ConfigCpuTimer(&CpuTimer0, 150, 1000);
    CpuTimer0Regs.TCR.bit.TSS = 0;

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

	Vuv = AD_chan0*ADCONST;
	Vvw = AD_chan1*ADCONST;
	Vwu = AD_chan2*ADCONST;

	Iu = AD_chan3*ADCONST;
	Iv = AD_chan4*ADCONST;
	Iw = AD_chan5*ADCONST;

	if(Vuv>0.5) EPwm4Regs.CMPA.half.CMPA = 1000;
	else EPwm4Regs.CMPA.half.CMPA=11000;

	EPwm1Regs.ETCLR.bit.INT = 1;
	PieCtrlRegs.PIEACK.all |= PIEACK_GROUP3;
}

//===========================================================================
// No more.
//===========================================================================

