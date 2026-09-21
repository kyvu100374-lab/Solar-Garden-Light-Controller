/* 
 * File:   PWM.h
 * Author: congx
 *
 * Created on August 23, 2026, 10:16 AM
 */

#ifndef PWM_H
#define	PWM_H
#include<xc.h>
#include <stdint.h>
//1. DN
#define _XTAL_FREQ  20000000

//2. Khai báo bi?n

//3. Cac ham
void PWM_INIT(uint8_t Period , uint8_t Prescale);
void SET_DUTY_2(uint16_t duty_cycle);
void SET_DUTY_1(uint16_t duty_cycle);
#endif	/* PWM_H */

