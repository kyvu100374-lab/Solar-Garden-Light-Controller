/*
 * File:     MPPT-control.c
 * Author: Thi
 * Created on August 17, 2026, 8:46 PM
 */

// CONFIG
#pragma config FOSC     = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE     = ON        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE    = ON        // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN    = ON        // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP      = OFF       // Low-Voltage Programming Enable bit
#pragma config CPD      = ON        // Data EEPROM Memory Code Protection bit
#pragma config WRT      = OFF       // Flash Program Memory Write Enable bits
#pragma config CP       = ON        // Flash Program Memory Code Protection bit

#define _XTAL_FREQ  20000000

// lenh
#define val_calib       2276
#define STEP            2
#define EPSILON_V       0.02f       // Nguong coi dV = 0 (Volt)
#define EPSILON_I       0.005f      // Nguong coi dI = 0 (A)
#define EPSILON_INC     0.01f       // Nguong coi dI/dV + I/V = 0

// MPPT <=> light
#define STATE           PORTDbits.RD2

// UART
#define NUM_RX          3

// thu vien
#include <xc.h>
#include "i2c.h"
#include "INA219.h"
#include "USART.h"
#include "PWM.h"

// cau truc du lieu goi 
typedef union {
    __pack struct {
        uint16_t value_DAC;
        uint8_t  light_power;
    };
    uint8_t buffer[3];
} RX_data;

typedef union {
    struct {
        float Current;
        float Volt;   
        float Power;   
    };
    uint8_t buffer[12]; 
} Payload_t;

volatile uint8_t rx_index = 0;
volatile uint8_t rx_buffer[5];

// bien MPPT
float vol, current, power;
float last_vol = 0.0f, last_current = 0.0f;
float D_vol, D_current;
int16_t vol_control = 500;

// ADC
uint16_t val;
float Vout  ;

// khai bao goi
Payload_t   data;
RX_data     rx_data;

// Bien luu thoi gian
volatile uint8_t count_50ms = 0;
volatile uint8_t seconds = 0;
volatile uint16_t minutes = 0;

// CO BAP GHI EEPROM 
volatile uint8_t save_eeprom_flag = 0; 

// bien do sang
uint16_t P_light;
uint8_t last_state = 0xFF, dip = 1; 

// co bao ngat sac
bool flag_battery = false, last_flag_battery = false;

// Prototypes
void ADC_Init(void);
uint16_t read_adc(void);
void Timer1_Init(void);
void MPPT(void);
void LGHT(uint16_t time);
void WDT(void);
void EEPROM_Write(uint8_t address, uint8_t data);
uint8_t EEPROM_Read(uint8_t address);


// ==================== HAM NGAT ==================== //
void __interrupt() my_isr0()
{
    // 1. Ngat Timer1
    if (PIR1bits.TMR1IF) {
        TMR1H = 0x85;
        TMR1L = 0xEE;
        
        count_50ms++;
        if (count_50ms >= 20) { // Du 1 giay
            count_50ms = 0;
            seconds++;
            
            if (seconds >= 60) { // Du 1 phut
                seconds = 0;
                minutes++; // Tang so phut
                
                // Cu 10 phut BAT CO de main() ghi EEPROM
                if (minutes % 10 == 0) {
                    save_eeprom_flag = 1; 
                }
            }
        }
        PIR1bits.TMR1IF = 0; 
    }

    // 2. Ngat UART RX
    if(RCIF){
        if(RCSTAbits.OERR) { 
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;
            uint8_t dummy = RCREG; 
            return;
        }
        uint8_t tx_byte = RCREG; 
        
        // Them kiem tra rx_index de tranh ghi ghi vuot marray
        if (rx_index < 5) {
            rx_buffer[rx_index++] = tx_byte;
        } else {
            rx_index = 0; // Tránh tràn b? nh? ??m n?u d?n byte rác
        }
        
        if(rx_index >= NUM_RX + 2) {
            if(rx_buffer[0] == 0xCC && rx_buffer[1] == 0xDD) {
                for(uint8_t i = 0; i < NUM_RX; i++) {
                    rx_data.buffer[i] = rx_buffer[i + 2];
                }
                rx_index = 0;      
            } 
            else {
                for(uint8_t i = 0; i < rx_index - 1; i++) {
                    rx_buffer[i] = rx_buffer[i + 1];
                }
                rx_index--;
            } 
        }
    }
}

