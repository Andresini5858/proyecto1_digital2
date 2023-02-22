/* #include "tcs230.h"
*
* Creada por: Ing. Abiezer Hernandez O.
* Fecha de creacion: 13/05/2020
* Electronica y Circuitos
*
*/

#include "tcs230.h"

void TCS230_Init(void)
{
    S2_DIR = 0;
    S3_DIR = 0;
    OUT_DIR = 1;
    S2_PIN = 0;
    S3_PIN = 0;
    T1CON = 0xF8;
    T1CONbits.TMR1ON = 1;
}

unsigned int TCS230_Get_Value(short channel)
{
    unsigned int tcs_value = 0;
    switch(channel)
    {
        case CHANNEL_R:
            S2_PIN = 0;
            S3_PIN = 0;
            while(OUT_PIN == 1);
            while(OUT_PIN == 0);
            while(OUT_PIN == 1);
            TMR1 = 0x0000;
            while(OUT_PIN == 0);
            tcs_value = TMR1;
            break;
            
        case CHANNEL_G:
            S2_PIN = 1;
            S3_PIN = 1;
            while(OUT_PIN == 1);
            while(OUT_PIN == 0);
            while(OUT_PIN == 1);
            TMR1 = 0x0000;
            while(OUT_PIN == 0);
            tcs_value = TMR1;
            break;
            
        case CHANNEL_B:
            S2_PIN = 0;
            S3_PIN = 1;
            while(OUT_PIN == 1);
            while(OUT_PIN == 0);
            while(OUT_PIN == 1);
            TMR1 = 0x0000;
            while(OUT_PIN == 0);
            tcs_value = TMR1;
            break;
    }
    return tcs_value;
}