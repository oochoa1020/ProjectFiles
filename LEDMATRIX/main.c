/*
 * LEDMATRIX.c
 *
 * Created: 11/23/2019 9:53:27 AM
 * Author : Imperial
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

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

unsigned char Horiz[] = {0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000, 0b00100000, 0b01000000, 0b10000000};
unsigned char Vert[] = {0b11111110, 0b11111101, 0b11111011, 0b11110111, 0b11101111, 0b11011111, 0b10111111, 0b01111111};
unsigned char ex = 0b00001000;
unsigned char ex2 = 0b10000000;
int main(void)
{
    DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	TimerSet(50);
	TimerOn();
	//unsigned char row = 0;
	//PORTB = 4top4left
	//PORTD = 4right4bottom
    while (1) 
    {
		
		
		//Diagonal
		//PORTB = 0b00000001;
		PORTD = 0b11111110;
		
		for (unsigned int i = 0; i < 8; ++i){
			PORTB = 0b00000001;
			while (!TimerFlag);
			TimerFlag = 0;
			for (unsigned int j = 0; j < 8; ++j){
				PORTB = PORTB << 1;
				//PORTD = PORTD << 1;
				//++PORTD;
				if (j == 7){}
				else{
				while (!TimerFlag);
				TimerFlag = 0;
				}
			}
			PORTD = PORTD << 1;
			++PORTD;
			while (!TimerFlag);
			TimerFlag = 0;
		}
		
		
		for (unsigned int j = 0; j < 8; ++j){
			PORTB = 0b00000001;
			PORTD = 0b01111111;
			unsigned int k = j;
			while (k != 0) {
				PORTD = PORTD >> 1;
				PORTD |= 0b10000000;
				--k;
			}
			while (!TimerFlag);
			TimerFlag = 0;
			for (unsigned int i = 0; i < 8; ++i){
				PORTB = PORTB << 1;
				if (PORTD != 0b10000000){
					PORTD = PORTD << 1;
					++PORTD;
				}
				if (i != 7){
					while (!TimerFlag);
					TimerFlag = 0;
				}
			}
		}
		
		for (unsigned int j = 0; j < 8; ++j){
			PORTB = 0b00000010;
			PORTD = 0b11111110;
			unsigned int k = j;
			while (k != 0) {
				PORTB = PORTB << 1;
				--k;
			}
			while (!TimerFlag);
			TimerFlag = 0;
			for (unsigned int i = 0; i < 8; ++i){
				if (PORTB != 0b10000000){
					PORTB = PORTB << 1;
					PORTD = PORTD << 1;
					++PORTD;
				}
				if (i != 7){
					while (!TimerFlag);
					TimerFlag = 0;
				}
			}
		}
    }
}

