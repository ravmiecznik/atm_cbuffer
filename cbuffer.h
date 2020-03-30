/*
 * cbuffer.h
 *
 *  Created on: 28 lip 2017
 *      Author: miecznik
 */

#ifndef CBUFFER_H_
#define CBUFFER_H_

#include <stdint.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/delay.h>
#include <stdio.h>
#include <ctype.h>


namespace cbuffer_constr_method{
	enum constr_method{
		with_malloc,
		by_pointer,
	};
}

void print_buff_char(char c);

//TODO: move all methods code to *.cpp file, possibly it will save some ram

class CircBuffer{
private:
	volatile uint32_t head;
	volatile uint32_t tail;
	cbuffer_constr_method::constr_method constr_method;
public:
	volatile uint32_t available;
	volatile uint8_t* buffer;
	volatile uint32_t size;
	CircBuffer(uint32_t siz);
	CircBuffer(CircBuffer* cbuffer);
	~CircBuffer();
	uint32_t free_space();
	void flush();
	void flush(uint32_t amount);
	CircBuffer peek();
	void peek_sync(CircBuffer* peek_cbuffer);
	void put(uint8_t chr);
	uint8_t get();
	void get(uint32_t amount, uint8_t* ext_buffer);
	void get_reversed(uint32_t amount, uint8_t* ext_buffer);
	void get(uint32_t amount, char* ext_buffer);
	char* get_all(char* ext_buffer);
	bool is_string_in_buff();
	void put(uint32_t amount, uint8_t* ext_buffer);
	template<typename data_type>
	void puts(data_type* ext_buffer);
	bool is_endl_in_buffer();
	bool is_in_buffer(const char* string);
	void set_head(uint32_t new_head);
	volatile uint8_t* get_buff_ptr();
};


#endif /* CBUFFER_H_ */
