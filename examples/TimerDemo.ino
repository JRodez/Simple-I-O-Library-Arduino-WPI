#define VERBOSE

#include <Arduino.h>

#include <siodm.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <HardwareSerial.h>
#define STEP_BUTTONS 10

Mlpx_7Seg_BCD disp;
uint8_t ch[4];
uint32_t time = 0, timeCache = 0;
Button buttonAdd, buttonSub, buttonSetting;
void (*resetFunc)(void) = 0; // The reset function

char buffer[64];

/// \brief simple sub to function
void sub(uint32_t *a)
{

	if (*a < STEP_BUTTONS)
		*a = 0;
	else
		*a -= STEP_BUTTONS;
}

/// \brief simple add to function
void add(uint32_t *a)
{

	if (*a + STEP_BUTTONS < 3600)
		(*a) += STEP_BUTTONS;
	else
		*a = 3600 - 1;
}

void setTime()
{
	uint32_t blinkTimer = millis();
	bool off = true;

	do
	{
		do_button(&buttonAdd, add, &time);
		do_button(&buttonSub, sub, &time);

		if (!off)
			print_Mlpx_7Seg_BCD(&disp, millisToArray(time * 1000, FTIME_ss + FTIME_mm, ch, 4));

		if (millis() - blinkTimer > 500)
		{
			if ((off = !off))
				pwoff_Mlpx_7Seg_BCD(&disp);
			blinkTimer = millis();
		}
	} while (!is_pushed_button(&buttonSetting));

	timeCache = millis();
}

void bip()
{
	//put any action on timer end
	for (int i = 0; i < 8; i++)
	{
		pwon_Mlpx_7Seg_BCD(&disp);
		digitalWrite(13, HIGH);
		delay(500);
		digitalWrite(13, LOW);
		pwoff_Mlpx_7Seg_BCD(&disp);
		delay(500);
	}
	resetFunc(); //call reset
}

void setup()
{

	create_Mlpx_7Seg_BCD(&disp, 5, 4, 3, 2, true, 4, 11, 10, 9, 8);
	create_button(&buttonAdd, A0, true);
	create_button(&buttonSub, A1, true);
	create_button(&buttonSetting, A2, true);
	pinMode(13, OUTPUT);

	Serial.begin(9600);
	Serial.println("Débuts");

	set_DispShiftDelay(3);
	setTime();
}

void loop()
{

	if (is_pushed_button(&buttonSetting))
		setTime();

	if (timeCache + 1000 < millis())
	{
		if (time <= 1)
			bip();
		else
			time -= 1;

		timeCache = millis();
	}

	print_Mlpx_7Seg_BCD(&disp, millisToArray(time * 1000, FTIME_ss + FTIME_mm, ch, 4));
}