/*
 * GccApplication1.c
 *
 * Created: 11/19/2019 1:53:34 AM
 * Author : Imperial
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x080;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	   // ADEN: enables analog-to-digital conversion
	  // ADSC: starts the first conversion
	 // ADATE enables auto-triggering
	// Since we are in Free Running mode, a new conversion will trigger whenever the previous conversion completes
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;   // input
	DDRC = 0xFF; PORTC = 0x00;  // output
	DDRD = 0xFF; PORTD = 0x00; // output
	TimerOn();
	TimerSet(500);
	ADC_init();
	
	unsigned short my_short = PINA;
	unsigned char my_char = (char)my_short; // my_char = 0xCD
	unsigned char my_sol = (char)my_short;
	uint16_t rock;
	int test = 0;

	LCD_init();
	
    while (1) {
		rock = ADC;
		my_short = rock;
		my_char = (char)(my_short); // my_char = 0xAB
		if (test == 0) {
			my_sol = my_char;
			test++;
		}
		if (my_char == my_sol) {
			LCD_DisplayString(1, "Center");
		}
		else if ((my_char > (my_sol + 20)) &&  (my_char < (my_sol + 50) )){LCD_DisplayString(1, "Left");}
		else if ((my_char > (my_sol + 190)) &&  (my_char < (my_sol + 200) )){LCD_DisplayString(1, "Right");}
		else {LCD_DisplayString(1, "Center");}
		//LCD_DisplayString(1, my_char);
		while (!TimerFlag);
		TimerFlag = 0;

    }
    return 1;
}
