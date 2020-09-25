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

void I2C_Master_Init(void)
{
    
    ANSELD = 0x00;
//    PMD1 = 0xFF;
    
    TRISDbits.RD0 = 1;                            // Configura pinos SDA e SCL como entrada
    TRISDbits.RD1 = 1;                           
   
    SSP2CON1 = 0b00101000;                        //Modulo MSSP como mestre e sca e scl como serial
    SSP2CON2 = 0;
    SSP2ADD = ((_XTAL_FREQ/(400000))-1);        //c=400khz (_XTAL_FREQ/(4*c))-1
    SSP2STAT = 0;                                 // Sleew On (velocidade 400khz) (primeiro bit = 0)

}

void I2C_Master_Wait(void)
{
    while ((SSP2STAT & 0x04) || (SSP2CON2 & 0x1F)); //Transmissao esta em progresso.
}

char I2C_Livre (void)
{
    if(SSP2STATbits.R_W)                     //retorna 0 se existe transmissao em andamento
        return 0;
    if(SSP2CON2 & 0x17)                      //retorna 0 se existe algum evento de transmissao em andamento
        return 0;
    else    
        return 1;                           //retorna 1 se barramento esta livre   
}

void I2C_Master_Start(void)
{
    I2C_Master_Wait();
    SSP2CON2bits.SEN = 1;                    //Inicia a condicao START
}

void I2C_Master_RepeatedStart(void)
{
    I2C_Master_Wait();
    SSP2CON2bits.RSEN = 1;                   //Inicia a condicao Re-Start
}

void I2C_Master_Stop(void)
{
    I2C_Master_Wait();
    SSP2CON2bits.PEN = 1;                    //Inicia Stop Condicao
}

void I2C_Transmite (unsigned char dado_I2c)
{
    I2C_Master_Wait();
    SSP2BUF = dado_I2c;                      //carrega dado a ser transmitido no registrador SSPBUF
}

char I2C_Testa_ACK(void)
{
    if(!SSP2CON2bits.ACKSTAT)                //Escravo recebeu dado com sucesso
        return 1;
    else                                    // Erro na transmissao
        return 0;               
}

unsigned char I2C_Recebe(void)
{
    
    I2C_Master_Wait();
    SSP2CON2bits.RCEN = 1;                   // ativa mestre-receptor
    while (!SSP2STATbits.BF);                // aguarda chegada do dado
    return SSP2BUF;                          // retorna dado
}

void I2C_ACK (void)
{
    I2C_Master_Wait();
    SSP2CON2bits.ACKDT = 0;                 // Carrega bit ACK
    SSP2CON2bits.ACKEN = 1;                 //Inicia sequencia ACK
    while(SSP2CON2bits.ACKEN);              //aguarda terminar sequencia ACK
}   

void I2C_NACK (void)
{
    I2C_Master_Wait();
    SSP2CON2bits.ACKDT = 1;                  //carrega bit NACK
    SSP2CON2bits.ACKEN = 1;                  //inicia sequencia NACK
    while(SSP2CON2bits.ACKEN);               //aguarda terminar sequencia NACK
}


char I2C_Master_Escreve(unsigned char end_i2c, unsigned char dado_i2c)
{
    I2C_Master_Start();                
    I2C_Transmite(end_i2c|0x00);            //transmite endereco + escrita
    if(!I2C_Testa_ACK())                    //se erro na transmissao, aborta transmissao
    {
        I2C_Master_Stop();                  //gera bit STOP
        return -1;                          //erro na transmissao
    }
    I2C_Transmite(dado_i2c);                //transmite dado  
    if(!I2C_Testa_ACK())                    //se erro na transmissao, aborta transmissao
    {
        I2C_Master_Stop();                  //gera bit STOP
        return -1;                          //erro na transmissao
    }
    I2C_Master_Stop();                      //gera bit STOP
    return 0;                               //transmissao feita com sucesso
}

char I2C_Master_Escreve_Multiplos(unsigned char end_i2c, unsigned char *buffer, unsigned char cont_b)
{
    unsigned char cont = 0;
    I2C_Master_Start();        
    
    I2C_Transmite(end_i2c|0x00);            //transmite endereco + escrita
    
    if(!I2C_Testa_ACK())                    //se erro na transmissao, aborta transmiss?o
    {
        I2C_Master_Stop();                  //gera bit STOP
        return -1;                          //erro na transmissao
    }
    for(;cont < cont_b ; cont++)
    {
        I2C_Master_Wait();
        I2C_Transmite(buffer[cont]);        //transmite dado  
    }  
    if(!I2C_Testa_ACK())                    //se erro na transmissao, aborta transmissao
    {
        I2C_Master_Stop();                  //gera bit STOP
        return -1;                          //erro na transmissao
    }
    I2C_Master_Stop();                      //gera bit STOP
    return 0;                               //transmissao feita com sucesso
}

