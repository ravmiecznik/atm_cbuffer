/*
 * cbuffer.cpp
 *
 *  Created on: 24 mar 2019
 *      Author: rafal
 */

#include "cbuffer.h"



CircBuffer::CircBuffer(uint32_t siz):
	head(0), tail(0), size(siz){
	buffer = (uint8_t*)malloc(size);
	constr_method = cbuffer_constr_method::with_malloc;
}

uint32_t CircBuffer::available(){
	return tail>head ? tail - head : head- tail;
}

CircBuffer::CircBuffer(CircBuffer* cbuffer){
	head = cbuffer->head;
	size = cbuffer->size;
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
	return size - available();
}

void CircBuffer::flush(){
	head = 0;
	tail = 0;
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
	peek_cbuffer->head = head;
	peek_cbuffer->tail = tail;
}

bool CircBuffer::put(uint8_t chr){

	if(free_space()){
		buffer[tail] = chr;
		tail = (tail + 1) % size;
		return true;
	}
	else{
		return false;
	}
}

uint8_t CircBuffer::get(){
	if(available()){
		char c;
		c = buffer[head];
		head = (head + 1) % size;
		return c;
	}
	else
		return 0;
}

