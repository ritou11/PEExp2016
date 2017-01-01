#include "math.h"

#define pi2 6.2832
#define Kp_SPLL 0.1
#define Ki_SPLL 0.007
#define sine_N 50

float theta = 0;
float uq_i = 0;
float sine[sine_N];

float dtheta = 0*3.1415/12;

void sine_init(float* sine)
{
	int i;
	for(i = 0; i<sine_N; i++)
	{
		sine[i] = sin(pi2*i/sine_N);
	}
}

void Update_SPLL(float u_ab, float u_bc, float u_ca)
{
	float si;
	float co;
	float uq_pid;
	float uq;
	si = sin(theta);
	co = cos(theta);
	uq = (-si - 0.5774*co) * u_ab + 1.155*co*u_bc + (si - 0.5774*co)*u_ca;
	uq_i = fmod(uq_i + uq,pi2 / Ki_SPLL);
	uq_pid = Kp_SPLL * uq + Ki_SPLL * uq_i;
	theta = fmod(theta + uq_pid + pi2/33/26,pi2);

	if(theta < 0)
		theta += pi2;
}

float Get_SPLL()
{
	return theta+dtheta;
}
