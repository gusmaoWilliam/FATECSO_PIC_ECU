#ifndef ROT_DELAY_H
#define	ROT_DELAY_H

#define _XTAL_FREQ 40000000
//------------------------------------------------------------------------------
// PROTÓTIPOS DE FUNÇÕES
//------------------------------------------------------------------------------


/**
 * Rotina para delay em milisegundos.
 * 
 * @since 02/08/2017
 * 
 * @param milli Tempo em milisegundos.
 */
void delayMs(unsigned long int milli);


/**
 * Rotina para delay em microsegundos.
 * 
 * @since 02/08/2017
 * 
 * @param micro Tempo em microsegundos.
 */
void delayUs(unsigned long int micro);


/**
 * Rotina para delay em segundos.
 * 
 * @since 02/08/2017
 * 
 * @param sec Tempo em segundos.
 * 
 */
void delaySc(unsigned long int sec);

#endif	/* ROT_DELAY_H */