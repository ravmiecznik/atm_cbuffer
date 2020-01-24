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

char read_pgm_byte_smart(const char* ptr){
	uint32_t addr = (uint32_t)ptr;
	if(addr < 0xFFFF)
		return pgm_read_byte(addr);
	else
		return pgm_read_byte_far(addr);
}

bool CircBuffer::is_in_local(const char* string){
	/*
	 * check if string in local buffer
	 */
	char c;
	for(uint8_t i=0; i<local_buff_for_string_size; i++){
		c = *string++;
		if(c == '\x00'){
			break;
		}
		if(buffer_for_string[i] != c)
			return false;
	}
	return true;
}


bool CircBuffer::is_in_local_p(const char* string){

		/*
		 * check if string in local buffer
		 */
		char b;
		for(uint8_t i=0; i<local_buff_for_string_size; i++){
			b=read_pgm_byte_smart(&(string[i]));
			if(b == '\x00'){
				break;
			}
			if(buffer_for_string[i] != b)
				return false;
		}
		return true;
}




