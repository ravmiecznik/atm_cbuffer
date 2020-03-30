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
	uint8_t local_buff_for_string_size;
public:
	volatile uint32_t available;
	volatile uint8_t* buffer;
	volatile uint32_t size;
	CircBuffer(uint32_t siz, uint8_t local_buff_for_string_size=20);
	CircBuffer(CircBuffer* cbuffer, uint8_t local_buff_for_string_size=20);
	~CircBuffer();

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


	void set_head(uint32_t new_head){
		head = new_head;
	}



	volatile uint8_t* get_buff_ptr(){
		return buffer;
	}
};


#endif /* CBUFFER_H_ */
