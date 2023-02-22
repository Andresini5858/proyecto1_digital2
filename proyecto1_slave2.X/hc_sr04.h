/* #include "hc_sr04.h"
*
* Creada por: Ing. Abiezer Hernandez O.
* Fecha de creacion: 23/02/2020
* Electronica y Circuitos
*
*/

#include <xc.h>
#define _XTAL_FREQ 8000000

#define TRIG_DIR TRISDbits.TRISD3
#define ECHO_DIR TRISDbits.TRISD2

#define TRIG_PIN PORTDbits.RD3
#define ECHO_PIN PORTDbits.RD2

void HCSR04_Init(void);
float HCSR04_Get_Distance(void);