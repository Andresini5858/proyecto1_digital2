/* #include "hc_sr04.h"
*
* Creada por: Ing. Abiezer Hernandez O.
* Fecha de creacion: 23/02/2020
* Electronica y Circuitos
*
*/

#include "hc_sr04.h"

void HCSR04_Init(void)
{
    TRIG_DIR = 0;
    ECHO_DIR = 1;
    TRIG_PIN = 0;
    T1CON = 0xF8;
    T1CONbits.TMR1ON = 1;
}

float HCSR04_Get_Distance(void)
{
    unsigned int timer_high;
    unsigned int timer_low;
    
    TRIG_PIN = 1;
    __delay_us(10);
    TRIG_PIN = 0;
    while(ECHO_PIN == 0);
    TMR1H = 0x00;
    TMR1L = 0x00;
    while(ECHO_PIN == 1);
    timer_low = TMR1L;
    timer_high = TMR1H;
    float dis_hc = (((timer_high << 8) + timer_low)*0.000666*34.0)/2.0;
    return dis_hc;
}