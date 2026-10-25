#include "INA219.h"
// hamf khoir taoj
void INA219_INIT()
{
    //config
    i2c_start();
    i2c_write(ADDR<<1);
    i2c_write(config_reg);
    i2c_write((uint8_t)(Config>>8));    //12bit 4 maux
    i2c_write(Config&0xff);
    i2c_stop();
    
   
    
}
// do vol shunt
float INA219_READ_SHUNT()
{
    int16_t reval;
    uint8_t highByte, lowByte;
    i2c_start();
    i2c_write(ADDR<<1);
    i2c_write(Vshunt_reg);
    i2c_restart();
    
    i2c_write((ADDR<<1)|1);
    highByte = i2c_read(0); 
    lowByte  = i2c_read(1); 
    i2c_stop();
    reval = (int16_t)((highByte << 8) | lowByte); 
    return reval*0.01;
    
}
// do vol bus
float INA219_READ_BUS()
{
    int16_t reval;
    uint8_t highByte, lowByte;
    i2c_start();
    i2c_write(ADDR<<1);
    i2c_write(Vbus_reg);
    i2c_restart();
    
    i2c_write((ADDR<<1)|1);
    highByte = i2c_read(0); 
    lowByte  = i2c_read(1);
    i2c_stop();
     reval = (uint16_t)((highByte << 8) | lowByte)>>3; 
    
    return ((float)reval * 4.0) / 1000.0;
}
//do cong suat
float INA219_READ_POWER()
{
    int16_t reval;
    uint8_t highByte, lowByte;
    i2c_start();
    i2c_write(ADDR<<1);
    i2c_write(Power_reg);
    i2c_restart();
    
    i2c_write((ADDR<<1)|1);
    highByte = i2c_read(0); 
    lowByte  = i2c_read(1); 
    i2c_stop();
     reval = (uint16_t)((highByte << 8) | lowByte); 
    
    return ((float)reval*2.0);
}
float INA219_READ_CURRENT()
{
    int16_t reval;
    uint8_t highByte, lowByte;
    i2c_start();
    i2c_write(ADDR<<1);
    i2c_write(Current_reg);
    i2c_restart();
    
    i2c_write((ADDR<<1)|1);
    highByte = i2c_read(0); 
    lowByte  = i2c_read(1); 
    i2c_stop();
     reval = (uint16_t)((highByte << 8) | lowByte); 
    
    return ((float)reval)*0.001;
}
// calib
void INA219_WRTE_CALIB(uint16_t Calib_Value)
{
     i2c_start();
    i2c_write(ADDR << 1);
    i2c_write(Calib_reg);
    i2c_write((uint8_t)(Calib_Value >> 8)); 
    i2c_write(Calib_Value & 0xFF);          
    i2c_stop();
}