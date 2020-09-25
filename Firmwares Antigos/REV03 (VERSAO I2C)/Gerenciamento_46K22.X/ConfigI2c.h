/*
 * File:   ConfigI2c.h
 * Author: Marcus Vinicius de Lima Rebolo
 *
 * Created on 05 de November de 2018, 22:20
 */

#ifndef CONFIGI2C_H
#define	CONFIGI2C_H

/*
 * caso queira usar a comunicação I2C por interrupção, é necessario ativar a interrupção e sempre no final da
 * condição START/STOP/ACK/NACK, o bit SSPIF(PIR1<3>) é setado, sendo necessário apagar por software.
 * 
 * Todos os outros bits nos quais são setados sozinhos por hardware (START, STOP, ACK, etc..),
 * nao precisa ser apagado por software pois são apagados automaticamente por hardware.
 * 
 */
#define ENDERECO_COMUNICACAO        0x30
#define ENDERECO_GERENCIAMENTO      0x40
#define ENDERECO_ADMISSAO           0x50
#define ENDERECO_SINCRONISMO        0x60

#define DADOS_ENVIADOS              17

//------------------------------------------------------------------------------
// VARIAVEIS
//------------------------------------------------------------------------------

unsigned char buffer_read[20];

//------------------------------------------------------------------------------
// FUNCOES
//------------------------------------------------------------------------------

/*
 * Função: void I2C_Master_Init(void);
 * 
 * Inicialização de dispositivo como mestre e velocidade de 400KHz, alem de declarar pinos SDA e SCL como entrada.
 * 
 */
void I2C_Master_Init(void);

/*
 * Função: void I2C_Master_Wait(void);
 * 
 * Função para esperar comunicação ser concluida.
 * 
 */
void I2C_Master_Wait(void);

/*
 * Função: char I2C_Livre (void);
 * 
 * Avisa que barramento esta livre para inicio de comunicação
 * 
 */
char I2C_Livre (void);

/*
 * Função: void I2C_Master_Start(void);
 * 
 * Inicio de comunicação
 * 
 */
void I2C_Master_Start(void);

/*
 * Função: void I2C_Master_RepeatedStart(void);
 * 
 * Repetição de inicio de comunicação
 * 
 */
void I2C_Master_RepeatedStart(void);

/*
 * Função: void I2C_Master_Stop(void);
 * 
 * Stop da comunicação
 * 
 */
void I2C_Master_Stop(void);

/*
 * Função: char I2C_Master_Escreve(unsigned char end_i2c, unsigned char dado_i2c);
 * 
 * Transmite um byte via I2c com endereçamento de 7 bits
 * 
 */
char I2C_Master_Escreve(unsigned char end_i2c, unsigned char dado_i2c);

/*
 * Função: char I2C_Master_Escreve_Multiplos(unsigned char end_i2c, unsigned char *buffer, unsigned char cont_b);
 * 
 * Transmite varios bytes via I2c com endereçamento de 7 bits
 * 
 */
char I2C_Master_Escreve_Multiplos(unsigned char end_i2c, unsigned char *buffer, unsigned char cont_b);
/*
 * Função: void I2C_Transmite (char dado_I2c);
 * 
 * Esta função inicializa a transmissão de um byte e detecta o seu fim.
 * 
 */
void I2C_Transmite (unsigned char dado_I2c);

/*
 * Função: char I2C_Testa_ACK(void);
 * 
 * Testa bit ACK, se retorna 1, escravo recebeu dado com sucesso, se 0, erro na transmissão
 * 
 */
char I2C_Testa_ACK(void);

/*
 * Função: char I2C_Recebe(void);
 * 
 * Testa se chegou dado e retorna valor do dado para função
 * 
 */
unsigned char I2C_Recebe(void);

/*
 * Função: void I2C_ACK (void);
 * 
 * Gera o bit ACK.
 * 
 */
void I2C_ACK (void);

/*
 * Função: void I2C_NACK (void);
 * 
 * Gera o bit NACK.
 * 
 */
void I2C_NACK (void);

/*
 * Função: char I2C_Leitura(unsigned int end_i2c);
 * 
 * Esta função efetua a leitura de um byte via barramento I2C com endereçamento de 7 bits
 * 
 */
unsigned char I2C_Leitura(unsigned char end_i2c);

/*
 * Função: void I2C_Leitura_Multiplos(unsigned char end_i2c, unsigned char cont_b);
 * 
 * Esta função efetua a leitura varios bytes via barramento I2C com endereçamento de 7 bits
 * 
 */
void I2C_Leitura_Multiplos(unsigned char end_i2c, unsigned char cont_b);

/*
 * Função: void I2C_Leitura_Multiplos_Endereco(unsigned char end_i2c, unsigned char cont_b, unsigned char endereco);
 * 
 * Esta função efetua a leitura varios bytes via barramento I2C com endereçamento de 7 bits + envio de endereco do mestre
 * 
 */
void I2C_Leitura_Multiplos_Endereco(unsigned char end_i2c, unsigned char cont_b, unsigned char endereco);

/*
 * Função: unsigned char I2C_Leitura (unsigned char end_i2c, unsigned char endereco);
 * 
 * Esta função efetua a leitura de um byte via barramento I2C com endereçamento de 7 bits + envio de endereco do mestre
 * 
 */
unsigned char I2C_Leitura_Endereco(unsigned char end_i2c, unsigned char endereco);

char I2C_Master_Escreve_Endereco(unsigned char end_i2c, unsigned char dado_i2c, unsigned char end_master);

char I2C_Master_Escreve_Multiplos_Endereco(unsigned char end_i2c, unsigned char *buffer, unsigned char cont_b, unsigned char end_master);

#endif	/* CONFIGI2C_H */

