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
#include <stdio.h>
#include <ctype.h>


namespace cbuffer_constr_method{
	enum constr_method{
		with_malloc,
		by_pointer,
	};
}


class CircBuffer{
private:
	volatile uint32_t head;
	volatile uint32_t tail;
	cbuffer_constr_method::constr_method constr_method;
public:
	uint32_t available();
	volatile uint8_t* buffer;
	volatile uint32_t size;
	CircBuffer(uint32_t siz);
	CircBuffer(CircBuffer* cbuffer);
	~CircBuffer();
	bool put(uint8_t chr);
	void pop_last(uint8_t num);
	uint32_t free_space();
	void flush();
	void flush(uint32_t amount);
	CircBuffer peek();
	void peek_sync(CircBuffer* peek_cbuffer);
	uint8_t get();

};


#endif /* CBUFFER_H_ */
