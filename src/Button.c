/** 
 ** \file Button.c
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

unsigned int bounce_safety = 10;


void set_bounce_safety(unsigned long int delay)
{
	bounce_safety = delay;
}

/// \brief Create a new Button
void create_button(Button *button, uint8_t pin, bool use_pull_up)
{

	button->pin = pin;
	button->time_passed = 0;
	button->use_pull_up = use_pull_up;
	button->cache = false;

	if (button->use_pull_up)
#ifndef WIRINGPI
		pinMode(button->pin, INPUT_PULLUP);
#else
	{
		pinMode(button->pin, INPUT);
		pullUpDnControl(button->pin, PUD_UP);
	}
#endif
	else
		pinMode(button->pin, INPUT);
}

/// \brief Return the immediate state of the button :
/// LOW if it's open or HIGH if it's closed, even if pull_up is set.
bool read_button(Button *button)
{
	if (button->use_pull_up)
		return (digitalRead(button->pin) == LOW);
	else
		return (digitalRead(button->pin) == HIGH);
}

/// \brief Return the software state of the button :
/// if state did not change since last call -> return false
/// else if time between this call and the last one is inferior to bounce_safety -> return false
/// else return button_pushed_now value
bool is_pushed_button(Button *button)
{
	bool state = false;
	bool read = read_button(button);
	if (read == true)
	{

		if ((read != button->cache) && ((millis() - button->time_passed) > bounce_safety))
		{
			state = true;
		}
		button->time_passed = millis();
	}
	button->cache = read;

	return (state);
}


/// \brief Do stuff when button is pressed (button_pushed)
void do_button(Button *button, void *(*function)(void *), void *arg)
{
	if (is_pushed_button(button))
		function(arg);
}

void create_RotaryEncoder(RotaryEncoder *rotar, uint8_t pinClk, uint8_t pindt)
{
#ifndef WIRINGPI
	pinMode(pinClk, INPUT_PULLUP);
	pinMode(pindt, INPUT_PULLUP);
#else
	pinMode(pinClk, INPUT);
	pinMode(pindt, INPUT);
	pullUpDnControl(pinClk, PUD_UP);
	pullUpDnControl(pindt, PUD_UP);
#endif

	rotar->pinClk = pinClk;
	rotar->pinDt = pindt;
	//rotar->counterClock = counterClock;
	//rotar->counterClockWise = counterClockWise;
	rotar->clkLastState = digitalRead(pinClk);
	rotar->counter = 0;
}

Rotation hasTurned(RotaryEncoder *rotar)
{
	// Lecture des statuts actuels
	bool clkStateNow = digitalRead(rotar->pinClk);
	//Serial.println(clkStateNow);
	// Vérification de changement
	if (clkStateNow != rotar->clkLastState)
	{
		if (clkStateNow != digitalRead(rotar->pinDt))
		{
			// Pin_CLK a changé en premier
			if (INVERT_ROTAR)
				rotar->counter--;
			else
				rotar->counter++;
		}
		else
		{
			// Sinon Pin_DT achangé en premier
			if (INVERT_ROTAR)
				rotar->counter++;
			else
				rotar->counter--;
		}
	}

	// Préparation de la prochaine exécution:
	rotar->clkLastState = clkStateNow;
	if (rotar->counter >= STEP_ROTAR)
	{
		rotar->counter = 0;
#ifdef VERBOSE
		Serial.println("Clockwise");
#endif
		return (CounterClock);
	}
	else if (rotar->counter <= -STEP_ROTAR)
	{
		rotar->counter = 0;
#ifdef VERBOSE
		Serial.println("Counterclockwise");
#endif
		return (CounterClockWise);
	}
	else
		return (Unmodified);
}
