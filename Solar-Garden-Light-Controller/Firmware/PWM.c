#include"PWM.h"

void PWM_INIT(uint8_t Period , uint8_t Prescale)
{
    T2CONbits.T2CKPS = Prescale;    // chon bo chia truoc
    TMR2 = 0;
    PR2  = Period; 
    T2CONbits.TMR2ON = 1;           // bat timer2
    CCP2CONbits.CCP2M = 0b1100;     // vhe do pwm
    CCP1CONbits.CCP1M = 0b1100;     // vhe do pwm
}
void SET_DUTY_1(uint16_t duty_cycle)
{
    CCPR1L = (duty_cycle >> 2) & 0xff ;
    CCP1CONbits.CCP1X = (duty_cycle >> 1) & 1;
    CCP1CONbits.CCP1Y = duty_cycle & 1;
}
void SET_DUTY_2(uint16_t duty_cycle)
{
    CCPR2L = (duty_cycle >> 2) & 0xff ;
    CCP2CONbits.CCP2X = (duty_cycle >> 1) & 1;
    CCP2CONbits.CCP2Y = duty_cycle & 1;
}