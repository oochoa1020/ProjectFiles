/*
 * Project.c
 *
 * Created: 12/1/2019 2:39:55 PM
 * Author : Imperial
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>


unsigned char Horiz[] = {0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000, 0b00100000, 0b01000000, 0b10000000};
unsigned char Vert[] = {0b11111110, 0b11111101, 0b11111011, 0b11110111, 0b11101111, 0b11011111, 0b10111111, 0b01111111};
unsigned char load = 0;
unsigned char rock = 0;
unsigned int board[8][8] = {0};
unsigned int renderBoard[7] = {0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111};
unsigned int renderBoardC[7] = {0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111};
unsigned char anded[] = {0b11111110,0b11111101,0b11111011,0b11110111,0b11101111,0b11011111,0b10111111,0b01111111};
char player= 'g';
unsigned char horizWon = 0;
unsigned char vertWon = 0;
unsigned int haveWon = 0;
unsigned int choice = 0;
unsigned int winCheck = 1;
unsigned short my_short = 0;
unsigned char my_char = 0; // my_char = 0xCD
unsigned char my_sol = 0;
uint16_t joy;
int joytest = 0;
unsigned int joyTime = 0;
unsigned int joyMod = 0;
char direction = 'c';
unsigned int curPick = 0;
volatile unsigned char TimerFlag = 0;
unsigned int startVal = 0;
unsigned int startMod = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

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


enum sticks{noStick, readStick}stick;
enum games{wait, start, check, winner}game;
enum screens {init1, init2,updateBoard,playing1,playing2,playing3,playing4,playing5,playing6,playing7, done} screen;

void joystick() {
	switch(stick) {
		case noStick:
		{
			break;
		}
		case readStick:
		{
			++joyTime;
			if (joyTime % 5 == 0) {
				++joyMod;
			}
			if (joyMod > 100) {
				joyTime = 0;
				joyMod = 0;
				joy = ADC;
				my_short = joy;
				my_char = (char)(my_short); // my_char = 0xAB
				if (joytest == 0) {
					my_sol = my_char;
					joytest++;
				}
				if (my_char == my_sol) {
					direction = 'c';
				}
				//else if ((my_char > (my_sol + 20)) &&  (my_char < (my_sol + 50) )){
				else if ((my_char < (my_sol + 50) )){
					if (curPick == 0) {
						curPick = curPick;
						board[0][curPick] = board[0][curPick];
					}
					else {
						//board[0][curPick] = 0;
						//curPick = curPick - 1;
						board[0][curPick-1] = board[0][curPick];
						board[0][curPick] = 0;
						curPick = curPick - 1;
					}
					//board[0][curPick] = 1;
				}
				else if ((my_char > (my_sol + 190)) &&  (my_char < (my_sol + 200) )){
					if (curPick == 6) {
						curPick = curPick;
						board[0][curPick] = board[0][curPick];
					}
					else {
						//board[0][curPick] = 0;
						//curPick = curPick + 1;}
						//board[0][curPick] = 1;
						board[0][curPick+1] = board[0][curPick];
						board[0][curPick] = 0;
						curPick = curPick + 1;
				}
				}
				else {
					direction = 'c';
				}
			}
		}
		
	}
}

void play() {
	switch(game) {
		
		case wait:
		{
			break;
		}
		case start:
		{
			++startVal;
			if (startVal % 5 == 0) {
				++startMod;
			}
			if (startMod > 100) {
				startMod = 0;
				startVal = 0;
				if ((~PINA & 0x04) == 0x04){
					game = check;
					choice = curPick;
				}
				
			}
				
			break;
		}
		case check:
		{
			for (unsigned int j = 7; j >=1; --j){
				if (board[j][choice] == 0) {
					if (player == 'g') {
						board[j][choice] = 1;
						player = 'r';
						winCheck = 1;
						board[0][choice] = 2;
					}
					else if (player == 'r') {
						board[j][choice] = 2;
						player = 'g';
						winCheck = 2;
						board[0][choice] = 1;
					}
					game = winner;
					break;
				}
			}
			game = winner;
			break;
		}
		case winner:
		{
			for (unsigned int q = 7; q >= 1; --q){
				for (unsigned int j = 0; j <=7; ++j){
					if (board[q][j] == winCheck){
						++horizWon;
						if (horizWon >= 4) {
							haveWon = winCheck;
						}
					}
					else {
						horizWon = 0;
					}
				}
			}
			if (haveWon == winCheck) {
				screen = done;
				break;
			}
			horizWon = 0;
			for (unsigned int q = 0; q <= 7; ++q){
				for (unsigned int j = 7; j >=1; --j){
					if (board[j][q] == winCheck){
						++vertWon;
						if (vertWon >= 4) {
							haveWon = winCheck;
							screen = done;
							break;
						}
					}
					else {
						vertWon = 0;
					}
				}
			}
			vertWon = 0;
			game = start;
			break;
		}
	}
};

void display() {
	switch(screen) {
		
		case init1:
		{	
			++load;
			PORTC = 0b11111111;
			PORTB = 0b00111100;
			PORTD = 0b11000011;
			if (load % 5 == 0) {
				++rock;
			}
			if (rock > 100) {
				rock = 0;
				load = 0;
				screen = init2;
				if ((~PINA & 0x04) == 0x04){
					screen = updateBoard;
					game = start;
					stick = readStick;
				}
			}
			/*if ((~PINA & 0x04) == 0x04){
				screen = done;
			}*/
			break;
		}
		case init2:
		{
			++load;
			PORTC = 0b11111111;
			PORTB = 0b00000000;
			PORTD = 0b11111111;
			if (load % 5 == 0) {
				++rock;
			}
			if (rock > 100) {
				load = 0;
				rock = 0;
				screen = init1;
				if ((~PINA & 0x04) == 0x04){
					screen = updateBoard;
					game = start;
					stick = readStick;
				}
			}
			/*if ((PINA & 0x04) == 0x04){
				screen = done;
			}*/
			break;
		}
		
		case updateBoard:{
			load++;
			if (load % 5 == 0) {
				++rock;
			}
			if (rock > 100) {
				rock = 0;
				load = 0;
				if ((~PINA & 0x08) == 0x08){
					stick = readStick;
					for (unsigned int q = 7; q >= 1; --q){
						for (unsigned int j = 0; j <=7; ++j){
							board[q][j] = 0;
						}
					}
					board[0][choice] = 0;
					curPick = 0;
					choice = 0;
					board[0][0] = 1;
					player = 'g';
					winCheck = 1;
					haveWon = 0;
					screen = init1;
					game = wait;
				}
				break;
			}
			
			
			
			PORTB = 0b00000000;
			PORTD = 0b11111111;
			PORTC = 0b11111111;
			renderBoard[0] = 0b11111111;
			renderBoard[1] = 0b11111111;
			renderBoard[2] = 0b11111111;
			renderBoard[3] = 0b11111111;
			renderBoard[4] = 0b11111111;
			renderBoard[5] = 0b11111111;
			renderBoard[6] = 0b11111111;
			renderBoardC[0] = 0b11111111;
			renderBoardC[1] = 0b11111111;
			renderBoardC[2] = 0b11111111;
			renderBoardC[3] = 0b11111111;
			renderBoardC[4] = 0b11111111;
			renderBoardC[5] = 0b11111111;
			renderBoardC[6] = 0b11111111;
			//renderBoard[0] = PORTD & anded[0];
			//PORTD = renderBoard[7];
			for (unsigned int h = 0; h <=6; ++h) {
				for (unsigned int k = 0; k <= 7; ++k) {
					if(board[k][h] == 1) {
						renderBoard[h] = renderBoard[h] & anded[k];
					}
				}
			}
			for (unsigned int h = 0; h <=6; ++h) {
				for (unsigned int k = 0; k <= 7; ++k) {
					if(board[k][h] == 2) {
						renderBoardC[h] = renderBoardC[h] & anded[k];
					}
				}
			}
			screen = playing1;
		}
		case playing1:
		{
			/*PORTC = 0b11111111;
			PORTB = 0b00000001;
			PORTD = 0b00000000;*/
			PORTB = 0b00000001;
			PORTC = renderBoardC[0];
			PORTD = renderBoard[0];
			screen = playing2;
			break;
		}
		case playing2:
		{
			PORTB = 0b00000010;
			PORTC = renderBoardC[1];
			PORTD = renderBoard[1];
			screen = playing3;
			break;
		}
		case playing3:
		{
			PORTB = 0b00000100;
			PORTC = renderBoardC[2];
			PORTD = renderBoard[2];
			screen = playing4;
			break;
		}
		case playing4:
		{
			PORTB = 0b00001000;
			PORTC = renderBoardC[3];
			PORTD = renderBoard[3];
			screen = playing5;
			break;
		}
		case playing5:
		{
			PORTB = 0b00010000;
			PORTC = renderBoardC[4];
			PORTD = renderBoard[4];
			screen = playing6;
			break;
		}
		case playing6:
		{
			PORTB = 0b00100000;
			PORTC = renderBoardC[5];
			PORTD = renderBoard[5];
			screen = playing7;
			break;
		}
		case playing7:
		{
			PORTB = 0b01000000;
			PORTC = renderBoardC[6];
			PORTD = renderBoard[6];
			screen = updateBoard;
			break;
		}
		
		case done:
		{
			if (winCheck == 1) {
			PORTB = 0b11111111;
			PORTC = 0b11111111;
			PORTD = 0b00000000;
			}
			else {
				PORTB = 0b11111111;
				PORTD = 0b11111111;
				PORTC = 0b00000000;
			}
			load++;
			game = wait;
			stick = noStick;
			if (load % 5 == 0) {
				++rock;
			}
			if (rock > 100) {
				rock = 0;
				load = 0;
				if ((~PINA & 0x08) == 0x08){
					stick = readStick;
					for (unsigned int q = 7; q >= 1; --q){
						for (unsigned int j = 0; j <=7; ++j){
							board[q][j] = 0;
						}
					}
					board[0][choice] = 0;
					curPick = 0;
					choice = 0;
					board[0][0] = 1;
					player = 'g';
					winCheck = 1;
					haveWon = 0;
					screen = init1;
					break;
				}
			}
			break;
		}
		default:
		{
			screen = init1;
			break;
		}
	}
	switch(screen) {
		case init1:
		{
			//PORTB = 0x01;
			break;
		}
		case init2:
		{
			//PORTB = 0x01;
			break;
		}
		case done:
		{
			//PORTB = 0x02;
			break;
		}
		
	}
}


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	TimerSet(1);
	TimerOn();
	screen = init1;
	game = wait;
	stick = readStick;
	ADC_init();
	
	my_short = PINA;
	my_char = (char)my_short; // my_char = 0xCD
	my_sol = (char)my_short;
	int joytest = 0;
	board[0][0] = 1;
    while (1) 
    {
		joystick();
		play();
		display();
		//++load;
		while (!TimerFlag);
		TimerFlag = 0;
    }
}

