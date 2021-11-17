/*
 * File:        proyecto3.c
 * Dispositivo: Tiva C TM4123G
 * Autor:       Cristhofer Patzán
 * Compilador:  CCS V11
 *
 * Programa:    Informacion de parqueos en la nube
 * Hardware:    Tiva C TM4123G
 * Creación:    10 de noviembre de 2021
 * modificado:  14 de noviembre de 2021
 */
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
//#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "grlib/grlib.h"
//#include "drivers/cfal96x64x16.h"


#define XTAL 16000000       //16MHz


//------------------------------Variables Globales----------------------------
uint32_t i = 0;
uint8_t sensor1 = 0;
uint8_t sensor2 = 0;
uint8_t sensor3 = 0;
uint8_t sensor4 = 0;
uint8_t sensor5 = 0;
uint8_t sensor6 = 0;
uint8_t sensor7 = 0;
uint8_t sensor8 = 0;
uint8_t EspaciosLibres = 8;

//------------------------------Prototipos de Funciones-----------------------
void delay(uint32_t msec);
void delay1ms(void);
void InitUART0(void);
void InitUART1(void);
//------------------------------Función Principal-----------------------------

int main(void){
    // Configuración del oscilador externo, usando PLL y teniendo una frecuencia de 40MHz       datasheet pg223
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);    //Activo puerto A
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);    //Activo puerto B
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);    //Activo puerto C
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);    //Activo puerto D
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);    //Activo puerto E
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);    //Activo puerto F

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);             //RGB pines de salida
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_6);                                       //display7seg pines de salida
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_7 | GPIO_PIN_6);                          //display7seg pines de salida
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2 |GPIO_PIN_3 | GPIO_PIN_4);                          //display7seg pines de salida
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_7 | GPIO_PIN_6);                          //display7seg pines de salida
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);                                        //pines de entrada push
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);


    //                  puerto,         pin,        amperaje,          Pull-up u otros          -->pg 265
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4 , GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);   //Pull-up pin 4 puerto F (push1)
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_1 , GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);   //Pull-up pin 1 puerto E
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_2 , GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);   //Pull-up pin 2 puerto E
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_3 , GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);   //Pull-up pin 3 puerto E
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_4 , GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);   //Pull-up pin 4 puerto E
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0 , GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);   //Pull-up pin 0 puerto D
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_1 , GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);   //Pull-up pin 1 puerto D
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_2 , GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);   //Pull-up pin 2 puerto D
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_3 , GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);   //Pull-up pin 3 puerto D

    InitUART0();
    InitUART1();

    //-------------------------------Loop Principal------------------------------
    while(1){

        sensor1 = GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_0);
        sensor2 = GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_1);
        sensor3 = GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2);
        sensor4 = GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_3);
        sensor5 = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1);
        sensor6 = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2);
        sensor7 = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3);
        sensor8 = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_4);

        if (sensor1 == 0 ){
            //GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x02);//ROJO
            //UARTCharPut(UART0_BASE, '1');
            UARTCharPut(UART1_BASE, 'a');
            EspaciosLibres--;
            //delay(500);
        }else{
            UARTCharPut(UART1_BASE, 'A');
        }
        if (sensor2 == 0 ){
           // GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x04);//AZUL
            //UARTCharPut(UART0_BASE, '2');
            UARTCharPut(UART1_BASE, 'b');
            EspaciosLibres--;
           // delay(500);
        }else{
            UARTCharPut(UART1_BASE, 'B');
        }
        if (sensor3 == 0){
            //GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x08);//VERDE
           // UARTCharPut(UART0_BASE, '3');
            UARTCharPut(UART1_BASE, 'c');
            EspaciosLibres--;
           // delay(500);
        }else{
            UARTCharPut(UART1_BASE, 'C');
        }
        if (sensor4 == 0 ){
            //GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x06);
           // UARTCharPut(UART0_BASE, '4');
            UARTCharPut(UART1_BASE, 'd');
            EspaciosLibres--;
           // delay(500);
        }else{
            UARTCharPut(UART1_BASE, 'D');
        }
        if (sensor5 == 0 ){
            //GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x0A);
            //UARTCharPut(UART0_BASE, '5');
            UARTCharPut(UART1_BASE, 'e');
            EspaciosLibres--;
           // delay(500);
        }else{
            UARTCharPut(UART1_BASE, 'E');
        }
        if (sensor6 == 0 ){
            //GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x0C);
           // UARTCharPut(UART0_BASE, '6');
            UARTCharPut(UART1_BASE, 'f');
            EspaciosLibres--;
            //delay(500);
        }else{
            UARTCharPut(UART1_BASE, 'F');
        }
        if (sensor7 == 0 ){
           // GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x0E);
           // UARTCharPut(UART0_BASE, '7');
            UARTCharPut(UART1_BASE, 'g');
            EspaciosLibres--;
            //delay(500);
        }else{
            UARTCharPut(UART1_BASE, 'G');
        }
        if (sensor8 == 0 ){
           // GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x0A);
            //UARTCharPut(UART0_BASE, '8');
            UARTCharPut(UART1_BASE, 'h');
            EspaciosLibres--;
            //delay(500);
           // GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x0C);
            //delay(500);
           // GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x00);
        }else{
            UARTCharPut(UART1_BASE, 'H');
        }

        switch(EspaciosLibres){
        case(0):
                UARTCharPut(UART1_BASE, '0');
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2 |GPIO_PIN_3 | GPIO_PIN_4, 0B00011100);   //A ,B
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7 | GPIO_PIN_6, 0B01000000);   //X, C
                GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0B01000000);   //D, E
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7 | GPIO_PIN_6, 0B01000000);   //G, F
                break;
        case(1):
                UARTCharPut(UART1_BASE, '1');
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2 |GPIO_PIN_3 | GPIO_PIN_4, 0B00001000);   //A ,B
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7 | GPIO_PIN_6, 0B01000000);   //X, C
                GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0B00000000);   //D, E
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7 | GPIO_PIN_6, 0B00000000);   //G, F
                break;
        case(2):
                UARTCharPut(UART1_BASE, '2');
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2 |GPIO_PIN_3 | GPIO_PIN_4, 0B00011100);   //A ,B
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7 | GPIO_PIN_6, 0B00000000);   //X, C
                GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0B01000000);   //D, E
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7 | GPIO_PIN_6, 0B10000000);   //G, F
                break;
        case(3):
                UARTCharPut(UART1_BASE, '3');
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2 |GPIO_PIN_3 | GPIO_PIN_4, 0B00011100);   //A ,B
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7 | GPIO_PIN_6, 0B01000000);   //X, C
                GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0B00000000);   //D, E
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7 | GPIO_PIN_6, 0B10000000);   //G, F
                break;
        case(4):
                UARTCharPut(UART1_BASE, '4');
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2 |GPIO_PIN_3 | GPIO_PIN_4, 0B00001000);   //A ,B
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7 | GPIO_PIN_6, 0B01000000);   //X, C
                GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0B00000000);   //D, E
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7 | GPIO_PIN_6, 0B11000000);   //G, F
                break;
        case(5):
                UARTCharPut(UART1_BASE, '5');
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2 |GPIO_PIN_3 | GPIO_PIN_4, 0B00010100);   //A ,B
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7 | GPIO_PIN_6, 0B01000000);   //X, C
                GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0B00000000);   //D, E
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7 | GPIO_PIN_6, 0B11000000);   //G, F
                break;
        case(6):
                UARTCharPut(UART1_BASE, '6');
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2 |GPIO_PIN_3 | GPIO_PIN_4, 0B00010100);   //A ,B
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7 | GPIO_PIN_6, 0B01000000);   //X, C
                GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0B01000000);   //D, E
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7 | GPIO_PIN_6, 0B11000000);   //G, F
                break;
        case(7):
                UARTCharPut(UART1_BASE, '7');
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2 |GPIO_PIN_3 | GPIO_PIN_4, 0B00011000);   //A ,B
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7 | GPIO_PIN_6, 0B01000000);   //X, C
                GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0B00000000);   //D, E
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7 | GPIO_PIN_6, 0B00000000);   //G, F
                break;
        case(8):
                UARTCharPut(UART1_BASE, '8');
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2 |GPIO_PIN_3 | GPIO_PIN_4, 0B00011100);   //A ,B
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7 | GPIO_PIN_6, 0B01000000);   //X, C
                GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0B01000000);   //D, E
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7 | GPIO_PIN_6, 0B11000000);   //G, F
                break;
        }
        EspaciosLibres = 8;
        delay(1000);
    }

    return 0;
}

//--------------------------------------Funciones--------------------------------

void delay1ms(void){            //funcion para hacer delay de 1 milisegundo
    SysTickDisable();
    SysTickPeriodSet(16000);
    SysTickEnable();
    while ((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT) == 0); //Pg. 138
}

void delay(uint32_t msec){      //funcion para delay en milisegundos
    for (i = 0; i < msec; i++){
        delay1ms();
    }
}

void InitUART0(void)
{
    /*Enable the GPIO Port A*/
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    /*Enable the peripheral UART Module 0*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    /* Make the UART pins be peripheral controlled. */
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* Sets the configuration of a UART. */
    UARTConfigSetExpClk(
            UART0_BASE, SysCtlClockGet(), 115200,
            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    //UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}

void InitUART1(void){
    /*Enable the GPIO Port B*/
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    /*Enable the peripheral UART Module 1*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);

    /* Make the UART pins be peripheral controlled. */
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* Sets the configuration of a UART. */
    UARTConfigSetExpClk(
            UART1_BASE, SysCtlClockGet(), 115200,
            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTEnable(UART1_BASE);
    //UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
}
