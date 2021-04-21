/** 
 ** Simple I/O Library (Arduino / Wiring Pi)
 ** \file sios.h
 ** \author Jérémie Rodez https://github.com/JRodez 
 ** \version 2.0
 */

#ifndef _sios_h
#define _sios_h

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#define INVERT_ROTAR 1
#define STEP_ROTAR 2

#define FTIME_ss 0b1
#define FTIME_mm 0b10
#define FTIME_hh 0b100

#ifndef ROTARDIR
#define ROTARDIR 0
#endif  

#ifdef __cplusplus
extern "C"
{
#endif

	/// \brief represents a 
	typedef struct seg7_bcd_s
	{
		uint8_t *bits;
		uint8_t power_pin;
		bool common_cathode;
	} Seg7_BCD;

	typedef struct mlpx_seg7_bcd_s
	{
		Seg7_BCD *seg7_BCD;
		int count;
		unsigned long tempo;
		uint8_t step;
	} Mlpx_7Seg_BCD;

	typedef enum rotation_e
	{
		Unmodified,
		CounterClock,
		CounterClockWise
	} Rotation;

	typedef struct s_button
	{
		uint8_t pin;
		unsigned long time_passed;
		bool cache;
		bool use_pull_up;
	} Button;

	typedef struct s_rotary_encoder
	{
		Button button;
		uint8_t pinClk;
		uint8_t pinDt;
		bool clkLastState;
		int counter;
	} RotaryEncoder;

	//-------------------------------------

	/// \brief Set time (ms) before considering a button is repressed.
	/// This delay is ignored when the button use pull_up.
	/// default is 10 ms
	void set_bounce_safety(unsigned long int delay);

	/// \brief Create a new Button
	void create_button(Button *button, uint8_t pin, bool use_pull_up);

	/// \brief Return the immediate state of the button :
	/// LOW if it's open or HIGH if it's closed, even if pull_up is set.
	bool read_button(Button *button);

	/// \brief Return the software state of  button :
	/// if state did not change since last call -> return false
	/// else if time between this call and the last one is inferior to bounce_safety -> return false
	/// else return button_pushed_now value
	bool is_pushed_button(Button *button);

	/// \brief Do stuff when button is pressed (button_pushed)
	void do_button(Button *button, void *(*function)(void *), void *arg);

	/// \brief Create a new Rotary Encoder
	void create_RotaryEncoder(RotaryEncoder *rotar, uint8_t pinClk, uint8_t pindt);

	/// \brief return Rotation.Unmodified, CounterClock nor CounterClockWise if it detects some change in its position
	Rotation hasTurned(RotaryEncoder *rotar);

	// -------------------------

	/// \brief Can be usefull, quite explicit.
	uint8_t *intToArray(uint32_t n, uint8_t *buffer, uint8_t buffer_length);
	/// \brief To set the time interval between multiplexing delays.
	void set_DispShiftDelay(unsigned int delay);

	/// \brief Init of Seg7_BCD type, give it its adress, A,B,C and D pin of the BCD cheap then if it's a Common Cathode, finaly the power pin.
	void create_Seg7_BCD(Seg7_BCD *display,
						 uint8_t A,
						 uint8_t B,
						 uint8_t C,
						 uint8_t D,
						 bool CommonCathode,
						 uint8_t power);

	/// \brief Quite explicit
	void print_Seg7_BCD(Seg7_BCD *display, uint8_t one_digit);

	/// \brief Power on the specified Seg7_BCD
	void pwon_Seg7_BCD(Seg7_BCD *display);

	/// \brief Power off the specified Seg7_BCD
	void pwoff_Seg7_BCD(Seg7_BCD *display);

	//-----------------------------------------

	/// \brief Init of Mlpx_7Seg_BCD  type, give it its adress, A,B,C and D pin of the BCD cheap, if it's a Common Cathode, then give it the number of the 7 segments displays and their power pin.
	void create_Mlpx_7Seg_BCD(Mlpx_7Seg_BCD *displays,
							  uint8_t A,
							  uint8_t B,
							  uint8_t C,
							  uint8_t D,
							  bool CathodeComune,
							  uint8_t count,
							  ...);

	/// \brief Prints an array of digits
	void print_Mlpx_7Seg_BCD(Mlpx_7Seg_BCD *disp, uint8_t *vals);

	/// \brief Quite explicit
	void print_int_Mlpx_7Seg_BCD(Mlpx_7Seg_BCD *display, uint32_t val);

	/// \brief Power off the specified Mlpx_7Seg_BCD
	void pwoff_Mlpx_7Seg_BCD(Mlpx_7Seg_BCD *aff);

	/// \brief Power on the specified Mlpx_7Seg_BCD
	void pwon_Mlpx_7Seg_BCD(Mlpx_7Seg_BCD *aff);

	/// \brief Transform from millis like variable into a digit array following the format specified by FTIME addition (see example)
	uint8_t *millisToArray(uint32_t millis, uint8_t format, uint8_t *vec, uint8_t length);

#ifdef __cplusplus
}
#endif

#endif