// ==================== HAM MAIN ==================== //
int main(void) {
    WDT();
    
    // Set Pin
    TRISDbits.TRISD2 = 1;       // STATE
    TRISCbits.TRISC2 = 0;       // PWM1
    TRISCbits.TRISC1 = 0;       // PWM2
    
    // Khoi tao ngoai vi
    i2c_master_init(100);       
    INA219_INIT();
    __delay_ms(50);             
    INA219_WRTE_CALIB(val_calib);
    USART_INIT(9600);
    PWM_INIT(255, 0b01);        
    Timer1_Init();
    ADC_Init();
    __delay_ms(5);
    // do sang dua tren muc dien aps tich tru duoc
    val = read_adc();
    Vout = val*5.0/1023;
    
    if(Vout < 4.05)
    {
        dip = 2;
    }
    else 
    {
        dip = 1;
    }
    
    // Doc thoi gian luu trong EEPROM
    uint8_t min_high = EEPROM_Read(0x00);
    uint8_t min_low  = EEPROM_Read(0x01);
    uint16_t saved_min = ((uint16_t)min_high << 8) | min_low;
    
    // Kiem tra luc vua cap nguon (Power-On)
    if (STATE == 0) {
        if (saved_min < 1440) {
            minutes = saved_min; // Khoi phuc phut cu neu bi WDT Reset giua dem
        } else {
            minutes = 0;
            EEPROM_Write(0x00, 0);
            EEPROM_Write(0x01, 0);
        }
    } else {
        minutes = 0;
        EEPROM_Write(0x00, 0);
        EEPROM_Write(0x01, 0);
    }
    
    INTCONbits.GIE = 1;         // Ngat toan cuc 
    
    last_state = 0xFF; 
    SET_DUTY_1((uint16_t)vol_control);

    while (1) 
    {
        CLRWDT();               // Clear Watchdog Timer

        if (save_eeprom_flag) {
            save_eeprom_flag = 0; // Xoa co
            EEPROM_Write(0x00, (uint8_t)(minutes >> 8));   // Byte cao
            EEPROM_Write(0x01, (uint8_t)(minutes & 0xFF)); // Byte thap
        }

        // 1. CHE DO BAN NGAY (MPPT)
        if(STATE)
        {
            if (last_state != 1) 
            {
                PIE1bits.TMR1IE = 0; // Tat ngat Timer1
                
                minutes = 0;
                seconds = 0;
                count_50ms = 0;
                
                // Ch? ghi EEPROM reset n?u giá tr? tr??c ?ó khác 0 ?? b?o l?u tu?i th? EEPROM
                if (EEPROM_Read(0x00) != 0 || EEPROM_Read(0x01) != 0) {
                    EEPROM_Write(0x00, 0);
                    EEPROM_Write(0x01, 0);
                }
            }

            SET_DUTY_2(0); 
            MPPT();
            last_state = 1; 
        }
        // 2. CHE DO BAN DEM (CHIEU SANG)
        else
        {
            if (last_state != 0) 
            {
                PIE1bits.TMR1IE = 1; // BAT NGAT TIMER1 DEM GIO
                
                seconds = 0;
                count_50ms = 0;
            }

            SET_DUTY_1(1023); 
            LGHT(minutes);
            last_state = 0; 
        }
        
        // ==================== TRUYEN UART ==================== //
        data.Current = current;
        data.Volt    = vol;
        data.Power   = power;
        
        USART_WRITE(0xAA);      
        USART_WRITE(0xBB);      
        
        for(int i = 0; i < 12; i++) {
            USART_WRITE(data.buffer[i]);
        }
        
        __delay_ms(50);
    }
}

// ==================== CAC HAM CON ==================== //

