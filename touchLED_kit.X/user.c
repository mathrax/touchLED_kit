/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <xc.h>         /* XC8 General Include File */


#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"

/******************************************************************************/
/* User Functions                                                             */

#define _XTAL_FREQ      32000000
/******************************************************************************/

void InitApp(void) {
    unsigned int i;

    //8Mhz
    OSCCON = 0b01110010;
    
    //ALL DIGITAL
    ANSELA = 0x00;  

    //PWM
    //    APFCONbits.CCP1SEL = 1; //RA5..CCP1
    //
    //    CCP1CON = 0b10001100;
    //    T2CON = 0b00000000;
    //    PSTR1CON = 0b00000000;
    //
    //    PR2 = 20; //200KHz
    //    PWM1CON = PR2 / 4;
    //    CCPR1L = PR2 / 2;
    //    T2CON |= 0b00000100;

    //TRIS
    TRISA5=0;   //LED

    


    LATAbits.LATA5 = 1;
    __delay_ms(100);
    LATAbits.LATA5 = 0;
    __delay_ms(100);
    LATAbits.LATA5 = 1;
    __delay_ms(100);
    LATAbits.LATA5 = 0;
    __delay_ms(100);

}

