#include"i2c.h"

//1. Ham khoi tao master
void i2c_master_init(uint16_t speed)
{
    TRISCbits.TRISC3    =   1;          // chan scl
    TRISCbits.TRISC4    =   1;          // chan sda
    
    SSPSTATbits.SMP     =   1;          // khong kiem soat toc do (100khz - 1Mhz)
    SSPCONbits.SSPM     =   0b1000;     //I2C Master mode, clock = FOSC/(4 * (SSPADD + 1)) 
    SSPADD              =   (uint8_t)((FOSC / (4000.0 * speed)) - 1); // 
     SSPCONbits.SSPEN    =   1;          // kich hoat i2c
}
// ham khoi tao slave
void i2c_slave_init(unsigned char addr)
{
    TRISCbits.TRISC3    =   1;          // chan scl
    TRISCbits.TRISC4    =   1;          // chan sda          
    INTCONbits.PEIE     =   1;          //cho phep ngat ngoai vi
    PIE1bits.SSPIE      =   1;          //bat ngat SSP
    PIR1bits.SSPIF      =   0;          // xoa co ngat
    INTCONbits.GIE      =   1;          // ngat toan cuc 
    SSPCONbits.SSPM     =   0b1110;     //I2C Slave mode, 7-bit address with Start and Stop bit interrupts enabled
    SSPSTATbits.SMP     =   0;          //Slew rate control disabled for standard speed mode (100 kHz and 1 MHz)
    SSPCON2bits.SEN     =   1;          //Tính n?ng kéo dài xung nh?p (clock stretching) ???c kích ho?t cho c? ch? ?? truy?n và nh?n c?a thi?t b? t? (slave).
    SSPADD              =   addr<<1;    //dia chi slave
    SSPCONbits.SSPEN    =   1;          // kich hoat cong
}
//Hamf bat dau
void i2c_start(void)
{
    while ((SSPCON2 & 0x1F) || (SSPSTATbits.R_W)); // CH? BUS R?NH
    SSPCON2bits.SEN     =   1; //Kích ho?t ?i?u ki?n Start trên các chân SDA và SCL. ph?n c?ng t? ??ng xóa.
    i2c_check_timeout();
}
void i2c_stop(void)
{
    while ((SSPCON2 & 0x1F) || (SSPSTATbits.R_W)); // CH? BUS R?NH
    SSPCON2bits.PEN     =   1; //Kích ho?t ?i?u ki?n D?ng (Stop condition) trên các chân SDA và SCL. Tr?ng thái này ???c ph?n c?ng t? ??ng xóa.
    i2c_check_timeout();
}
void i2c_write(unsigned char data) {
    while ((SSPCON2 & 0x1F) || (SSPSTATbits.R_W)); // CH? BUS R?NH
    SSPCONbits.WCOL = 0;                           // XÓA C? XUNG ??T
    SSPBUF              =   data; 
    i2c_check_timeout();
}

unsigned char i2c_read(short AnK) {
    SSPCON2bits.RCEN         =   1;  //Enables Receive mode for I2C
    // ??i Buffer ??y (BF) v?i Timeout
    unsigned int timeout     = I2C_TIMEOUT;
    while(!SSPSTATbits.BF)      // cho SSPBUF dull
    {
        timeout--;
        if(timeout == 0) break; 
    }
    unsigned char data  =   SSPBUF;     //doc du lieu
    SSPCON2bits.ACKDT   =   AnK;        // set bit NACK
    SSPCON2bits.ACKEN   =   1;          //Kh?i t?o trình t? xác nh?n (Acknowledge) trên các chân SDA và SCL, ??ng th?i truy?n bit d? li?u ACKDT.
   
    // ??i g?i xong ACK
    timeout = I2C_TIMEOUT;
    while(SSPCON2bits.ACKEN)            // thoat khi gui xong nACK
    {
        timeout--;
        if(timeout == 0) break;
    }
    i2c_check_timeout(); // Xóa co SSPIF
    return data;
}
void i2c_restart(void)
{
    while ((SSPCON2 & 0x1F) || (SSPSTATbits.R_W)); 
    SSPCON2bits.RSEN    =   1;
    i2c_check_timeout();

}
// chong treo
unsigned char i2c_check_timeout(void) {
    unsigned int timeout = I2C_TIMEOUT;
    while(!PIR1bits.SSPIF) 
    {
        timeout--;
        if(timeout == 0) {
            SSPCON2bits.PEN    = 1; // C? g?ng g?i l?nh Stop ?? gi?i phóng bus
            return 1;
        }
    }
    PIR1bits.SSPIF = 0;
    return 0; 
}
