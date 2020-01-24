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
//#include "../safe_malloc.h"
//#include "../prints.h"


namespace cbuffer_constr_method{
	enum constr_method{
		with_malloc,
		by_pointer,
	};
}

void print_buff_char(char c);

class CircBuffer{
private:
	volatile uint32_t head;
	volatile uint32_t tail;
	cbuffer_constr_method::constr_method constr_method;
	uint8_t local_buff_for_string_size;
public:
	char* buffer_for_string;
	volatile uint32_t available;
	uint8_t* buffer;
	volatile uint32_t size;
	CircBuffer(uint32_t siz, uint8_t local_buff_for_string_size=20):
		head(0), tail(0), local_buff_for_string_size(local_buff_for_string_size), available(0), size(siz){
		buffer_for_string = (char*)malloc(local_buff_for_string_size);
		buffer = (uint8_t*)malloc(size);
		constr_method = cbuffer_constr_method::with_malloc;
	}
	CircBuffer(CircBuffer* cbuffer, uint8_t local_buff_for_string_size=20): local_buff_for_string_size(local_buff_for_string_size){
		head = cbuffer->head;
		size = cbuffer->size;
		available = cbuffer->available;
		buffer_for_string = cbuffer->buffer_for_string;
		buffer = cbuffer->buffer;
		constr_method = cbuffer_constr_method::by_pointer;
	}
	~CircBuffer(){
		switch (constr_method) {
			case cbuffer_constr_method::with_malloc:
				free(buffer);
				break;
			default:
				break;
		}

	}

	uint32_t free_space(){
		return size - available;
	}

	void flush(){
		head = 0;
		tail = 0;
		available = 0;
	}

	void flush(uint32_t amount){
		while(amount--)
			get();
	}

	CircBuffer peek(){
		/*
		 * Return CircBuffer created with pointer constructor
		 * It makes its own head, tail, available but points to common buffer
		 * In practice this is just freeze state of real CircBuffer can be used to peek
		 */
		return CircBuffer(this);
	}

	void peek_sync(CircBuffer* peek_cbuffer){
		peek_cbuffer->available = available;
		peek_cbuffer->head = head;
		peek_cbuffer->tail = tail;
	}

	void put(uint8_t chr){
		buffer[tail] = chr;
		tail = (tail + 1) % size;
		if(available < size)
			available++;
		if(available == size)
			head = tail;
	}

	uint8_t get(){
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

	void get(uint32_t amount, uint8_t* ext_buffer){
		//uint32_t i=0;
		while(amount and available){
			*(ext_buffer++) = get();
			amount--;
			//i++;
		}
	}
	void get_reversed(uint32_t amount, uint8_t* ext_buffer){
		while(amount and available){
			ext_buffer[(amount--)-1] = get();
		}
	}
	void get(uint32_t amount, char* ext_buffer){
		get(amount, (uint8_t*)ext_buffer);
	}


	//TODO: size of ext_buffer must be known!!
	char* get_all(char* ext_buffer){
		uint32_t cnt=0;
		char c;
		//while(c = get()){
		while(available){
			c = get();
			*(ext_buffer+ cnt++)=c;
		}
		ext_buffer[cnt] = get();
		flush();
		return ext_buffer;
	}

	bool is_string_in_buff(){
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

	char* gets(){
		/*
		 * max string len=50
		 */
		uint8_t cnt = 0;
		if(available){
			_delay_ms(10);
			//char c;
			buffer_for_string[cnt] = get();
			while(buffer_for_string[cnt] != '\r' and buffer_for_string[cnt] != '\n'){
				if(cnt >= local_buff_for_string_size){
					break;
				}
				cnt++;
				buffer_for_string[cnt] = get();
			}
		}
		else{
			buffer_for_string[0] = '\x00';
		}
		buffer_for_string[local_buff_for_string_size-1] = '\x00';
		return buffer_for_string;
	}

	char* gets(char * ext_buff, char str_end = '\x00'){
		/*
		 * max string len=50
		 */
		uint8_t cnt = 0;
		if(available){
			_delay_ms(10);
			//char c;
			ext_buff[cnt] = get();
			//while(ext_buff[cnt] and ext_buff[cnt] != additional_str_end){
			while(ext_buff[cnt] != str_end){
				if(cnt >= 50){
					break;
				}
				cnt++;
				ext_buff[cnt] = get();
			}
		}
		else{
			ext_buff[0] = '\x00';
		}
		return ext_buff;
	}

	char* gets(char str_end){
		gets(buffer_for_string, str_end);
		//for(int i=local_buff_for_string_size-3; i<local_buff_for_string_size; i++)
		//	buffer_for_string[i] = '.';
		buffer_for_string[local_buff_for_string_size-1] = '\0';
		return buffer_for_string;
	}

	char* copy_to_local(){
	/*
	 * Gets cbuffer content to buffer_for_string
	 */
		for(uint8_t i=0; i<local_buff_for_string_size; i++){
			buffer_for_string[i] = get();
		}
		buffer_for_string[local_buff_for_string_size-1] = '\x00';
		return buffer_for_string;
	}

	void put(uint32_t amount, uint8_t* ext_buffer){
		uint32_t i=0;
		while(amount){
			put(ext_buffer[i]);
			amount--;
			i++;
		}
	}

	template<typename data_type>
	void puts(data_type* ext_buffer){
		uint32_t i=0;
		while(*(ext_buffer + i) and i<size){
			put(*(ext_buffer + i++));
		}
		put(*(ext_buffer + i));
	}


	bool is_endl_in_buffer(){
			uint8_t* tmp_ptr=buffer+head;
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

	bool is_in_buffer(const char* string){
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

	bool is_in_local(const char* string);

	bool is_in_local_p(const char* string);


	int32_t is_in_buffer(char c){
		uint8_t* tmp_ptr=buffer+head;
		//uint32_t position = 0;
		char _c;
		for(uint32_t i=0; i<available; i++){
			_c = *(tmp_ptr+i); //<!!!!! overflow of pointer
			if(_c == c){
				return i;
			}
		}
		return -1;
	}

	void set_head(uint32_t new_head){
		head = new_head;
	}

	bool is_in_buffer(char* string){
		uint32_t _head = head;
		if(available){
			uint32_t i=0;
			while(string[i]){
				if(buffer[_head] != string[i])
					return false;
				i++;
				_head = (_head + 1) % size;
			}
			return true;
		}
		return false;
	}

	uint8_t* get_buff_ptr(){
		return buffer;
	}
};


#endif /* CBUFFER_H_ */
