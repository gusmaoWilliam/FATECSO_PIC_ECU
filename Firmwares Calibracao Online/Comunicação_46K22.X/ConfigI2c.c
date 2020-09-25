/*
 * File:   ConfigI2c.c
 * Author: Marcus Vinicius de Lima Rebolo
 *
 * Created on 05 de November de 2018, 22:20
 */

//------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------

#include <pic18f46k22.h>
#include "ConfigI2c.h"

#define _XTAL_FREQ 40000000
//------------------------------------------------------------------------------
// FUNCOES DE CONFIGURACAO E ORGANIZACAO
//------------------------------------------------------------------------------


void I2C_Slave_Init(unsigned char address) 
{
    
    ANSELC = 0x00;
//    PMD1 = 0xFF;
    TRISCbits.RC3 = 1;                         // Configura pinos SDA e SCL como entrada
    TRISCbits.RC4 = 1;                         // .......
    TRISCbits.RC5 = 1; 
    
    SSP1CON1 = 0b00110110;               //Módulo MSSP como mestre e sca e scl como serial
    SSP1CON2 = 0x01;                     // escravo em repouso stretching setado
    SSP1ADD = address;                   //Endereço do slave 7 bits address, primeiro bit read'1'/write'0'
    SSP1STAT = 0x00;                     // coloca o escravo em repouso 400khz sleew on
//    SSP1IP = 1;
    INTCONbits.GIE=1; //habilita interrupção global;
    INTCONbits.PEIE=1; //habilita interrupção dos perifericos;
    PIE1bits.SSPIE = 1; 
    PIE1bits.SSP1IE = 1;
    PIR1bits.SSP1IF = 0;                 // limpa flag de interrupção
    PIR1bits.SSPIF = 0;
}