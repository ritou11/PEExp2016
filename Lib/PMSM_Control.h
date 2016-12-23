#include "common_struct.h"
#include "Svpwm.h"
#include <math.h>
//====================================================================================================//
typedef struct
{
	float Ts;

	float Ia;
	float Ib;
	float Ic;
	float Vdc;

	float Ke; //Flux
	float Ld; //Ld
	float Lq; //Lq
	float p; //Pole Pairs
	float J; //Inertia

	float Ialpha;
	float Ibeta;
	float Id;
	float Iq;

	float TargetPosition;

	float PositionRef;
	float PositionRefOld;
	float Position;
	int   PositionLoop;

	float Temp;

	float SpeedRef;
	float Speed;
//	float Position;
	int   SpeedLoop;

	float Id_ref;
	float Iq_ref;

	float Ualpha;
	float Ubeta;
	float Theta;
	float cos_theta;
	float sin_theta;
	float Ud;
	float Uq;

	float  Ta;				/* Output: reference phase-a switching function  */
	float  Tb;				/* Output: reference phase-b switching function   */
	float  Tc;				/* Output: reference phase-c switching function   */


	Uint16  Brake_Close;
	Uint16  Brake_Open;


	PI_REG PI_Position;
	PI_REG PI_Speed;
	PI_REG PI_id;
	PI_REG PI_iq;

	void  (*Init)();
	void  (*Calc)();

} PMSM_Control;

//typedef PMSM_Control *PMSM_Control_handle;

#define PMSM_Control_DEFAULTS {0, \
						  0, \
						  0, \
						  0, \
						  0, \
						  0, \
                          0, \
                          0, \
						  0, \
						  0, \
						  0, \
						  0, \
						  0, \
						  0, \
                          0, \
                          0, \
						  0, \
						  0, \
						  0, \
                          0, \
                          0, \
						  0, \
						  0, \
						  0, \
						  0, \
						  0, \
						  0, \
						  0, \
						  0, \
						  0, \
						  0, \
						  0, \
						  0, \
						  0, \
						  0, \
						  0, \
						  0, \
						  PI_REG_DEFAULTS, \
						  PI_REG_DEFAULTS, \
						  PI_REG_DEFAULTS, \
						  PI_REG_DEFAULTS, \
						  (void (*)(long))PMSM_Control_Init,\
						  (void (*)(long))PMSM_Control_Calc}

//void PMSM_Control_Init(PMSM_Control_handle);
//void PMSM_Control_Calc(PMSM_Control_handle);

//==================================================================================================//
void PMSM_Control_Init(PMSM_Control *p)
{
	p->Id_ref = 0;
	p->Ts = 0.0001;

// R = 0.0695;
//	p->Ke = 0.256;
	p->Ke = 0.268;
//	p->Ld = 0.00151;
//	p->Lq = 0.0041;
	p->Ld = 0.001138;
	p->Lq = 0.001502;
	p->p = 4;
	p->J = 0.2;

	p->PI_Position.kp = 30;//
	p->PI_Position.ki = 0;//
	p->PI_Position.max = 5;
	p->PI_Position.ts = p->Ts;

	p->PI_Speed.kp = 2.1524*2;//200*p->J/(4*4*0.4646);
	p->PI_Speed.ki = 0.0010762*6;//100*p->J/(4*4*0.4646)/10000;
	p->PI_Speed.max = 60;
	p->PI_Speed.ts = p->Ts;

	p->PI_id.kp = 38*2;//2000*0.019;   2.3328
	p->PI_id.ki = 0.0224*2;//2000*0.1121/10000;  99.28
	p->PI_id.max = 500;
	p->PI_id.ts = p->Ts;

	p->PI_iq.kp = 44*3;//2000*0.022;     2.21202
	p->PI_iq.ki = 0.0224*3;//2000*0.1121/10000;   99.28
	p->PI_iq.max = 500;
	p->PI_iq.ts = p->Ts;

}

