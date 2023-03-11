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
}

uint16_t TCS230_Get_Value(short channel)
{
    uint16_t tcs_value = 0;
    uint16_t count = 0;
    switch(channel)
    {
        case CHANNEL_R:
            S2_PIN = 0;
            S3_PIN = 0;
            
            while (OUT == 0);
            while (OUT == 1) {
            count++;
            }

            return count;
            
            break;
            
        case CHANNEL_G:
            S2_PIN = 1;
            S3_PIN = 1;
  
            while (OUT == 0);
            while (OUT == 1) {
            count++;
            }

            return count;
            break;
            
        case CHANNEL_B:
            S2_PIN = 0;
            S3_PIN = 1;
  
            while (OUT == 0);
            while (OUT == 1) {
            count++;
            }

            return count;
    }
    return tcs_value;
}