void Timer1_Init(void) {
    T1CONbits.T1CKPS1 = 1;
    T1CONbits.T1CKPS0 = 1;
    T1CONbits.TMR1CS = 0;
    
    TMR1H = 0x85;
    TMR1L = 0xEE;
    
    PIR1bits.TMR1IF = 0;
    PIE1bits.TMR1IE = 0; 
    INTCONbits.PEIE = 1;
    T1CONbits.TMR1ON = 1; 
}

void MPPT(void)
{
    current = INA219_READ_CURRENT();
    vol     = INA219_READ_BUS();
    power   = vol * current;
    
    if (vol < 0.1f) {
        vol = 0.1f; 
    }
        
    D_vol     = vol - last_vol;
    D_current = current - last_current;
    
    if((D_vol > -EPSILON_V) && (D_vol < EPSILON_V))
    {
        if(D_current > EPSILON_I) {
            vol_control += STEP;
        }
        else if(D_current < -EPSILON_I) {
            vol_control -= STEP;
        }
    }
    else 
    {
        float inc_cond = ((D_current * vol) + (D_vol * current)) / D_vol;
        if(inc_cond > EPSILON_INC) {
            vol_control += STEP;    // Tang áp
        }
        else if(inc_cond < -EPSILON_INC) {
            vol_control -= STEP;    // Gi?m áp
        }
    }
        
    last_vol = vol;
    last_current = current;
    
    if(vol_control > 1023) {
        vol_control = 1023;
    }
    else if(vol_control < 0) {
        vol_control = 0;
    }
    // ngat sac
    val = read_adc();
    Vout = val*5.0/1023;
    if(Vout > 4.18)
    {
        float I_out = current*0.95f*(vol*1.0f/(Vout*3.0f));
        if(((I_out) < 0.22f)||(Vout>4.23))
        {
            flag_battery = true;
        }
       
    }
    else if (Vout < 4.14)
    {
        flag_battery = false;
    }
    
    if(flag_battery)
    {
        vol_control = 1023;
    }
    else if((flag_battery != last_flag_battery)&&!flag_battery)
    {
        vol_control = 500 ;
    }
    last_flag_battery = flag_battery;
    SET_DUTY_1((uint16_t)vol_control);
}

void LGHT(uint16_t time)
{
    
    if(time < 240) {
        P_light = (uint16_t)(800/(1.0*dip));
    }
    else if(time < 480) {
        P_light = (uint16_t)(400.0/(1.0*dip));
    }
    else if(time < 720) {
        P_light = (uint16_t)(200.0/(1.0*dip));
    }
    else {
        P_light = 0;
    }
    SET_DUTY_2(P_light);
}

void WDT(void)
{
    OPTION_REGbits.PSA = 1;   
    OPTION_REGbits.PS2 = 1;   
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;
}

void EEPROM_Write(uint8_t address, uint8_t data) {
    uint8_t gie_state = INTCONbits.GIE;
    INTCONbits.GIE = 0; 
    
    EEADR = address;
    EEDATA = data;
    EECON1bits.EEPGD = 0;
    EECON1bits.WREN = 1;
    
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    
    while(EECON1bits.WR); 
    
    EECON1bits.WREN = 0;
    INTCONbits.GIE = gie_state;
}

uint8_t EEPROM_Read(uint8_t address) {
    EEADR = address;
    EECON1bits.EEPGD = 0;
    EECON1bits.RD = 1;
    return EEDATA;
}

void ADC_Init(void)
{
    TRISAbits.TRISA0 = 1;       // Input
    ADCON0bits.ADCS  = 0b10;    // FOSC/32
    ADCON0bits.CHS   = 0b000;   // chon chan RA0 laf chan adc
    ADCON0bits.ADON  = 1;       //bat adc
    
    ADCON1bits.PCFG  = 0b1110;  // vref+ VDD
    ADCON1bits.ADFM  = 1;       // can phai
}

uint16_t read_adc(void)
{
    uint16_t value = 0;
    uint16_t timeout = 1000;
    __delay_us(25);
    ADCON0bits.GO_nDONE = 1;    // bat dau chuyen doi
    while(ADCON0bits.GO_nDONE && (--timeout > 0));
    if(timeout > 0) {
        value = (((uint16_t)ADRESH<<8)|ADRESL);
    }
    return value;
}
