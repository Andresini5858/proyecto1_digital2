/*
 * File:   main_slave1.c
 * Author: Andres Lemus
 *
 * Created on February 20, 2023, 10:35 AM
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
#include "tcs230.h"

#define _XTAL_FREQ 8000000 //Frecuencia 8MHz
//SLAVE SLAVE SLAVE

uint8_t recibido, enviado;
uint8_t red, green, blue, contr, contg, contb, cont, dc;

void setup(void);

void main(void){
    setup();
    //TCS230_Init();
    contr = 0;
    contg = 0;
    contb = 0;
    cont = 0;
    dc = 1;
    while(1){
        while(dc == 1){
            PORTAbits.RA1 = 1;
            PORTAbits.RA0 = 0;
            
            if (PORTAbits.RA3 == 1){
                PORTCbits.RC1 = 1;
            }
            else {
                PORTCbits.RC1 = 0;
            }
            
            //red = TCS230_Get_Value(CHANNEL_R);
            //green = TCS230_Get_Value(CHANNEL_G);
            //blue = TCS230_Get_Value(CHANNEL_B);

            if((red>28 && red<40) && (green>76 && green<90) && (blue>56 && blue<80))
            {
                contr++;
            }
            else if((red>78 && red<92) && (green>52 && green<72) && (blue>56 && blue<80))
            {
                contg++;
            }
            else if((red>82 && red<115) && (green>62 && green<82) && (blue>32 && blue<52))
            {
                contb++;
            }
            else
            {
                contr = contr;
                contg = contg;
                contb = contg;
            }
        }
        __delay_ms(100);
    }
}

void __interrupt() isr(void){ //Interrupciones
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
            I2C_Write_Slave(cont);
            /*if (recibido == 0){
                I2C_Write_Slave(contr);
            }
            else if (recibido == 1){
                I2C_Write_Slave(contg);
            }
            else if (recibido == 2){
                I2C_Write_Slave(contb);
            }
            else if (recibido == 3){
                I2C_Write_Slave(cont);
            }
            else if (recibido == 4){
                dc = 0;
                INTCONbits.RBIE = 0;
                PORTAbits.RA0 = 0;
                PORTAbits.RA1 = 0;
            }
            else if (recibido == 5){
                dc = 1;
                INTCONbits.RBIE = 1;
            }*/
        }
        PIR1bits.SSPIF = 0; //Limpiar bandera del i2c
    }
    if (INTCONbits.RBIF == 1){
        if (PORTBbits.RB4 == 1){
            cont++;
        }
        INTCONbits.RBIF = 0;
    }
}

void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA3 = 1;
    TRISCbits.TRISC1 = 0;
    
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    
    //INTCONbits.RBIE = 1;
    //INTCONbits.RBIF = 0;
    //IOCBbits.IOCB4 = 1;
    
    OSCCONbits.IRCF2 = 1; //Frecuencia de 8MHz
    OSCCONbits.IRCF1 = 1; 
    OSCCONbits.IRCF0 = 1;
    
    OSCCONbits.SCS = 1; //Utilizar reloj interno
    I2C_Init_Slave(0x50); //Inicar esclavo y definir su direccións
}