/*
 * File:   main_master.c
 * Author: Andres Lemus
 *
 * Created on February 20, 2023, 10:34 AM
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
#include "LCD.h" //librería LCD
#include "i2c.h" //Librería I2C
#include "float_str.h"

#define _XTAL_FREQ 8000000 //Frecuencia 8MHz

uint32_t temporal;
uint8_t estado;
uint32_t tmp1, tmp2, hum1, hum2, th, tmp3;
uint8_t dec2, uni2, deci2, cent2;
uint8_t distancia;
uint8_t contr, contg, contb, cont;
uint32_t t, h;
float humedad = 0.0;
uint8_t segundos, minutos, horas, dia, mes, ano;
char buffer[30];
char buffer2[48];

void setup(void);
void Set_sec(uint8_t sec); //Función para setear segundos
void Set_min(uint8_t min); //Función para setear minutos
void Set_hour(uint8_t hour); //Función para setear horas
void Set_day_week(uint8_t day_week); //Función para setear día de semana
void Set_day(uint8_t day); //Función para setear día
void Set_month(uint8_t month); //Función para setear mes
void Set_year(uint8_t year); //Función para setear año
uint8_t Read(uint8_t address); //Función para setear minutos
void Read_Time(uint8_t *s, uint8_t *m, uint8_t *h); //Función para obtener valores de tiempo
void Read_Fecha(uint8_t *d, uint8_t *mo, uint8_t *y); //Función para obtener valores de fecha
uint8_t Dec_to_Bcd(uint8_t dec_number); //Función para pasar de decimal a bcd
uint8_t Bcd_to_Dec(uint8_t bcd); //Función para pasar de bcd a decimal
void AHT10_Init(void); //Inicializar el sensor AHT10
void AHT10_Soft_Reset(void);
void AHT10_Read(void);
void Slave1_Total(void);
void Slave1_Red(void);
void Slave1_Blue(void);
void Slave1_Green(void);
void Slave_Overheat(void);
void Slave2(void);
void floatToString(float value, char* buffer, int decimalPlaces);
void uint32_to_string(uint32_t value, char* buffer, uint8_t buffer_size);

void main(void) {
    setup();
    Lcd_Init();
    Lcd_Clear();
    Lcd_Set_Cursor(1,10);
    Lcd_Write_String("T:");
    AHT10_Soft_Reset();
    AHT10_Init();
    Set_sec(0);
    Set_min(0);
    Set_hour(0);
    Set_day(20);
    Set_month(2);
    Set_year(23);
    while(1){
        Read_Time(&segundos, &minutos, &horas);
        Read_Fecha(&dia, &mes, &ano);
        AHT10_Read();
        Slave1_Total();
        
 
        //dec2 = distancia/1000;
        //uni2 = (distancia / 100) % 10;
        //deci2 = (distancia / 10) % 10;
        //cent2 = (distancia % 10);
        
        //Lcd_Set_Cursor(2, 12);
        //sprintf(buffer, "%d%d.%d%d ", dec2, uni2, deci2, cent2);
        //Lcd_Write_String(buffer);
        
        Lcd_Set_Cursor(1,1);
        sprintf(buffer, "%02u:%02u:%02u", horas, minutos, segundos);
        Lcd_Write_String(buffer);
        
        Lcd_Set_Cursor(2,1); //Cursor en 2,7
        sprintf(buffer, "%02u/%02u/20%02u", dia, mes, ano); //Función para pasar variables a cadena de caracteres
        Lcd_Write_String(buffer); //Mostrar en la LCD
        
        Lcd_Set_Cursor(2,12);
        sprintf(buffer, "%02u", cont);
        Lcd_Write_String(buffer);
    }
}

void __interrupt() isr(void){
    if (PIR1bits.TMR1IF == 1){
        unsigned char contador = 0;
        contador++;
        if (contador == 2){
            contador = 0;   
        }
        Lcd_Shift_Right();
        PIR1bits.TMR1IF = 0;
        TMR1H = 0x3C;
        TMR1L = 0xB0; 
    }
}

void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    TRISB = 0;
    TRISD = 0;
    
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.SCS = 1;
    
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.TMR1IE = 1;
    PIR1bits.TMR1IF = 0;
    
    T1CONbits.T1CKPS1 = 1;
    T1CONbits.T1CKPS0 = 1;
    T1CONbits.TMR1CS = 0;
    T1CONbits.TMR1ON = 1;
    TMR1H = 0x3C;
    TMR1L = 0xB0;        
    
    I2C_Init_Master(I2C_100KHZ); //Setear I2C a 100kHz
}

uint8_t Read(uint8_t address){ //Función para obtener datos
    uint8_t dato = 0; //Variable temporal
    I2C_Start(); //Iniciar i2c
    I2C_Write(0xD0); //Introducir dirección del esclavo
    I2C_Write(address); //Introducir dirección 
    I2C_Restart(); //Restart i2c
    I2C_Write(0xD1); //Introducir dirección del esclavo más bit de escritura
    dato = I2C_Read(); //Almacenar dato en variable temporal
    I2C_Nack(); //Encender bit de not aknowledge e iniciar secuencia de reconocimiento  y transimitir el bit de reconocimiento
    I2C_Stop(); //Stop i2c
    __delay_us(10); //delay de 10 us
    return dato; //Retornar dato
}

void Read_Time(uint8_t *s, uint8_t *m, uint8_t *h){ //Función de obtener valores del tiempo
    *s = Bcd_to_Dec(Read(0x00)); //Obtener segundos
    *m = Bcd_to_Dec(Read(0x01)); //Obtener minutos
    *h = Bcd_to_Dec(Read(0x02)); //Obtener horas
}

void Read_Fecha(uint8_t *d, uint8_t *mo, uint8_t *y){
    *d = Bcd_to_Dec(Read(0x04)); //Obtener días
    *mo = Bcd_to_Dec(Read(0x05)); //Obtener mes
    *y = Bcd_to_Dec(Read(0x06)); //Obtener año
}

void Set_sec(uint8_t sec){ //Función para setear segundo
    I2C_Start(); //Iniciar I2C
    I2C_Write(0xD0); //Dirección del esclavo y bit de escritura
    I2C_Write(0x00); //Dirección del registro a modificar
    I2C_Write(Dec_to_Bcd(sec)); //Mandar dato en BCD
    I2C_Stop(); //Terminar i2c
}

void Set_min(uint8_t min){ //Función para setear minutos
    I2C_Start(); //Iniciar I2C
    I2C_Write(0xD0); //Dirección del esclavo y bit de escritura
    I2C_Write(0x01); //Dirección del registro a modificar
    I2C_Write(Dec_to_Bcd(min)); //Mandar dato en BCD
    I2C_Stop(); //Terminar i2c
}

void Set_hour(uint8_t hour){ //Función para setear horas
    I2C_Start(); //Iniciar I2C
    I2C_Write(0xD0); //Dirección del esclavo y bit de escritura
    I2C_Write(0x02); //Dirección del registro a modificar
    I2C_Write(Dec_to_Bcd(hour)); //Mandar dato en BCD
    I2C_Stop(); //Terminar i2c
}

void Set_day_week(uint8_t day_week){ //Función para setear día de semana
    I2C_Start(); //Iniciar I2C
    I2C_Write(0xD0); //Dirección del esclavo y bit de escritura
    I2C_Write(0x03); //Dirección del registro a modificar
    I2C_Write(Dec_to_Bcd(day_week)); //Mandar dato en BCD
    I2C_Stop(); //Terminar i2c
}

void Set_day(uint8_t day){ //Función para setear numero de día
    I2C_Start(); //Iniciar I2C
    I2C_Write(0xD0); //Dirección del esclavo y bit de escritura
    I2C_Write(0x04); //Dirección del registro a modificar
    I2C_Write(Dec_to_Bcd(day)); //Mandar dato en BCD
    I2C_Stop(); //Terminar i2c
}

void Set_month(uint8_t month){ //Función para setear mes
    I2C_Start(); //Iniciar I2C
    I2C_Write(0xD0); //Dirección del esclavo y bit de escritura
    I2C_Write(0x05); //Dirección del registro a modificar
    I2C_Write(Dec_to_Bcd(month)); //Mandar dato en BCD
    I2C_Stop(); //Terminar i2c
}

void Set_year(uint8_t year){ //Función para setear año
    I2C_Start(); //Iniciar I2C
    I2C_Write(0xD0); //Dirección del esclavo y bit de escritura
    I2C_Write(0x06);
    I2C_Write(Dec_to_Bcd(year)); //Mandar dato en BCD
    I2C_Stop(); //Terminar i2c
}

uint8_t Dec_to_Bcd(uint8_t dec_number){ //Función para pasar de numero decimal a bcd
    uint8_t bcd_number; //Variable para almacenar dato bcd
    bcd_number = 0; //Limpiar numero
    while(1){ //Loop
        if (dec_number >= 10){ //Convertir numero y repetir ciclo hasta que el numero sea menor que 10
            dec_number = dec_number - 10; //Restar 10
            bcd_number = bcd_number + 0b00010000; //Ir sumando diez en bcd
        }
        else { //Suma de números
            bcd_number = bcd_number + dec_number; //Suma
            break; //Salirse del loop
        }
    }
    return bcd_number; //Retornar valor BCD
}

uint8_t Bcd_to_Dec(uint8_t bcd){ //Función para pasar números de bcd a decimal
    uint8_t dec; //Variable para guardar valor
    dec = ((bcd>>4)*10)+(bcd & 0b00001111); // Hacer un corrimiento de bits y sumar con la unidad
    return dec; //Retornar valor
}

void AHT10_Init(void){
    __delay_ms(40);
    uint8_t status;
    I2C_Start();
    I2C_Write(0x70);
    I2C_Write(0x71);
    I2C_Restart();
    I2C_Write(0x71);
    status = I2C_Read();
    I2C_Nack();
    I2C_Stop();
    
    status = status & 0b00001000;
    
    //if (status != 0b00001000){
        I2C_Start();
        I2C_Write(0x70);
        I2C_Write(0xE1);
        I2C_Write(0x08);
        I2C_Write(0x00);
        I2C_Stop();
    //}
    __delay_ms(10);

}

void AHT10_Read(void){
    uint8_t data[7];
    float temperatura;
    
    uint8_t r;
    I2C_Start();
    I2C_Write(0x70);
    I2C_Write(0xAC);
    I2C_Write(0x33);
    I2C_Write(0x00);
    I2C_Stop();
    __delay_ms(80);
    
    I2C_Start();
    I2C_Write(0x70);
    I2C_Write(0x71);
    I2C_Restart();
    I2C_Write(0x71);
    r = I2C_Read();
    I2C_Nack();
    I2C_Stop();
    
    r = r & 0b00000000;
    while (r != 0b00000000);
    
    I2C_Start();
    I2C_Write(0x71);
    //for (uint8_t i = 0; i < 6; i++ ) {
	//	data[i] = I2C_Read();
    //    I2C_Ack();
	//}
    //data[6] = I2C_Read();
    data[0] = I2C_Read();
    I2C_Ack();
    data[1] = I2C_Read();
    I2C_Ack();
    data[2] = I2C_Read();
    I2C_Ack();
    data[3] = I2C_Read();
    I2C_Ack();
    data[4] = I2C_Read();
    I2C_Ack();
    data[5] = I2C_Read();
    I2C_Ack();
    data[6] = I2C_Read();
    I2C_Nack();
    I2C_Stop();
    
    //uint32_t hum = ((uint32_t)data[1] << 12) | ((uint16_t)data[2] << 4) | (data[3] >> 4);
	//hum = (hum * 250) >> 18;
	//*humid = (uint16_t)hum;
	temperatura = (((uint32_t)data[3] & 0x0F) << 16) + ((uint16_t)data[4] << 8) + data[5];
    temperatura = ((temperatura/1048576)*200-50);
    floattostr(temperatura, buffer2, 2);
    Lcd_Set_Cursor(1,12);
    Lcd_Write_String(buffer2);
}

void AHT10_Soft_Reset(void){
    __delay_ms(40);
    I2C_Start();
    I2C_Write(0x70);
    I2C_Write(0xBA);
    I2C_Stop();
    __delay_ms(25);
}

void Slave1_Total(void){
    //I2C_Start();
    //I2C_Write(0x50);
    //I2C_Write(0x03);
    //I2C_Stop();
    //__delay_ms(1);
    
    I2C_Start();
    I2C_Write(0x51);
    cont = I2C_Read();
    //I2C_Nack();
    I2C_Stop();
    __delay_us(100);  
}

void Slave1_Red(void){
    I2C_Start();
    I2C_Write(0x50);
    I2C_Write(0x00);
    I2C_Restart();
    I2C_Write(0x51);
    cont = I2C_Read();
    //NACK
    I2C_Stop();
    __delay_us(10);  
}

void Slave1_Green(void){
    I2C_Start();
    I2C_Write(0x50);
    I2C_Write(0x01);
    I2C_Restart();
    I2C_Write(0x51);
    cont = I2C_Read();
    //NACK
    I2C_Stop();
    __delay_us(10);  
}

void Slave1_Blue(void){
    I2C_Start();
    I2C_Write(0x50);
    I2C_Write(0x02);
    I2C_Restart();
    I2C_Write(0x51);
    cont = I2C_Read();
    //NACK
    I2C_Stop();
    __delay_us(10);  
}

void Slave1_Overheat(void){
    I2C_Start();
    I2C_Write(0x50);
    I2C_Write(0x04);
    I2C_Stop();
    __delay_us(10);  
}

void Slave1_Normal_Temperature(void){
    I2C_Start();
    I2C_Write(0x50);
    I2C_Write(0x05);
    I2C_Stop();
    
}

void Slave2(void){
    I2C_Start();
    I2C_Write(0x41);
    distancia = I2C_Read();
    I2C_Stop();
    __delay_us(10);
}