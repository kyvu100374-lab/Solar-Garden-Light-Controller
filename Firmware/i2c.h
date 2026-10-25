/* 
 * File:   i2c.h
 * Author: congx
 *
 * Created on August 17, 2026, 8:56 PM
 */

#ifndef I2C_H
#define	I2C_H
#include<xc.h>
#include <stdint.h>
// 1. ??nh ngh?a Macro / Constant
#define I2C_TIMEOUT 1000    
#define _XTAL_FREQ  20000000
#define FOSC        _XTAL_FREQ
// 2. Khai báo Prototype (nguyên m?u hàm)
unsigned char i2c_check_timeout(void);
void i2c_slave_init(unsigned char addr);
void i2c_master_init(uint16_t speed);
void i2c_write(unsigned char  data);
unsigned char i2c_read(short AnK);
void i2c_start(void);
void i2c_stop(void);
void i2c_restart(void);
#endif	/* I2C_H */

