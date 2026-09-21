#include "USART.h"

//ham khoi tao
void USART_INIT(uint16_t baud)
{
    TRISCbits.TRISC6    = 0;                    //PIN TX
    TRISCbits.TRISC7    = 1;                    //PIN RX
    TXSTAbits.TX9       = 0;                    //chuoi gom cac dl 8 bit
    TXSTAbits.SYNC      = 0;                    // che do bat dong bo
    TXSTAbits.BRGH      = 1;                    // high speed
    TXSTAbits.TXEN      = 1;                    // cho phep truyen
    
    SPBRG = (uint32_t)(FOSC/(baud*16.0)) - 1;
   
    RCSTAbits.RX9       = 0;                    //8 bit
    RCSTAbits.CREN      = 1;                    // cho phep nhan lien tuc
    PIE1bits.RCIE       = 1;                    //bat ngat nhan
    RCSTAbits.SPEN      = 1;                    // bat cong com
    
}

   

// funcion tran
void USART_WRITE(uint8_t data)
{
    TXREG = data;
    while(!TXSTAbits.TRMT);         // cho gui hoan tat
}
//funcion read
uint8_t USART_READ(void)
{
    while(!PIR1bits.RCIF);          // Ch? c? RCIF = 1 (có d? li?u m?i g?i t?i)
    return RCREG;
}
