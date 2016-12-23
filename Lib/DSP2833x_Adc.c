// TI File $Revision: /main/5 $
// Checkin $Date: October 23, 2007   13:34:09 $
//###########################################################################
//
// FILE:	DSP2833x_Adc.c
//
// TITLE:	DSP2833x ADC Initialization & Support Functions.
//
//###########################################################################
// $TI Release: DSP2833x/DSP2823x C/C++ Header Files V1.31 $
// $Release Date: August 4, 2009 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

#define ADC_usDELAY  5000L

//---------------------------------------------------------------------------
// InitAdc:
//---------------------------------------------------------------------------
// This function initializes ADC to a known state.
//
void InitAdc(void)
{
    extern void DSP28x_usDelay(Uint32 Count);


    // *IMPORTANT*
	// The ADC_cal function, which  copies the ADC calibration values from TI reserved
	// OTP into the ADCREFSEL and ADCOFFTRIM registers, occurs automatically in the
	// Boot ROM. If the boot ROM code is bypassed during the debug process, the
	// following function MUST be called for the ADC to function according
	// to specification. The clocks to the ADC MUST be enabled before calling this
	// function.
	// See the device data manual and/or the ADC Reference
	// Manual for more information.

	    EALLOW;
		SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;//
		ADC_cal();
		EDIS;

	AdcRegs.ADCTRL1.bit.RESET = 1;		// Reset the ADC module
	asm(" RPT #10 || NOP");				// Must wait 12-cycles (worst-case) for ADC reset to take effect

	AdcRegs.ADCREFSEL.bit.REF_SEL = 0x0; // Internal reference 2.048V
//	AdcRegs.ADCREFSEL.bit.REF_SEL = 0x1; // External reference 2.048V

	AdcRegs.ADCTRL3.all = 0x00E4;

//	AdcRegs.ADCTRL3.all = 0x00C4;		// first power-up ref and bandgap circuits
//	AdcRegs.ADCTRL3.bit.ADCBGRFDN= 0x3;//11
//	AdcRegs.ADCTRL3.bit.ADCPWDN = 1;	// Set ADCPWDN=1 to power main ADC(ADC power up)
	DSP28x_usDelay(5000);					// Wait 5ms before Conversion

//	AdcRegs.ADCTRL3.bit.ADCCLKPS = 0x0;// 25MHz
//	AdcRegs.ADCTRL1.all = 0x0e90;
	AdcRegs.ADCTRL1.all = 0x0010;
//	AdcRegs.ADCTRL1.bit.CPS = 0;//25MHz.
//	AdcRegs.ADCTRL1.bit.ACQ_PS = 0xA;  //2.5MHz
//	AdcRegs.ADCTRL1.bit.CONT_RUN = 0x0;  //Start-Stop Mode
//	AdcRegs.ADCTRL1.bit.SEQ_CASC = 0x1;  //Cascaded Operation

	AdcRegs.ADCTRL2.all = 0x0100;

	DSP28x_usDelay(5000);
    // To powerup the ADC the ADCENCLK bit should be set first to enable
    // clocks, followed by powering up the bandgap, reference circuitry, and ADC core.
    // Before the first conversion is performed a 5ms delay must be observed
	// after power up to give all analog circuits time to power up and settle

    // Please note that for the delay function below to operate correctly the
	// CPU_CLOCK_SPEED define statement in the DSP2833x_Examples.h file must
	// contain the correct CPU clock period in nanoseconds.

//    AdcRegs.ADCTRL3.all = 0x00E0;  // Power up bandgap/reference/ADC circuits
//    DELAY_US(ADC_usDELAY);         // Delay before converting ADC channels

	    //--------------------------------------------------------------------------//
	     AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 0xF;  // convert and store in 8 results registers

	     AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;
	     AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x1;
	     AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x2;
	     AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x3;

	     AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0x4;
	     AdcRegs.ADCCHSELSEQ2.bit.CONV05 = 0x5;
	     AdcRegs.ADCCHSELSEQ2.bit.CONV06 = 0x6;
	     AdcRegs.ADCCHSELSEQ2.bit.CONV07 = 0x7;

	     AdcRegs.ADCCHSELSEQ3.bit.CONV08 = 0x8;
	     AdcRegs.ADCCHSELSEQ3.bit.CONV09 = 0x9;
	     AdcRegs.ADCCHSELSEQ3.bit.CONV10 = 0xa;
	     AdcRegs.ADCCHSELSEQ3.bit.CONV11 = 0xb;

	     AdcRegs.ADCCHSELSEQ4.bit.CONV12 = 0xc;
	     AdcRegs.ADCCHSELSEQ4.bit.CONV13 = 0xd;
	     AdcRegs.ADCCHSELSEQ4.bit.CONV14 = 0xe;
	     AdcRegs.ADCCHSELSEQ4.bit.CONV15 = 0xf;

	     //AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 0x1;
	     AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 0x1;
	     //====================================================================//

    //AdcRegs.ADCTRL3.all = 0x00E0;  // Power up bandgap/reference/ADC circuits
    //DELAY_US(ADC_usDELAY);         // Delay before converting ADC channels
}

//===========================================================================
// End of file.
//===========================================================================
