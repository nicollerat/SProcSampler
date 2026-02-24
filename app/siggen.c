/*
 * siggen.c
 *
 *	Update the DAC1 output to generate a signal with several frequencies at different levels
 *	add some noise to hide evident values...
 *
 *  to ensure that every frequency fit the window, each is defined with a number of periods
 *  f[i] = Fe / N[i]
 *  Created on: Dec 2, 2025
 *      Author: marc.nicoller
 */

#include "math.h"
#include "stdint.h"
#include "dac.h"
#include "main.h"

#define N 2048
#define CHN 5

uint16_t tabValues[N];
float Te = 10e-6;

float g[CHN] = { 3.1416, 0.5, 2.81, 1, 2 };
float NP[CHN] = { 100, 80, 60, 55, 50 }; // periods
float f[CHN];

int my_index = 0;

void sgInit()
{
	for (int i=0;i<CHN;i++) {
		f[i] = 1 / (Te*NP[i]);
	}
	float s=0;
	for (int i=0; i<N; i++ ) {
		float t=i*Te;
		for (int j=0;j<CHN;j++) {
			s+= g[j]*sinf(2*M_PI*f[j]*t);
		}

		tabValues[i] = 1024.5 + 512*s;
	}

	setPWMFreq(1/Te);
}

void sgUpdate()
{
	writeDAC1(tabValues[my_index]);
	my_index = (my_index+1)%N;
}
