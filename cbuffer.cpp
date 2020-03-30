/*
 * cbuffer.cpp
 *
 *  Created on: 24 mar 2019
 *      Author: rafal
 */

#include "cbuffer.h"



CircBuffer::CircBuffer(uint32_t siz, uint8_t local_buff_for_string_size):
	head(0), tail(0), local_buff_for_string_size(local_buff_for_string_size), available(0), size(siz){
	buffer = (uint8_t*)malloc(size);
	constr_method = cbuffer_constr_method::with_malloc;
}

CircBuffer::CircBuffer(CircBuffer* cbuffer, uint8_t local_buff_for_string_size): local_buff_for_string_size(local_buff_for_string_size){
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
