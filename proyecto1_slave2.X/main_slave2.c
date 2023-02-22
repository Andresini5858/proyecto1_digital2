/*
 * File:   main_slave2.c
 * Author: Andres Lemus
 *
 * Created on February 20, 2023, 10:36 AM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "i2c.h" //Incluir Librería de i2c
#include "float_str.h"
#include "hc_sr04.h"

#define _XTAL_FREQ 8000000 //Frecuencia 8MHz

#define v_min 14
#define v_max 60
#define SERVO_HARDWARE

#define SERVO_1_PIN PORTAbits.RA0
#define SERVO_1_DIR TRISAbits.TRISA0

#define SERVO_2_PIN PORTAbits.RA1
#define SERVO_2_DIR TRISAbits.TRISA1

#ifdef SERVO_1_PIN
void Servo_1_Write(unsigned char ang);
#endif

#ifdef SERVO_2_PIN
void Servo_2_Write(unsigned char ang);
#endif
//SLAVE2 SLAVE2 SLAVE2

uint8_t recibido, enviado, dis;
unsigned int cont_sr = 0;
unsigned char data_s[8];

void setup(void);
void Servo_Init(void);

void main(void){
    setup();
    HCSR04_Init();
    while(1){
        float distancia = HCSR04_Get_Distance();
        dis = (distancia * 100) / 1;
        
        Servo_1_Write(0);
        Servo_2_Write(0);
        __delay_ms(500);
        
        Servo_1_Write(90);
        Servo_2_Write(90);
        __delay_ms(500);
        
        Servo_1_Write(180);
        Servo_2_Write(180);
        __delay_ms(500);
    }
}

void __interrupt() isr(void){ //Interrupciones
    if(INTCONbits.TMR0IF == 1){
        cont_sr = cont_sr + 1;
        if(cont_sr > 490){
            cont_sr = 0;
        }
        #ifdef SERVO_1_PIN
        if(cont_sr < data_s[0]){SERVO_1_PIN = 1;} else{SERVO_1_PIN = 0;}
        #endif

        #ifdef SERVO_2_PIN
        if(cont_sr < data_s[1]){SERVO_2_PIN = 1;} else{SERVO_2_PIN = 0;}
        #endif

        TMR0 = 250;
        INTCONbits.TMR0IF = 0;
    }
    if (PIR1bits.SSPIF == 1){ //Chequear si se generó interrupción del ADC del i2c
        if(I2C_Error_Read() != 0) //Revisar si hubo error en recepción
        {
            I2C_Error_Data();
        }
        if(I2C_Write_Mode() == 1) //Revisar si se quiere  recibir del maestro
        {
            recibido = I2C_Read_Slave();
        }
        if(I2C_Read_Mode() == 1) //Revisar si se quiere enviar al maestro
        {
            I2C_Write_Slave(dis);
        }
        PIR1bits.SSPIF = 0; //Limpiar bandera del i2c
    }
}

void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    TRISA = 0;
    
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    
    OSCCONbits.IRCF2 = 1; //Frecuencia de 8MHz
    OSCCONbits.IRCF1 = 1; 
    OSCCONbits.IRCF0 = 1;
    
    OSCCONbits.SCS = 1; //Utilizar reloj interno
    Servo_Init();
    I2C_Init_Slave(0x40); //Inicar esclavo y definir su direccións
}

void Servo_Init(void){
    #ifdef SERVO_1_PIN
    SERVO_1_DIR = 0;
    SERVO_1_PIN = 0;
    #endif

    #ifdef SERVO_2_PIN
    SERVO_2_DIR = 0;
    SERVO_2_PIN = 0;
    #endif

    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IF = 0;
    
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.T0SE = 1;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS2 = 0;
    OPTION_REGbits.PS1 = 0;
    OPTION_REGbits.PS0 = 0;
    
    TMR0 = 250;
}

#ifdef SERVO_1_PIN
void Servo_1_Write(unsigned char ang)
{
    data_s[0] = (ang-0)*(v_max-v_min)/(180-0)+v_min;
}
#endif

#ifdef SERVO_2_PIN
void Servo_2_Write(unsigned char ang)
{
    data_s[1] = (ang-0)*(v_max-v_min)/(180-0)+v_min;
}
#endif