/** 
 ** \file 7Seg_BCD.c
 ** \author Jérémie Rodez https://github.com/JRodez 
 ** \version 2.0
 */

#include "sios.h"

#ifdef WIRINGPI
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#else
#include <Arduino.h>
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#ifdef DEBUG
#include <HardwareSerial.h>
#endif

unsigned int delayBeforeShifting = 5; //delay before shifting digits

// ------- TOOLS ------------

void intToDigits(uint32_t n, uint8_t *tab, uint8_t size)
{
	for (int i = size - 1; i >= 0; i--)
	{
		tab[i] = n % 10;
		n = (n - (n % 10)) / 10;
	}
}

// --------------------------

void set_DispShiftDelay(unsigned int delay)
{
	delayBeforeShifting = delay;
}

void zero_Seg7_BCD(Seg7_BCD *afficheur)
{
	for (int i = 0; i < 4; i++)
		digitalWrite(afficheur->bits[i], LOW);
}

void pwon_Seg7_BCD(Seg7_BCD *display)
{
	if (display->common_cathode)
		digitalWrite(display->power_pin, LOW);
	else
		digitalWrite(display->power_pin, HIGH);
}

void pwoff_Seg7_BCD(Seg7_BCD *display)
{
	if (display->common_cathode)
		digitalWrite(display->power_pin, HIGH);
	else
		digitalWrite(display->power_pin, LOW);
}

void create_Seg7_BCD(Seg7_BCD *display,
					 uint8_t A,
					 uint8_t B,
					 uint8_t C,
					 uint8_t D,
					 bool CathodeComune,
					 uint8_t alim)
{
	uint8_t *ABCD = (uint8_t *)malloc(sizeof(uint8_t) * 4);

	ABCD[0] = A;
	ABCD[1] = B;
	ABCD[2] = C;
	ABCD[3] = D;

	display->bits = ABCD;
	display->power_pin = alim;
	display->common_cathode = CathodeComune;
	pinMode(display->power_pin, OUTPUT);
	pwoff_Seg7_BCD(display);
	//	digitalWrite(display->power_pin, HIGH);

	for (int i = 0; i < 4; i++)
		pinMode(display->bits[i], OUTPUT);

	//	digitalWrite(display->power_pin, LOW);
	pwon_Seg7_BCD(display);
	zero_Seg7_BCD(display);

	//	return (display);
}

void print_Seg7_BCD(Seg7_BCD *display, uint8_t n)
{
	pwoff_Seg7_BCD(display);
	zero_Seg7_BCD(display);

	if (n >= 0b1000)
	{
		digitalWrite(display->bits[3], HIGH);
		n -= 0b1000;
	}
	if (n >= 0b100)
	{
		digitalWrite(display->bits[2], HIGH);
		n -= 0b100;
	}
	if (n >= 0b10)
	{
		digitalWrite(display->bits[1], HIGH);
		n -= 0b10;
	}
	if (n >= 0b1)
	{
		digitalWrite(display->bits[0], HIGH);
		n -= 0b1;
	}
	pwon_Seg7_BCD(display);
}

//-------------------------------------------------------------

