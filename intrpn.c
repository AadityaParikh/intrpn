#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <curses.h>


int main(int argc,char** argv) {
	char input; // input
	double doubleIn = 0; // double parse of input
	double registers[26] = {0}; // registers, capital letters
	double* stack;
	double temp = 0; // general purpose temp var
	stack = malloc(sizeof(double)); 
	if(stack == NULL) {
		fprintf(stderr,"Stack couldn't be initialized");
		return(1);
	}
	int stackIndex = 0; // where stack is pointing (not a literal pointer) to 
	int stackSize = 1;
	/* Stack size is fundementally different from stack index,
	 * the code doesn't pop and push values as needed like a proper stack,
	 * it will go in the middle and perform calculations for the sake of brevity,
	 * the stack size will have the actual size of the stack while the index can vary
	 */

	initscr();
	cbreak();
	noecho();
	nonl();
	intrflush(stdscr,false);
	keypad(stdscr,true);

	while(1) {
		clear();
		refresh();
		for(int i = stackSize-1;i>=0;i--) {
			printw("%d\t%G\n",i,stack[i]);
		}
		for(int i = 0;i<26;i++) {
			if(registers[i] != 0) {
				printw("%c:%G,",0x41+i,registers[i]);
			}
		}
		echo(); // so you can see what you're typing
		input = getch();
		noecho();
		printw("\n");
		if(input >= 0x41) { // commands don't have numbers
			if(input <= 0x5A) { // register entry
				stackIndex--;
				registers[input-0x41] = stack[stackIndex];
				stack[stackIndex] = 0;
				continue;
			}
			switch(input) {
				case 'q' :
					stackIndex--;
					stack[stackIndex] = sqrt(stack[stackIndex]);
					stackIndex++;
					continue;
				case 'w' :
					if(stackIndex<2) {
						printw("Not enough items\n");
						continue;
					}
					temp = stack[stackIndex-2];
					stack[stackIndex-2] = stack[stackIndex-1];
					stack[stackIndex-1] = temp;
					temp = 0;
					continue;
				case 'c' :
					stackSize = 1;
					stack = realloc(stack,sizeof(double));
					stackIndex = 0;
					stack[0] = 0;
					stack[1] = 0;
					continue;
			}
			// below here is all arithmetic operations
			if(stackIndex<2) { // small stack
				printw("Not enough items\n");
				continue;
			}
			stackIndex-=2; // all arithmetic operations implemented have 2 operands
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
				case 'r' : // index+1 being root
					stack[stackIndex] = (log(stack[stackIndex]))/(log(stack[stackIndex+1]));
					break;
				default: 
					printw("not a valid command\n");
					stackIndex++; // so top value doesn't get cleared
			}
			stackIndex++; // "popping" top value since there is only 1 result
			stack[stackIndex] = 0; 
		} else {
			errno = 0;
			doubleIn = strtod(&input,NULL);
			if(errno != 0) {
				printw("Couldn't parse number");
				continue;
			}
			stack[stackIndex] = doubleIn;
			stackIndex++;
			if(stackIndex == stackSize) {
				//stack = realloc(stack,(stackIndex+1)*sizeof(double));
				stackSize++;
				stack = realloc(stack,stackSize*sizeof(double));
				stack[stackSize-1] = 0;
			}
		}
		if(stackIndex+1 < stackSize) {
			stackSize--;
			stack = realloc(stack,stackSize*sizeof(double));
			stack[stackSize-1] = 0;
		}
	}
	free(stack);
	endwin();
	return(0);
}
