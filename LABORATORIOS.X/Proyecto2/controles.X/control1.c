/* File:   PROYECTO.c
 * Dispositivo:	PIC16f887
 * Author: isaac
 *
 * Compilador:	pic-as (v2.30), MPLABX V5.45
 * 
 * Programa: Pic centro de mandos
 * Hardware: Pic 16f887, 2 pot, comunicación serial
 * 
 * Created on 11 de octubre 2021, 16:51
 * Last modification 11 de octubre 2021, 21:51
 */

// PIC16F887 Configuration Bit Settings
// 'C' source line config statements
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
#pragma config LVP = OFF         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//-------------------------Librerias--------------------------------------------
#include <xc.h>
#include <stdint.h>
#include "USART.h"

//--------------------------directivas del compilador---------------------------
#define _XTAL_FREQ 8000000 //__delay_ms(x)

//---------------------------variables------------------------------------------
char ingreso;
char dato2, dato1;
char centena1, decena1, unidad1;
char centena2, decena2, unidad2;

//--------------------------funciones-------------------------------------------
char centenas (int dato);
char decenas (int dato);
char unidades (int dato);
//---------------------------interrupciones-------------------------------------

void __interrupt()isr(void){
    
}

//----------------------configuracion microprocesador---------------------------

void main(void) {
    USART_Init();
    ANSEL = 0b01100000; //pin 5 y 6 analogicos
    ANSELH = 0x00;      // solo pines digitales
    
    TRISC = 0b10000000;  
    TRISE = 0b111;     //pin 0 y 1 entradas
    
    OSCCONbits.IRCF = 0b111; //Config. de oscilacion 8MHz
    OSCCONbits.SCS = 1;      //reloj interno
    
                           
                            //Config. ADC
    ADCON1bits.ADFM = 0;    //Justificado a la izquierda
    ADCON1bits.VCFG0 = 0;   //voltaje de 0V-5V
    ADCON1bits.VCFG1 = 0;
    ADCON0bits.ADCS0 = 0;   //Fosc/32
    ADCON0bits.ADCS1 = 1;
    ADCON0bits.CHS = 5;     //canal 5 pin 8
    __delay_us(100);
    ADCON0bits.ADON = 1;    //activo el modulo
    
    ADCON0bits.GO = 1;
    centena1 = 0;
    decena1 = 0;
    unidad1 = 0;
    centena2 = 0;
    decena2 = 0;
    unidad2 = 0;
    PORTA = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    
    
//------------------------------loop principal----------------------------------
    while (1){
        if(ADCON0bits.GO == 0){
            
            if(ADCON0bits.CHS == 6){        //leo canal 6 y cambio al 5
                dato2 = ADRESH;
                ADCON0bits.CHS = 5;
            }
            else if(ADCON0bits.CHS == 5){   //leo canal 5 y cambio al 6
                dato1 = ADRESH;
                ADCON0bits.CHS = 6;
            }
            __delay_us(50);     //con 6 micros segundos será suficiente se dejo
                                //en 50 por fallos de software en proteus
            ADCON0bits.GO = 1;
        }
        centena1 = centenas(dato1) + 48;
        decena1 = decenas(dato1) + 48;
        unidad1 = unidades(dato1) + 48;
        
        centena2 = centenas(dato2) + 48;
        decena2 = decenas(dato2) + 48;
        unidad2 = unidades(dato2) + 48;
        
        if (PIR1bits.RCIF == 1){ //compruebo si se introdujo un dato
            ingreso = USART_Rx();
            
            if(ingreso == 'h'){
                USART_Tx(centena1);//variable a enviar con h
                USART_Tx(decena1);//variable a enviar con h
                USART_Tx(unidad1);//variable a enviar con h
                ingreso = 0;
            }
            else if(ingreso == 't'){
                USART_Tx(centena2);//variable a enviar con t
                USART_Tx(decena2);//variable a enviar con t
                USART_Tx(unidad2);//variable a enviar con t
                ingreso = 0;
            }
             
            PIR1bits.RCIF = 0;
        }
        __delay_ms(10);
    }
    return;
}

char centenas (int dato){
    char out =(char) dato / 100;
    return out;
}

char decenas (int dato){
    char out;
    out = (dato % 100) / 10;
    return out;
}

char unidades (int dato){
    char out;
    out = (dato % 100) % 10;
    return out;
}
