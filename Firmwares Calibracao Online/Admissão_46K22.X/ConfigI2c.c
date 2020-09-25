
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
    
    TRISDbits.RD0 = 1;                            // Configura pinos SDA e SCL como entrada
    TRISDbits.RD1 = 1;                           
    
    SSP2CON1 = 0b00110110;               //Módulo MSSP como mestre e sca e scl como serial
    SSP2CON2 = 0x01;                     // escravo em repouso stretching setado
    SSP2ADD = address;                   //Endereço do slave 7 bits address, primeiro bit read'1'/write'0'
    SSP2STAT = 0x00;                     // coloca o escravo em repouso 400khz sleew on
//    SSP1IP = 1;
    INTCONbits.GIE=1; //habilita interrupção global;
    INTCONbits.PEIE=1; //habilita interrupção dos perifericos;
    PIE3bits.SSP2IE = 1;
    PIR3bits.SSP2IF = 0;                 // limpa flag de interrupção
}