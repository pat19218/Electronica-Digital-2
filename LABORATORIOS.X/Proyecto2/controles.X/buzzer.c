/* File:   PROYECTO.c
 * Dispositivo:	PIC16f887
 * Author: isaac
 *
 * Compilador:	pic-as (v2.30), MPLABX V5.45
 * 
 * Programa: Pic BUZZER PASIVO
 * Hardware: Pic 16f887, 2 pot, comunicación serial, BUZZER PASIVO
 * 
 * Created on 13 de octubre 2021, 11:38
 * Last modification 13 de octubre 2021, 21:51
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
char dato1, dato2;
//--------------------------funciones-------------------------------------------

//---------------------------interrupciones-------------------------------------

void __interrupt()isr(void){
     if(PIR1bits.ADIF == 1){
        
            CCPR1L = dato1;
            
        PIR1bits.ADIF = 0;
        
    }
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
    
    /*
     * PR2 = [PERIODO/(4*Tosc*PRESCALER_TMR2)]-1
     * PR2 = 255    periodo de 2ms
     * Duty_cicle = 0-100%
     */
                            //Config. PWM
    TRISCbits.TRISC2 = 1;   //RC2/CCP1 como entrada
    //TRISDbits.TRISD5 = 1;   //RC2/CCP1 como entrada
    
    PR2 = 255;              //Valor de pwm
    
    CCP1CONbits.P1M = 0;            //modulo en config PWM primer canal
    CCP1CONbits.CCP1M = 0b1100; 
    CCPR1L = 0x0f;                  //inicio de ciclo de trabajo
    
    //CCP2CONbits.CCP2M = 0;        //modulo en config PWM segundo canal
    //CCP2CONbits.CCP2M = 0b1100; 
    //CCPR2L = 0x0f;                //inicio de ciclo de trabajo
    
    CCP1CONbits.DC1B = 0;           //
    //CCP2CONbits.DC2B0 = 0;
    //CCP2CONbits.DC2B1 = 0;
    //
    PIR1bits.TMR2IF = 0;     //bajo la bandera
    T2CONbits.T2CKPS = 0b11; //pre-escaler 1:16
    T2CONbits.TMR2ON = 1;    //Enciendo el timmer 2
    while(PIR1bits.TMR2IF == 0);    //espero a completar el un ciclo tmr2
    PIR1bits.TMR2IF = 0;
    TRISCbits.TRISC2 = 0;           // Salida PWM
    //TRISDbits.TRISD5 = 0;           // Salida PWM
    
    
    
                                    //confi. interrupciones
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1; 
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

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
        
        __delay_ms(10);
    }
    return;
}
