/* 
 * File:   INA219.h
 * Author: Thi
 *
 * Created on August 18, 2026, 2:50 PM
 */

#ifndef INA219_H
#define	INA219_H

#include<xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "i2c.h"
// 1. dinh ngh?a Macro / Constant 
#define _XTAL_FREQ  20000000
#define ADDR        0b1000000
#define Config      0x3557
#define time_n      3

//Reg
#define config_reg  0x00
#define Vshunt_reg  0x01
#define Vbus_reg    0x02
#define Power_reg   0x03
#define Current_reg 0x04
#define Calib_reg   0x05

//2. Khai báo bien

//3. Khai báo Prototype (nguyên m?u hàm)
void INA219_INIT();
float INA219_READ_SHUNT();
float INA219_READ_BUS();
float INA219_READ_POWER();
float INA219_READ_CURRENT();
void INA219_WRTE_CALIB(uint16_t Calib_Value);
#endif	/* INA219_H */

