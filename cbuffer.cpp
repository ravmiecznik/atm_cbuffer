/*
 * cbuffer.cpp
 *
 *  Created on: 24 mar 2019
 *      Author: rafal
 */

#include "cbuffer.h"

void print_buff_char(char c){
	/*
	 * Prints char if alpha numeric
	 * or Hex if not
	 */
	if(isalnum(c))
		printf("%c", c);
	else if(c == '>' or c == '<')
		printf("%c", c);
	else
		printf("%02X ", c);
}



