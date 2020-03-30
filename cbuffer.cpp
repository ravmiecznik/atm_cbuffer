/*
 * cbuffer.cpp
 *
 *  Created on: 24 mar 2019
 *      Author: rafal
 */

#include "cbuffer.h"



CircBuffer::CircBuffer(uint32_t siz):
	head(0), tail(0), available(0), size(siz){
	buffer = (uint8_t*)malloc(size);
	constr_method = cbuffer_constr_method::with_malloc;
}

CircBuffer::CircBuffer(CircBuffer* cbuffer){
	head = cbuffer->head;
	size = cbuffer->size;
	available = cbuffer->available;
	buffer = cbuffer->buffer;
	constr_method = cbuffer_constr_method::by_pointer;
}

CircBuffer::~CircBuffer(){
	switch (constr_method) {
		case cbuffer_constr_method::with_malloc:
			break;
		default:
			break;
	}
}


uint32_t CircBuffer::free_space(){
	return size - available;
}

void CircBuffer::flush(){
	head = 0;
	tail = 0;
	available = 0;
}

void CircBuffer::flush(uint32_t amount){
	while(amount--)
		get();
}

CircBuffer CircBuffer::peek(){
	/*
	 * Return CircBuffer created with pointer constructor
	 * It makes its own head, tail, available but points to common buffer
	 * In practice this is just freeze state of real CircBuffer can be used to peek
	 */
	return CircBuffer(this);
}

void CircBuffer::peek_sync(CircBuffer* peek_cbuffer){
	peek_cbuffer->available = available;
	peek_cbuffer->head = head;
	peek_cbuffer->tail = tail;
}

void CircBuffer::put(uint8_t chr){
	buffer[tail] = chr;
	tail = (tail + 1) % size;
	if(available < size)
		available++;
	if(available == size)
		head = tail;
}

uint8_t CircBuffer::get(){
	if(available){
		char c;
		c = buffer[head];
		available--;
		head = (head + 1) % size;
		return c;
	}
	else
		return 0;
}

void CircBuffer::get(uint32_t amount, uint8_t* ext_buffer){
	while(amount and available){
		*(ext_buffer++) = get();
		amount--;
	}
}
void CircBuffer::get_reversed(uint32_t amount, uint8_t* ext_buffer){
	while(amount and available){
		ext_buffer[(amount--)-1] = get();
	}
}
void CircBuffer::get(uint32_t amount, char* ext_buffer){
	get(amount, (uint8_t*)ext_buffer);
}


//TODO: size of ext_buffer must be known!!
char* CircBuffer::get_all(char* ext_buffer){
	uint32_t cnt=0;
	char c;
	while(available){
		c = get();
		*(ext_buffer+ cnt++)=c;
	}
	ext_buffer[cnt] = get();
	flush();
	return ext_buffer;
}

bool CircBuffer::is_string_in_buff(){
	if(available){
		if(buffer[tail] == '\n' or buffer[tail] == '\n'){
			return true;
		}
		else{
			return false;
		}
	}
	return false;
}



void CircBuffer::put(uint32_t amount, uint8_t* ext_buffer){
	uint32_t i=0;
	while(amount){
		put(ext_buffer[i]);
		amount--;
		i++;
	}
}

template<typename data_type>
void CircBuffer::puts(data_type* ext_buffer){
	uint32_t i=0;
	while(*(ext_buffer + i) and i<size){
		put(*(ext_buffer + i++));
	}
	put(*(ext_buffer + i));
}


bool CircBuffer::is_endl_in_buffer(){
		volatile uint8_t* tmp_ptr=buffer+head;
		volatile uint32_t _head = head;
		char c;
		for(uint32_t i=0; i<available; i++){
			c = *(tmp_ptr+_head);
			_head = (_head + 1) % size;
			if( c == '\r' or c == '\n')
				return true;
		}
		return false;
}

bool CircBuffer::is_in_buffer(const char* string){
	uint32_t _head = head;
	if(available){
		uint32_t i=0;
		char c;
		while(c = pgm_read_byte(string+i)){
			if(buffer[_head] != c)
				return false;
			i++;
			_head = (_head + 1) % size;
		}
		return true;
	}
	return false;
}


void CircBuffer::CircBuffer::set_head(uint32_t new_head){
	head = new_head;
}

volatile uint8_t* CircBuffer::get_buff_ptr(){
	return buffer;
}
