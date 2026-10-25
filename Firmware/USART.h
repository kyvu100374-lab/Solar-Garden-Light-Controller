/* 
 * File:   USART.h
 * Author: congx
 *
 * Created on August 19, 2026, 9:04 AM
 */

#ifndef USART_H
#define	USART_H

//
#include<xc.h>
#include <stdint.h>
//1. DN
#define _XTAL_FREQ  20000000
#define FOSC        _XTAL_FREQ
//2. Khai báo bi?n

//3. Cac ham
void USART_INIT(uint16_t baud);
void USART_SLAVE(void);
void USART_WRITE(uint8_t data);
uint8_t USART_READ(void);
#endif	/* USART_H */