unsigned char I2C_Leitura (unsigned char end_i2c)
{
    unsigned char dado_i2c = 0;
    
    I2C_Master_Start();                     
    
    I2C_Transmite(end_i2c|0x01);             //envia endereco com primeiro bit sendo 1 (leitura)
    
    dado_i2c = I2C_Recebe();                 //Recebe dado
    
    I2C_NACK();
    
    I2C_Master_Stop();
    return dado_i2c;

}

void I2C_Leitura_Multiplos(unsigned char end_i2c, unsigned char cont_b)
{
    unsigned char cont = 0;
    I2C_Master_Start();
   
    I2C_Transmite(end_i2c|0x01);            //envia endereco com primeiro bit sendo 1 (leitura)
    
    for(; cont < cont_b ; cont++)          //recebe varios dados
    {
        buffer_read[cont] = I2C_Recebe();
        if(cont < (cont_b-1))
            I2C_ACK();                      //envia ACK porque nao recebeu todos dados ainda
        else
            I2C_NACK();                     //envia NACK quando recebeu todos os dados
    }
    
    I2C_Master_Stop();
}

void I2C_Leitura_Multiplos_Endereco(unsigned char end_i2c, unsigned char cont_b, unsigned char endereco)
{
    unsigned char cont = 0;
    
    I2C_Master_Start();
    
    I2C_Transmite(end_i2c|0x00);
    
    I2C_Transmite(endereco);
    
    I2C_Master_RepeatedStart();
    
    I2C_Transmite(end_i2c|0x01);            //envia endereco com primeiro bit sendo 1 (leitura)
    
    for(; cont < cont_b ; cont++)          //recebe varios dados
    {
        buffer_read[cont] = I2C_Recebe();
        if(cont < (cont_b-1))
            I2C_ACK();                      //envia ACK porque nao recebeu todos dados ainda
        else
            I2C_NACK();                     //envia NACK quando recebeu todos os dados
    }
    
    I2C_Master_Stop();
}

unsigned char I2C_Leitura_Endereco (unsigned char end_i2c, unsigned char endereco)
{
    unsigned char dado_i2c = 0;
    
    I2C_Master_Start();                     
    
    I2C_Transmite(end_i2c|0x00);
    
    I2C_Transmite(endereco);
    
    I2C_Master_RepeatedStart();
    
    I2C_Transmite(end_i2c|0x01);             //envia endereco com primeiro bit sendo 1 (leitura)
    
    dado_i2c = I2C_Recebe();                 //Recebe dado
    
    I2C_NACK();
    
    I2C_Master_Stop();
    return dado_i2c;

}

char I2C_Master_Escreve_Endereco(unsigned char end_i2c, unsigned char dado_i2c, unsigned char end_master)
{
    I2C_Master_Start();                
    I2C_Transmite(end_i2c|0x00);            //transmite endereco + escrita
    
    I2C_Transmite(end_master);
    
    if(!I2C_Testa_ACK())                    //se erro na transmissao, aborta transmissao
    {
        I2C_Master_Stop();                  //gera bit STOP
        return -1;                          //erro na transmissao
    }
    I2C_Transmite(dado_i2c);                //transmite dado  
    if(!I2C_Testa_ACK())                    //se erro na transmissao, aborta transmissao
    {
        I2C_Master_Stop();                  //gera bit STOP
        return -1;                          //erro na transmissao
    }
    I2C_Master_Stop();                      //gera bit STOP
    return 0;                               //transmissao feita com sucesso
}

char I2C_Master_Escreve_Multiplos_Endereco(unsigned char end_i2c, unsigned char *buffer, unsigned char cont_b, unsigned char end_master)
{
    unsigned char cont = 0;
    I2C_Master_Start();        
    
    I2C_Transmite(end_i2c|0x00);            //transmite endereco + escrita
    
    I2C_Transmite(end_master);
    
    if(!I2C_Testa_ACK())                    //se erro na transmissao, aborta transmiss?o
    {
        I2C_Master_Stop();                  //gera bit STOP
        return -1;                          //erro na transmissao
    }
    for(;cont < cont_b ; cont++)
    {
        I2C_Master_Wait();
        I2C_Transmite(buffer[cont]);        //transmite dado  
    }  
    if(!I2C_Testa_ACK())                    //se erro na transmissao, aborta transmissao
    {
        I2C_Master_Stop();                  //gera bit STOP
        return -1;                          //erro na transmissao
    }
    I2C_Master_Stop();                      //gera bit STOP
    return 0;                               //transmissao feita com sucesso
}