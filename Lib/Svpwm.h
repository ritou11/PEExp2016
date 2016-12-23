#ifndef SVPWM_H
#define SVPWM_H
typedef struct 	{ float  Ualfa; 			/* Input: reference alfa-axis phase voltage   */
				  float  Ubeta;			/* Input: reference beta-axis phase voltage   */
				  float  V_ba_inv;
				  int  sector;
				  float  Vdc;
				  float  T1;				
				  float  T2;		
				  float  T0;
				  float  T0_half;
				  float  Ta0;				/* Output: reference phase-a switching function  */			
				  float  Tb0;				/* Output: reference phase-b switching function   */
				  float  Tc0;				/* Output: reference phase-c switching function   */
				  float  Ta;				/* Output: reference phase-a switching function  */			
				  float  Tb;				/* Output: reference phase-b switching function   */
				  float  Tc;				/* Output: reference phase-c switching function   */
				  void   (*calc)();	    /* Pointer to calculation function */ 
				} Svm;
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																				
typedef Svm *Svm_handle;
/*-----------------------------------------------------------------------------
Default initalizer for the SVGENDQ object. section
-----------------------------------------------------------------------------*/                     
#define Svm_DEFAULTS { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                       (void (*)(long))Svm_calc }

/*------------------------------------------------------------------------------
Prototypes for the functions in SVGEN_DQ.C
------------------------------------------------------------------------------*/
void Svm_calc(Svm_handle);

//extern Svm Svpwm1=Svm_DEFAULTS;
#endif

