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
#include "safe_malloc.h"
#include "prints.h"

class CircBuffer{
private:
	volatile uint32_t head;
	volatile uint32_t tail;
	char buffer_for_string[50];
public:
	volatile uint32_t size;
	uint8_t* buffer;
	//MallocSafe <uint8_t> safe_allocated_buffer;
	volatile uint32_t available;
	CircBuffer(uint32_t siz): head(0), tail(0), size(siz), available(0) {
		buffer = (uint8_t*)malloc(size);
	}
	~CircBuffer(){
		free(buffer);;
	}
	void flush(){
		head = 0;
		tail = 0;
		available = 0;
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
	char* gets(char str_end = '\x00'){
		return gets(buffer_for_string, str_end);
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
};




#endif /* CBUFFER_H_ */
