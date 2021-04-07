#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <curses.h>
#include <string.h>



double* stack;
int stackIndex = 0; // where stack is pointing (not a literal pointer) to 
int stackSize = 1;
/* Stack size is fundementally different from stack index,
 * the code doesn't pop and push values as needed like a proper stack,
 * it will go in the middle and perform calculations for the sake of brevity,
 * the stack size will have the actual size of the stack while the index can vary
 * I know this stack implementation is jank, doing it this way is just easier
 */

int main(int argc,char** argv) {
	char input; // input
	char strIn[100] = {0}; // string input
	double registers[26] = {0}; // registers, capital letters
	double temp = 0; // general purpose temp var
	int intTemp = 0;
	stack = malloc(sizeof(double)); 
	if(stack == NULL) {
		fprintf(stderr,"Stack couldn't be initialized");
		return(1);
	}

	initscr();
	cbreak();
	noecho();
	nonl();
	intrflush(stdscr,false);
	keypad(stdscr,true);

	while(1) {
		if(stackIndex+1 < stackSize) {
			stackSize--;
			stack = realloc(stack,stackSize*sizeof(double));
			stack[stackSize-1] = 0;
			continue; // acts as a loop if stack is *too* small
		}
		if(stackIndex >= stackSize) {
			stackSize++;
			stack = realloc(stack,stackSize*sizeof(double));
			stack[stackSize-1] = 0;
			continue; // same as above but for *too* big of a stack
		}
		clear();
		refresh();
		for(int i = stackSize-1;i>=0;i--) {
			printw("%d\t%lG\t%c\n",i,stack[i],i==stackIndex?'-':' ');
		}
		for(int i = 0;i<26;i++) {
			if(registers[i] != 0) {
				printw("%c:%G,",0x61+i,registers[i]);
			}
		}
		input = getch();
		if(input == KEY_ENTER || input == '\n' || input == '\r') {
			stackIndex++;
			continue;
		}
		if((input >= 0x41 || input <= 0x2F )&& input != 0x2E) { // non numerical input
			switch(input) {
				case 'u' : // register popping
					printw("input in a register to pop");
					input = getch();
					if(input > 0x7A || input < 0x61) {continue;}
					stack[stackIndex] = registers[input-0x61];
					stackIndex++;
					registers[input-0x61] = 0;
					continue;
				case 'U' : // register pushing
					printw("input in a register to push");
					input = getch();
					if(input > 0x7A || input < 0x61 || (stack[stackIndex] == 0 && stackIndex == 0)) {continue;}
					if(stack[stackIndex] == 0) {stackIndex--;}
					registers[input-0x61] = stack[stackIndex];
					stack[stackIndex] = 0;
					continue;
				case 'q' : // square rooting
					if(stack[stackIndex] == 0) {stackIndex--;} // TODO reduce ^C ^V here
					stack[stackIndex] = sqrt(stack[stackIndex]);
					stackIndex++;
					continue;
				case 'x' :
					if(stack[stackIndex] == 0) {stackIndex--;}
					stack[stackIndex] = pow(stack[stackIndex],(float)2);
					stackIndex++;
					continue;
				case 'w' : // switching values
					temp = stack[stackIndex-2];
					stack[stackIndex-2] = stack[stackIndex-1];
					stack[stackIndex-1] = temp;
					temp = 0;
					continue;
				case 'C' : // clear stack
					stackSize = 1;
					stack = realloc(stack,sizeof(double));
					stackIndex = 0;
					stack[0] = 0;
					stack[1] = 0;
					continue;
				case 'c' : // clear current value
					if(stack[stackIndex] == 0 && stackIndex > 0) {stackIndex--;}
					stack[stackIndex] = 0;
					continue;
				case 'p' : // pi
					stack[stackIndex] = M_PI;
					stackIndex++;
					continue;
				case 'o' : // e
					stack[stackIndex] = M_E;
					stackIndex++;
					continue;
				case 'i' : // manual input
					echo();
					printw("manual input: ");
					refresh();
					noecho();
					scanf("%lg",&stack[stackIndex]);
					stackIndex++;
					continue;
				case '-' : // invert
					stack[stackIndex] *= -1;
					
			}
			// below here is all arithmetic operations
			if(stackSize<2 || (stack[stackIndex] == 0 && stackSize == 2)) {continue;} // edge case
			stackIndex-= stack[stackIndex]==0?2:1;
			/* all operations in this switch have 2 operands
			 * usually index will go down by 2 for this
			 * but if user has just put in a number,
			 * that number needs to be used so it will go down by 1
			 */
			switch(input) {
				case 'a' :
					stack[stackIndex] += stack[stackIndex+1];
					break;
				case 's' :
					stack[stackIndex] -= stack[stackIndex+1];
					break;
				case 'm' :
					stack[stackIndex] *= stack[stackIndex+1];
					break;
				case 'd' :
					stack[stackIndex] /= stack[stackIndex+1];
					break;
				case 'e' : 
					stack[stackIndex] = pow(stack[stackIndex],stack[stackIndex+1]);
					break;
				case 'l' : // index+1 being base
					stack[stackIndex] = (log(stack[stackIndex]))/(log(stack[stackIndex+1]));
					break;
				default: 
					stackIndex++; // so top value doesn't get cleared
			}
			stackIndex++; // "popping" top value since there is only 1 result
			stack[stackIndex] = 0; 
		} else { // number input
			//setting temp variables
			intTemp = 0;
			memset(strIn,0,sizeof(strIn)/sizeof(char));
			if(isfinite(stack[stackIndex]) == 0) {continue;} // only edit real numbers
			
			strfromd(strIn,100,"%G",stack[stackIndex]); // turning current stack value to string

			for(intTemp = 0;strIn[intTemp] != 0;intTemp++) {}

			if(input == KEY_BACKSPACE || input == '\b') {
				strIn[intTemp] = 0;
				intTemp = 0; // TODO make resetting these vars a function
				memset(strIn,0,sizeof(strIn)/sizeof(char));
				continue;
			}

			if(input == '.' && (float)(int)stack[stackIndex] == stack[stackIndex]) {
				strIn[intTemp] = '.';
				input = getch();
				strIn[intTemp+1] = input;
			} else {
				strIn[intTemp] = input;
			}

			stack[stackIndex] = atof(strIn);
			
			//resetting intTemp variables
			intTemp = 0;
			memset(strIn,0,sizeof(strIn));
		}
	}
	free(stack);
	endwin();
	return(0);
}
