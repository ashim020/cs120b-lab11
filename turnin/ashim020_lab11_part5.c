/*	Author: Andrew Shim
 *  	Partner(s) Name: 
 *	Lab Section: 21
 *	Assignment: Lab # 11 Exercise # 5
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 * 	Demo: https://drive.google.com/file/d/1lSTIteAusztkDRAK77keKSVbtgIyyzNW/view?usp=sharing
 */
#include <avr/io.h>
#include <timer.h>
#include <keypad.h>
#include <io.h>
#include <scheduler.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char pos = 2;

enum Button_State {B_Start, B_INIT, B_DOWN, B_UP, B_RELEASE};
int Button_Tick(int state) {
    switch(state) {
	case B_Start:
	    state = B_INIT;
	    break;
	case B_INIT:
	    if((~PIND & 0x03) == 0x01) {
		state = B_DOWN;
	    }
	    else if((~PIND & 0x03) == 0x02) {
		state = B_UP;
	    }
	    else {
		state = B_INIT;
	    }
	    break;
	case B_DOWN:
	    state = B_RELEASE;
	    break;
	case B_UP:
	    state = B_RELEASE;
	    break;
	case B_RELEASE:
	    if((~PIND & 0x03) == 0x00) {
		state = B_INIT;
	    } else {
		state = B_RELEASE;
	    }
	    break;
	default:
	    state = B_Start;
	    break;
    }
    switch(state) {
	case B_DOWN:
	    pos = 18;
	    break;
	case B_UP:
	    pos = 2;
	    break;
	default:
	    break;
    }
    return state;
}

enum LCD_States {LCD_Start, LCD_INIT, LCD_LOSE};
unsigned char phrase[] = "YOU LOSE.";
unsigned char display[] = "                &        &                ";
unsigned char ind = 0;
int LCD_Tick(int state) {
    switch(state) {
	case LCD_Start:
	    state = LCD_INIT;
	    break;
	case LCD_INIT:
	    if(display[pos + ind] == '&') {
		state = LCD_LOSE;
	    } else {
		state = LCD_INIT;
	    }
	    break;
	case LCD_LOSE:
	    if((~PIND & 0x07) == 0x04) {
		state = LCD_INIT;
	    } else {
		state = LCD_LOSE;
	    }
	    break;
	default:
	    state = LCD_Start;
	    break;
    }
    switch(state) {
	case LCD_INIT:
	    for(int i = 1; i <= 32; i++) {
		LCD_Cursor(i);
		LCD_WriteData(display[(i + ind) % 32]);
	    }
	    LCD_Cursor(pos);
	    if(ind >= 32) {
		ind = 0;
	    } else {
		ind++;
	    }
	    break;
	case LCD_LOSE:
	    for(ind = 1; ind <= sizeof(phrase) - 1; ind++) {
		LCD_Cursor(ind);
		LCD_WriteData(phrase[ind - 1]);
	    }
	    pos = 2;
	    ind = 0;
	    break;
	default:
	    break;
    }
    return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0xFF; PORTA = 0x00;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0x00; PORTD = 0xFF;

    LCD_init();
    static task task1, task2;
    task *tasks[] = {&task1, &task2};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    const char start = -1;

    task1.state = start;
    task1.period = 10;
    task1.elapsedTime = task1.period;
    task1.TickFct = &Button_Tick;

    task2.state = start;
    task2.period = 20;
    task2.elapsedTime = task2.period;
    task2.TickFct = &LCD_Tick;

    TimerSet(10);
    TimerOn();
    
    unsigned short i;
    while(1) {
	for(i = 0; i < numTasks; i++) {
	    if(tasks[i]->elapsedTime == tasks[i]->period) {
		tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
		tasks[i]->elapsedTime = 0;
	    }
	    tasks[i]->elapsedTime += 1;
	}
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 0;
}