void create_Mlpx_7Seg_BCD(Mlpx_7Seg_BCD *displays,
						  uint8_t A,
						  uint8_t B,
						  uint8_t C,
						  uint8_t D,
						  bool CathodeComune,
						  uint8_t count,
						  ...)
{

	//Mlpx_7Seg_BCD displays = (mlpx_seg7_bcd_t*) malloc(sizeof(mlpx_seg7_bcd_t));
	uint8_t *ABCD = (uint8_t *)malloc(sizeof(uint8_t) * 4);

	ABCD[0] = A;
	ABCD[1] = B;
	ABCD[2] = C;
	ABCD[3] = D;

	for (int i = 0; i < 4; i++)
	{
		pinMode(ABCD[i], OUTPUT);
		digitalWrite(ABCD[i], LOW);
	}

	displays->seg7_BCD = (Seg7_BCD *)malloc(sizeof(Seg7_BCD) * count);
	displays->step = 0;
	displays->tempo = 0;
	displays->count = count;

	va_list list;
	va_start(list, count);

	for (int i = 0; i < count; i++)
	{
#ifdef DEBUG
		sprintf(strBuf, "\n%d : ABCD = %d,%d,%d,%d | alim = %d | CC = %d\n", i, displays->bits[0], displays->bits[1], displays->bits[2], displays->bits[3], alims[i], CathodeComune == 1);
		Serial.println(strBuf);
		Serial.println("test2");
#endif

		//displays->seg7_BCD[i]

		//displays->seg7_BCD[i] = create_Seg7_BCD(ABCD,
		//	va_arg(list, int),
		//	CathodeComune);

		displays->seg7_BCD[i].bits = ABCD;
		displays->seg7_BCD[i].power_pin = va_arg(list, int);
		displays->seg7_BCD[i].common_cathode = CathodeComune;

		pinMode(displays->seg7_BCD[i].power_pin, OUTPUT);
		pwoff_Seg7_BCD(&(displays->seg7_BCD[i]));
		zero_Seg7_BCD(&(displays->seg7_BCD[i]));
		pwon_Seg7_BCD(&(displays->seg7_BCD[i]));
	}

	va_end(list);

	//	return (displays);
}

void print_Mlpx_7Seg_BCD(Mlpx_7Seg_BCD *display, uint8_t *vals)
{
	if ((millis() - display->tempo) > delayBeforeShifting)
	{
		pwoff_Seg7_BCD(&(display->seg7_BCD[display->step]));
		display->step = (display->step + 1) % display->count;
		print_Seg7_BCD(&(display->seg7_BCD[display->step]), vals[display->step]);
		display->tempo = millis();
	}
}

void print_int_Mlpx_7Seg_BCD(Mlpx_7Seg_BCD *display, uint32_t val)
{
	uint8_t vals[10];
	intToDigits(val, vals, display->count);

	if ((millis() - display->tempo) > delayBeforeShifting)
	{
		pwoff_Seg7_BCD(&(display->seg7_BCD[display->step]));
		display->step = (display->step + 1) % display->count;
		print_Seg7_BCD(&(display->seg7_BCD[display->step]), vals[display->step]);
		display->tempo = millis();
	}
}

void pwoff_Mlpx_7Seg_BCD(Mlpx_7Seg_BCD *display)
{
	for (int i = 0; i < display->count; i++)
		pwoff_Seg7_BCD(&(display->seg7_BCD[i]));
}

void pwon_Mlpx_7Seg_BCD(Mlpx_7Seg_BCD *display)
{
	for (int i = 0; i < display->count; i++)
		pwon_Seg7_BCD(&(display->seg7_BCD[i]));
}

//----------------------------------------------------

uint8_t *millisToArray(uint32_t millis, uint8_t format, uint8_t *vec, uint8_t length)
{
	//	uint32_t time = millis / 1000;
	uint8_t sec = (millis / 1000) % 60;
	uint8_t min = (millis / 60000) % 60;
	uint8_t hrs = (millis / 3600000) % 24;

	for (int i = 0; i < length; i++)
		vec[i] = 0;

	int j = length - 1;
	if (format & FTIME_ss)
	{
		vec[j] = sec % 10;
		vec[j - 1] = (sec - vec[j]) / 10;
		j -= 2;
	}
	if (format & FTIME_mm)
	{
		vec[j] = min % 10;
		vec[j - 1] = (min - vec[j]) / 10;
		j -= 2;
	}
	if (format & FTIME_hh)
	{
		vec[j] = hrs % 10;
		vec[j - 1] = (hrs - vec[j]) / 10;
		j -= 2;
	}

	return vec;
}

uint8_t *intToArray(uint32_t n, uint8_t *buffer, uint8_t buffer_length)
{

	for (int i = 0; i < buffer_length; i++)
		buffer[i] = 0;

	for (uint8_t i = buffer_length; i > 0; i--)
	{
		buffer[i - 1] = n % 10;
		n = (n - (n % 10)) / 10;
	}

	return buffer;
}
