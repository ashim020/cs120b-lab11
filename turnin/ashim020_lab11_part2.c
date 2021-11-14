/*	Author: Andrew Shim
 *  	Partner(s) Name: 
 *	Lab Section: 21
 *	Assignment: Lab # 11 Exercise # 2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 * 	Demo: https://drive.google.com/file/d/1WdabA2wogcg8imLb8XRkD7Wr1DiZCCmf/view?usp=sharing
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
		    PORTB = 0x1F; 
		    break;
	        case '1': 
		    PORTB = 0x01;
		    break;
	        case '2':
		    PORTB = 0x02;
		    break;
	        case '3':
		    PORTB = 0x03;
		    break;
	        case '4':
		    PORTB = 0x04;
		    break;
	        case '5':
		    PORTB = 0x05;
		    break;
	        case '6':
		    PORTB = 0x06;
		    break;
	        case '7':
		    PORTB = 0x07;
		    break;
	        case '8':
		    PORTB = 0x08;
	   	    break;
	        case '9':
		    PORTB = 0x09;
		    break;
	        case 'A':
		    PORTB = 0x0A;
		    break;
	        case 'B':
		    PORTB = 0x0B;
		    break;
	        case 'C':
		    PORTB = 0x0C;
		    break;
	        case 'D':
		    PORTB = 0x0D;
		    break;
	        case '*':
		    PORTB = 0x0E;
		    break;
	        case '0':
		    PORTB = 0x00;
		    break;
	        case '#':
		    PORTB = 0x0F;
		    break;
	        default:
		    PORTB = 0x1B;
		    break;
	    }
	    break;
	default:
	    break;
    }
    return state;
}

enum LCD_States {LCD_Start, LCD_INIT};
unsigned char phrase[] = "CS120B is Legend... wait for it DARY!           ";
unsigned char pos = 0x00;
unsigned char lcd[16];
int LCD_Tick(int state) {
    switch(state) {
	case LCD_Start:
	    state = LCD_INIT;
	    break;
	case LCD_INIT:
	    state = LCD_INIT;
	    break;
	default:
	    state = LCD_Start;
	    break;
    }
    switch(state) {
	case LCD_INIT:
	    for(int j = 0; j < 16; j++) {
		lcd[j] = phrase[(j + pos) % 48];
	    }
	    pos = (pos + 1) % 48;
	    LCD_DisplayString(1, lcd);
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
