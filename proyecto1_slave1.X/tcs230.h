/* #include "tcs230.h"
*
* Creada por: Ing. Abiezer Hernandez O.
* Fecha de creacion: 13/05/2020
* Electronica y Circuitos
*
*/

#include <xc.h>
#define _XTAL_FREQ 8000000

#define OUT_DIR TRISDbits.TRISD3
#define S2_DIR TRISDbits.TRISD2
#define S3_DIR TRISDbits.TRISD1

#define OUT PORTDbits.RD3
#define S2_PIN PORTDbits.RD2
#define S3_PIN PORTDbits.RD1

#define CHANNEL_R 0x01
#define CHANNEL_G 0x02
#define CHANNEL_B 0x03

void TCS230_Init(void);
uint16_t TCS230_Get_Value(short channel);