#pragma CODE_SECTION(PMSM_Control_Calc, "ramfuncs")
void PMSM_Control_Calc(PMSM_Control *p)
{
	//CLARKE  VolClarke = CLARKE_DEFAULTS;
	//PARK VolPark = PARK_DEFAULTS;
	IPARK VolIPark = IPARK_DEFAULTS;
//	ICLARKE  VolIClarke = ICLARKE_DEFAULTS;

	CLARKE  CurClarke = CLARKE_DEFAULTS;
	PARK CurPark = PARK_DEFAULTS;

	Svm Svpwm=Svm_DEFAULTS;

	float theta=0;
	float cos_theta=0;
	float sin_theta=0;
	float temp=0;
	float pos_temp;
	//float temp1=0;

	p->SpeedLoop++;
	p->PositionLoop++;

/////////////////////////////////////
	theta=p->Theta;
/////////////////////////////////////
	cos_theta = cos(theta);
	sin_theta = sin(theta);

	p->cos_theta=cos_theta;
	p->sin_theta=sin_theta;

	CurClarke.as = p->Ia;
	CurClarke.bs = p->Ib;
	CurClarke.cs = p->Ic;
	CurClarke.calc(&CurClarke);
	CurPark.xalpha = CurClarke.xalpha;
	CurPark.xbeta = CurClarke.xbeta;
	p->Ialpha=CurClarke.xalpha;
	p->Ibeta=CurClarke.xbeta;
	CurPark.cos_ang= cos_theta;
	CurPark.sin_ang= sin_theta;
	CurPark.calc(&CurPark);
	p->Id = CurPark.xd;
	p->Iq = CurPark.xq;
	
	if((p->PositionLoop)>9)
	{
		p->PI_Position.ref = p->PositionRef;//rc1.setpt_value;
//		p->PI_Position.ref=0;
		p->PI_Position.fb = p->Position;
		pos_temp=p->TargetPosition-p->Position;
		if(pos_temp<0)
		{
			pos_temp=-pos_temp;
		}
		/*if(p->Brake_Close==1)
		{
			if(pos_temp<0.5)
			{
					p->PI_Position.kp = 50;//
			}else
			{
					p->PI_Position.kp = 50;//
					//p->PI_Position.kp = 60*(temp-0.05)+20;//
			}
		}else
		{
			if(pos_temp<0.5)
			{
					p->PI_Position.kp = 50;//
			}else
			{
					p->PI_Position.kp = 50;//
							//p->PI_Position.kp = 10*(temp-0.05)+0.1;60*(temp-0.05)+20//
			}
		}*/

		//p->PI_Position.kp = 1;//
		p->PI_Position.kp = 50;
		p->PI_Position.ki = 0;//
//		p->PI_Position.max = 600*0.1047;


		/*if(p->Brake_Close==1)
		{
			p->PI_Position.max = 80;
//			p->PI_Position.max = 400*0.01047;
		}else if(p->Brake_Open==1)
		{
			p->PI_Position.max = 105;
		//	p->PI_Position.max = 400*0.1047;
		}*/

		p->PI_Position.calc(&(p->PI_Position));
		p->PositionLoop=0;
		/*if(temp<0.02)
		{
			p->PI_Position.out=0;
		}*/
	}

	if(pos_temp<0.7&& p->Brake_Open == 1)
	        {
			p->Temp=p->Temp*0.1;
	        }
	if(pos_temp<0.1&& p->Brake_Close == 1)
	        {
			p->Temp=p->Temp*0.1;
	        }
	if((p->SpeedLoop)>3)
	{
		p->PI_Speed.ref =  p->PI_Position.out+p->Temp;//+rc1.setpt_value;+
		if(p->PI_Speed.ref > 110)
		{p->PI_Speed.ref = 110;}
		else if(p->PI_Speed.ref < -110){
			p->PI_Speed.ref = -110;
		}

	//	brake_tmp = brake_r * brake_sin +(brake_r*brake_r*brake_sin2)/2/sqrt(brake_l*brake_l-brake_r*brake_r*brake_sin*brake_sin);
	//	if(brake_tmp<0) brake_tmp=-brake_tmp;
	//	if(brake_tmp<0.0001) brake_tmp = 0.0001;
	//	p->PI_Speed.ref = p->PI_Speed.ref/brake_tmp;
/*		float position_tmp;
		position_tmp = p->TargetPosition-p->Position;
		if(position_tmp<0) position_tmp=-position_tmp;
		if(p->PI_Speed.ref> 105)
		{
			if(position_tmp <1) {p->PI_Speed.ref =  20*p->PI_Position.out+0.1*p->Temp;}
			else {
				p->PI_Speed.ref= 105;
			//}
		}
		else if(p->PI_Speed.ref< -80)
		{
			if(position_tmp <1) {p->PI_Speed.ref =  20*p->PI_Position.out+0.1*p->Temp;}
						else {
							p->PI_Speed.ref= -80;
					//	}
		}
		if(position_tmp <1) {p->PI_Speed.ref =  20*p->PI_Position.out+0.1*p->Temp;}*/
		//		p->PI_Speed.ref = -1*0.1047;//rc1.setpt_value;
//		p->PI_Speed.ref = p->SpeedRef;//rc1.setpt_value;

		p->PI_Speed.fb = p->Speed;

			//p->PI_Speed.kp = 2.1524*0.005;//200*p->J/(4*4*0.4646);
			//p->PI_Speed.ki = 1.0762*10;//100*p->J/(4*4*0.4646)/10000;

		if(p->Brake_Close==1)
		{
			/*if(p->PI_Speed.out>0)
			{
				p->PI_Speed.max = 200;
						//	p->PI_Speed.max = 400;
			}else
			{
				p->PI_Speed.max = 200;
						//	p->PI_Speed.max = 400;
			}*/
			p->PI_Speed.max = 200;
//			p->PI_Speed.max = 100;
		}else if(p->Brake_Open==1)
		{

				if(p->Position > 5.8) {
					if(p->PI_Speed.out>0)
					{
						p->PI_Speed.max = 100;
					}
				}
				else{
					p->PI_Speed.max = 410;
				}
			//	p->SpeedRef = p->PI_Speed.ref;
			//	p->PI_Speed.max = 400;

/*			else
			{
				if(p->Position > 5.8) {
									p->PI_Speed.max = 100;
								}
								else{
									p->PI_Speed.max = 410;
								}
			//	p->PI_Speed.ref = p->SpeedRef;
			//	p->PI_Speed.max = 400;
			}*/

		}
		/*temp=p->PI_Speed.ref;
		temp1=p->PI_Speed.ref-p->PI_Speed.fb;
		if(temp<0)
		{
			temp=-temp;
		}
		if(temp1<0)
		{
			temp1=-temp1;
		}
			//p->PI_Speed.kp = 2.1524*0.005*(1+10000*temp*temp*temp*temp);//200*p->J/(4*4*0.4646);
			//p->PI_Speed.ki = 1.0762*0.005*(1+10000*temp*temp*temp*temp);;//100*p->J/(4*4*0.4646)/10000;

			if(temp1>20)
			{
				p->PI_Speed.kp = 2.1524*2;//200*p->J/(4*4*0.4646);
				p->PI_Speed.ki = 3.0762*0.06;//100*p->J/(4*4*0.4646)/10000;
			}else if((temp1>5))
			{
				p->PI_Speed.kp = 2.1524*2;//200*p->J/(4*4*0.4646);
				p->PI_Speed.ki = 3.0762*0.06;//100*p->J/(4*4*0.4646)/10000;
			}else if((temp1>1))
			{
				p->PI_Speed.kp = 2.1524*2;//200*p->J/(4*4*0.4646);
				p->PI_Speed.ki = 3.0762*0.06;//100*p->J/(4*4*0.4646)/10000;
			}else
			{
			//	p->PI_Speed.kp = 2.1524*0.5*(1+10000*temp*temp*temp*temp);//200*p->J/(4*4*0.4646);
			//	p->PI_Speed.ki = 3.0762*0.1*(1+10000*temp*temp*temp*temp);;//100*p->J/(4*4*0.4646)/10000;
				p->PI_Speed.kp = 2.1524*2;//200*p->J/(4*4*0.4646);
			    p->PI_Speed.ki = 3.0762*0.06;//100*p->J/(4*4*0.4646)/10000;
			}*/

			if((pos_temp>0.8))
			{
							p->PI_Speed.kp = 2.1524*8;//200*p->J/(4*4*0.4646);   *5
							p->PI_Speed.ki = 3.0762*0.5;//100*p->J/(4*4*0.4646)/10000;   *0.3
			}
			else
			{
							p->PI_Speed.kp = 2.1524*6;//200*p->J/(4*4*0.4646);   *5
							p->PI_Speed.ki = 3.0762*0.02;//100*p->J/(4*4*0.4646)/10000;   *0.3
			}


		p->PI_Speed.calc(&(p->PI_Speed));
		/*if((temp<0.1)&&(temp1<0.1))
		{
			p->PI_Speed.out=0;
		}*/
		p->SpeedLoop=0;
	}

//	if((p->Position <4.2) && (p->Brake_Open == 1)){
//		p->Iq_ref = p->PI_Speed.max;
//		}
//	else
//		{
			p->Iq_ref = p->PI_Speed.out;
//		}

//	p->Iq_ref =0;
//	p->Id_ref=1.2*(49.465-sqrt(2446.78+(p->Iq_ref)*(p->Iq_ref)));//KE/(2*(Lq-Ld))-sqrt((KE/(2*(Lq-Ld)))^2+Iq^2);
//	p->Id_ref=1.0*(49.465-sqrt(2446.78+(p->Iq_ref)*(p->Iq_ref)));//KE/(2*(Lq-Ld))-sqrt((KE/(2*(Lq-Ld)))^2+Iq^2);
//	p->Id_ref=1.0*(-815.8-sqrt(2446.78+(p->Iq_ref)*(p->Iq_ref)));//KE/(2*(Lq-Ld))-sqrt((KE/(2*(Lq-Ld)))^2+Iq^2);
//	p->Iq_ref =3;
//	p->Id_ref=0;
//	p->Id_ref=1.0*(815.8-sqrt(815.8*815.8+(p->Iq_ref)*(p->Iq_ref)));//KE/(2*(Lq-Ld))-sqrt((KE/(2*(Lq-Ld)))^2+Iq^2);
	p->Id_ref = p->Iq_ref;
	if( p -> Id_ref > 0) p->Id_ref = -p->Id_ref;
//    p->Id_ref=0;

	p->PI_id.max = p->Vdc;
	p->PI_iq.max = p->Vdc;

	p->PI_id.ref = p->Id_ref;
	p->PI_id.fb = p->Id;
	temp=p->PI_id.ref - p->PI_id.fb;
	float id_kp = 0.5832;
	float iq_kp = 1;
	float id_ki = 2.17125;
	float iq_ki = 0.019;  //220
/*if (p->Brake_Open == 1){
	if(p->Iq_ref < 350)
	{
	 id_kp = 2.3328*0.25;
	 iq_kp = 2.2125*0.45;
	 id_ki = 0.01737*0.25/0.002;
	 iq_ki = 0.01737*1.1;
	}
	else{
		 id_kp = 2.3328*0.25;
		 iq_kp = 2.2125*0.45;
		 id_ki = 0.01737*0.25/0.002;
		 iq_ki = 0.01737*1.1;
	}
}
else{
	 id_kp = 2.3328*0.25;
	 iq_kp = 2.2125*0.45;
	 id_ki = 0.01737*0.25/0.002;
	 iq_ki = 0.01737*1.1;
}*/

	if(temp<0)
	{
		temp=-temp;
	}
/*	if(temp>0)
	{*/
		p->PI_id.kp = 1.1664;//2000*0.019;
		p->PI_id.ki = 0.0434;//2000*0.1121/10000;
	/*}else if(temp>2)
	{
		p->PI_id.kp = 1.1664;//2000*0.019;
		p->PI_id.ki = 0.00434;//2000*0.1121/10000;
	}else
	{
		p->PI_id.kp = 0.11664;//2000*0.019;
		p->PI_id.ki = 4.34;//2000*0.1121/10000;
	}*/
	p->PI_id.calc(&(p->PI_id));

	p->PI_iq.ref = p->Iq_ref;
	p->PI_iq.fb = p->Iq;
	temp=p->PI_iq.ref - p->PI_iq.fb;


	if(temp<0)
	{
			temp=-temp;
	}
	if(temp>25)
	{
			p->PI_iq.kp = 1;//2000*0.019;
			p->PI_iq.ki = 0.019;//2000*0.1121/10000;
	}else
	{
			p->PI_iq.kp = 0.5;//2000*0.019;
			p->PI_iq.ki = 0.009;//2000*0.1121/10000;
	}
	p->PI_iq.calc(&(p->PI_iq));
	
//	p->ud_ref = p->ud + p->PI_id.out - p->iq * p->Ls * p->ws *1;
//	p->uq_ref = p->uq + p->PI_iq.out + p->id * p->Ls * p->ws *1;

	p->Ud = p->PI_id.out;
//	p->Uq = -p->Uq +2*p->PI_iq.out;
	p->Uq = p->PI_iq.out;
	float u_temp;
	u_temp=sqrt(p->Ud*p->Ud+p->Uq*p->Uq);
	if(u_temp>p->Vdc)
	{
		p->Ud = p->Vdc/u_temp*p->Ud;
		p->Uq = p->Vdc/u_temp*p->Uq;
	}
	/*if(p->Uq > 0.7*p->Vdc)
	{
		p->Uq = 0.7*p->Vdc;
	}
	else if(p->Uq < -0.7*p->Vdc)
	{
		p->Uq = -0.7*p->Vdc;
	}*/

	VolIPark.xd = p->Ud;
	VolIPark.xq = p->Uq;

//	VolIPark.xd = 0;
//	VolIPark.xq = 5;
	VolIPark.cos_ang = cos_theta;
	VolIPark.sin_ang = sin_theta;
	VolIPark.calc(&VolIPark);

	Svpwm.Ualfa=VolIPark.xalpha;
	Svpwm.Ubeta=VolIPark.xbeta;

	p->Ualpha=Svpwm.Ualfa;
	p->Ubeta=Svpwm.Ubeta;

//	Svpwm.Ualfa=60;
//	Svpwm.Ubeta=0;
	Svpwm.Vdc=p->Vdc;
//	Svpwm.Vdc=30;

	Svpwm.calc(&Svpwm);
	p->Ta = Svpwm.Ta;
	p->Tb = Svpwm.Tb;
	p->Tc = Svpwm.Tc;

}




