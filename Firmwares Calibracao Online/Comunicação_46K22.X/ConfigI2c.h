/*
 * File:   ConfigI2c.h
 * Author: Marcus Vinicius de Lima Rebolo
 *
 * Created on 05 de November de 2018, 22:20
 */

#ifndef CONFIGI2C_H
#define	CONFIGI2C_H
#define __Interrupt_I2c__            if(PIR1bits.SSPIF || PIR1bits.SSP1IF)

#define ENDERECO_MASTER         0
#define ENDERECO_COMUNICACAO        0x30
#define ENDERECO_GERENCIAMENTO      0x40
#define ENDERECO_ADMISSAO           0x50
#define ENDERECO_SINCRONISMO        0x60
//------------------------------------------------------------------------------
// FUNCOES
//------------------------------------------------------------------------------

void I2C_Slave_Init(unsigned char address);

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* CONFIGI2C_H */

