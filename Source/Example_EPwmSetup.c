// TI File $Revision: /main/9 $
// Checkin $Date: August 10, 2007   09:05:08 $
//###########################################################################
//
// FILE:	Example_EpwmSetup.c
//
// TITLE:	Frequency measurement using EQEP peripheral
//
// DESCRIPTION:
//
// This file contains source for the ePWM initialization for the
// freq calculation module
//
//###########################################################################
// Original Author: SD
//
// $TI Release: DSP2833x Header Files V1.01 $
// $Release Date: September 26, 2007 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "User_Define.h"

#if (CPU_FRQ_150MHZ)
  #define CPU_CLK   150e6
#endif
#if (CPU_FRQ_100MHZ)
  #define CPU_CLK   100e6
#endif
#define PWM_CLK   5e3                 // If diff freq. desired, change freq here.
#define SP        CPU_CLK/(2*PWM_CLK)
#define TBCTLVAL  0x200E              // Up-down cnt, timebase = SYSCLKOUT

// SWTICKS % 3 = 0
// SWTICKS %24 = 0
void EPwmSetup()
{
	InitEPwmGpio();
	//=====================================================================
	// Config
	// Initialization Time
	//========================
	// EPWM Module 1 config
	EPwm4Regs.TBPRD = SWTICKS; // Period = 900 TBCLK counts
	EPwm4Regs.TBPHS.half.TBPHS = 0; // Set Phase register to zero
	EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Symmetrical mode
	EPwm4Regs.TBCTL.bit.PHSEN = TB_DISABLE; // Master module
	EPwm4Regs.TBCTL.bit.PRDLD = TB_SHADOW;
	EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO; // Sync down-stream module
	EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR=Zero
	EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR=Zero
	EPwm4Regs.AQCTLA.bit.CAU = AQ_SET; // set actions for EPWM1A
	EPwm4Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm4Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
	EPwm4Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary
	EPwm4Regs.DBFED = 2000; // FED = 20 TBCLKs
	EPwm4Regs.DBRED = 2000; // RED = 20 TBCLKs

	// EPWM Module 2 config
	EPwm5Regs.TBPRD = SWTICKS; // Period = 900 TBCLK counts
	EPwm5Regs.TBPHS.half.TBPHS = SWTICKS/3*2; // Phase = 300/900 * 360 = 120 deg
	EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Symmetrical mode
	EPwm5Regs.TBCTL.bit.PHSEN = TB_ENABLE; // Slave module
	EPwm5Regs.TBCTL.bit.PHSDIR = TB_DOWN; // Count DOWN on sync (=120 deg)
	EPwm5Regs.TBCTL.bit.PRDLD = TB_SHADOW;
	EPwm5Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN; // sync flow-through
	EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR=Zero
	EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR=Zero
	EPwm5Regs.AQCTLA.bit.CAU = AQ_SET; // set actions for EPWM2A
	EPwm5Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm5Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
	EPwm5Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi Complementary
	EPwm5Regs.DBFED = 2000; // FED = 20 TBCLKs
	EPwm5Regs.DBRED = 2000; // RED = 20 TBCLKs
	// EPWM Module 3 config
	EPwm6Regs.TBPRD = SWTICKS; // Period = 900 TBCLK counts
	EPwm6Regs.TBPHS.half.TBPHS = SWTICKS/3*2; // Phase = 300/900 * 360 = 120 deg
	EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Symmetrical mode
	EPwm6Regs.TBCTL.bit.PHSEN = TB_ENABLE; // Slave module
	EPwm5Regs.TBCTL.bit.PHSDIR = TB_UP; // Count UP on sync (=240 deg)
	EPwm6Regs.TBCTL.bit.PRDLD = TB_SHADOW;
	EPwm6Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN; // sync flow-through
	EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm6Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR=Zero
	EPwm6Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR=Zero
	EPwm6Regs.AQCTLA.bit.CAU = AQ_SET; // set actions for EPWM3Ai
	EPwm6Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm6Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
	EPwm6Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary
	EPwm6Regs.DBFED = 2000; // FED = 20 TBCLKs
	EPwm6Regs.DBRED = 2000; // RED = 20 TBCLKs
	// Run Time (Note: Example execution of one run-time instant)
	//===========================================================
	EPwm4Regs.CMPA.half.CMPA = SWTICKS/2; // adjust duty for output EPWM1A
	EPwm5Regs.CMPA.half.CMPA = SWTICKS/2; // adjust duty for output EPWM2A
	EPwm6Regs.CMPA.half.CMPA = SWTICKS/2; // adjust duty for output EPWM3A

	EPwm1Regs.TBPRD = SWTICKS/78;// TPwm=2*PWMPERIOD*SysClock
	EPwm1Regs.TBPHS.all = 0;     // the counter value when there is a synchronize signal

	EPwm1Regs.TBCTL.bit.CLKDIV = 0;
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0;// the timer frequency of the PWM module is 150M Hz
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
	EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;
	EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;
	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;// select the synchronize signal

	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;// set the action when comparision A event
	EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm1Regs.AQCTLB.bit.CAD = AQ_SET;
	EPwm1Regs.AQCTLB.bit.CAU = AQ_CLEAR;

	EPwm1Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;

	EPwm1Regs.ETSEL.bit.SOCAEN = 1;
	EPwm1Regs.ETSEL.bit.SOCASEL = 1;
	EPwm1Regs.ETPS.bit.SOCAPRD = ET_1ST;	//Enable the ADC Conversion A Event

	EPwm1Regs.ETSEL.bit.INTSEL=1;        	// Enable event time-base counter equal to zero. (TBCTR = 0x0000)
                                            // Generate an interrupt on the first event INTCNT = 01 (first event)
	EPwm1Regs.ETPS.bit.INTPRD =1;        // Generate an interrupt on the first event INTCNT = 01 (first event)
	EPwm1Regs.ETCLR.bit.INT = 1;       		// Enable more interrupts
	EPwm1Regs.ETSEL.bit.INTEN=1;        	// Enable event time-base counter equal to zero. (TBCTR = 0x0000)
}


