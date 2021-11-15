/*	Author: Andrew Shim
 *  	Partner(s) Name: 
 *	Lab Section: 21
 *	Assignment: Lab # 11 Exercise # 3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 * 	Demo: https://drive.google.com/file/d/1IXZqgTds_YRVzM3BmCXyWu246BFpMOxa/view?usp=sharing
 */
#include <avr/io.h>
#include <timer.h>
#include <keypad.h>
#include <io.h>
#include <scheduler.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char x = 0x00;
unsigned char tmpB = 0x00;
enum Keypad_State {KP_Start, KP_INIT};
int Keypad_Tick(int state) {
    x = GetKeypadKey();
    switch(state) {
	case KP_Start:
	    state = KP_INIT;
	    break;
	case KP_INIT:
	    state = KP_INIT;
	    break;
	default:
	    state = KP_Start;
	    break;
    }
    switch(state) {
	case KP_INIT:
	    switch(x) {
	        case '\0': 
		    tmpB = 0x1F; 
		    break;
	        case '1': 
		    tmpB = 0x01;
		    break;
	        case '2':
		    tmpB = 0x02;
		    break;
	        case '3':
		    tmpB = 0x03;
		    break;
	        case '4':
		    tmpB = 0x04;
		    break;
	        case '5':
		    tmpB = 0x05;
		    break;
	        case '6':
		    tmpB = 0x06;
		    break;
	        case '7':
		    tmpB = 0x07;
		    break;
	        case '8':
		    tmpB = 0x08;
	   	    break;
	        case '9':
		    tmpB = 0x09;
		    break;
	        case 'A':
		    tmpB = 0x0A;
		    break;
	        case 'B':
		    tmpB = 0x0B;
		    break;
	        case 'C':
		    tmpB = 0x0C;
		    break;
	        case 'D':
		    tmpB = 0x0D;
		    break;
	        case '*':
		    tmpB = 0x0E;
		    break;
	        case '0':
		    tmpB = 0x00;
		    break;
	        case '#':
		    tmpB = 0x0F;
		    break;
	        default:
		    tmpB = 0x1B;
		    break;
	    }
	    PORTB = tmpB;
	    break;
	default:
	    break;
    }
    return state;
}

enum LCD_States {LCD_Start, LCD_INIT, LCD_DISPLAY};
int LCD_Tick(int state) {
    switch(state) {
	case LCD_Start:
	    state = LCD_INIT;
	    break;
	case LCD_INIT:
	    if(tmpB == 0x1F) {
		state = LCD_INIT;
	    } else {
		state = LCD_DISPLAY;
	    }
	    break;
	case LCD_DISPLAY:
	    state = LCD_INIT;
	    break;
	default:
	    state = LCD_Start;
	    break;
    }
    switch(state) {
	case LCD_DISPLAY:
	    LCD_Cursor(1);
	    LCD_WriteData(x);
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
    DDRC = 0xF0; PORTC = 0x0F;
    DDRD = 0xFF; PORTD = 0x00;

    LCD_init();
    static task task1, task2;
    task *tasks[] = {&task1, &task2};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    const char start = -1;

    task1.state = start;
    task1.period = 10;
    task1.elapsedTime = task1.period;
    task1.TickFct = &Keypad_Tick;

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
