/*******************************************************************************************************
 * File:   main.c
 * Author: Hiroki
 *
 * Created on 14 de Julho de 2016, 20:33
 * SW COMUNICAÇÃO VERSÃO 1.0
 * Autor: Gustavo Hiroki
 * Modificação: Início declarações e RS232
 * 14/07/2016
 ******************************************************************************************************/
/*******************************************************************************************************
 * File:   main.c
 * Author: Hiroki
 *
 * Created on 14 de Outubro de 2016, 00:38
 * SW COMUNICAÇÃO VERSÃO 1.0
 * Autor: Gustavo Hiroki
 * Modificação: Implementação de Timer para COM GOL
 * 14/10/2016
 ******************************************************************************************************/
/*******************************************************************************************************
 * File:   main.c
 * Author: Hiroki
 *
 * Created on 18 de Outubro de 2016, 13:06
 * SW COMUNICAÇÃO VERSÃO 1.0
 * Autor: Gustavo Hiroki
 * Modificação: Condilção de atualização.
 * 18/10/2016
 /******************************************************************************************************/
/* File:   main.c
 * Author: PLANK TEAM
 * Modificação: Implementação Sensor Composição de Combustível.
 * 21/10/2017 */
/**************************************** INCLUDE LIBRARY ********************************************/
#include <xc.h>
//#include <usart.h>
#include "RS232_K22.h"
#include "ADC_K22.h"
#include "Config_XC8.h"
#include "EXT_INT.h"
#include "SPI_XC8.h"
#include "TIMERS_XC8.h"
#include "stdlib.h"
#include "MCP2515.h"
#include "ConfigI2c.h"
#include "Rot_Delay.h"
#include "lcd4bitsmode.h"

/******************************************* DEFINES ************************************************/
#define SPI_MAX 10//11 /* Numero de bytes recebidos do Ger pelo SPI */
//#define SPI_MAX 1 /* Numero de bytes recebidos do Ger pelo SPI */

#define nPin_LOW    0
#define nPin_HIGH   1

#define L15         PIN_B0
#define nLED_TEST   LATAbits.LA0
#define nPORT_L15   PORTBbits.RB0
#define nON         1
#define nOFF        0
#define nFT_CTS         LATCbits.LATC1
#define nFT_RTS         LATCbits.LATC0

#define _1msTmr     55535       /*For Preescaler 1:1*/
#define _5msTmr     15536       /*For Preescaler 1:1*/
#define _10msTmr    40535       /*For Preescaler 1:4*/
#define _20msTmr    15535       /*For Preescaler 1:4*/

#define RotTimeOut  10
#define TempTimeOut 20

#define RS PORTEbits.RE0
#define RW PORTEbits.RE1
#define E  PORTEbits.RE2

#define nVERSION    "    2 . 1"
/********************************** Variaveis globais *************************************************/
unsigned int txbuff[] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int can_ctr;

unsigned long int rotacao;
short int lcd_ctr; /* Apenas controla a impressao no LCD */

//unsigned int valor[SPI_MAX]; /* Guarda os dados recebidos via interrupcao do SPI */
unsigned int spi_ctr; /* Indice de controle do vetor "valor[]" */
unsigned int teste_envio_william = 0;
unsigned int pedal_ad = 0;
unsigned int map = 0;
unsigned int ig_dente_14 = 0;
unsigned int tempo_bob = 0;
unsigned int tempo_inj = 0;
float tempo_inj_com = 0;
unsigned int ij_dente_14;
unsigned int temp_bob = 0;
unsigned int t_ar_com = 0;
unsigned int t_agua = 0;
unsigned int ref_rpm = 0;
unsigned int ig_tmr_disparo;
unsigned int ij_tmr_disparo;
long long rotacao_4;
unsigned char char_serial_ant = 0;
unsigned char char_serial = 0;


unsigned char flag_serial = 0;
/*************************Sensor de composição de combustível*******************/
unsigned int SCC = 0, cont_SCC = 0, porc_etanol = 0;

/* Testes */
unsigned int Lambda = 1;
unsigned int VB = 0;
unsigned int BAT = 12;

typedef enum
{
    Serial_Init = 0,
    Serial_Pedal = Serial_Init,
    Serial_Lambda,
    Serial_MAP,
    Serial_Temp,
    Serial_Rot,
    Serial_VB,
    Serial_Ag,
    Serial_Inj,
    Serial_Bob,
    Serial_Bat,
    Serial_L15,
    Serial_DataMax
} Serial_Data;
Serial_Data Select_Data;

unsigned int Old_Value[Serial_DataMax] = 0;
unsigned int Count = 0;
unsigned int TempMotor = 0;
unsigned int UpdateRot = 0;
unsigned int CountCANRot = 0;
unsigned int CountCANTemp = 0;
unsigned int CountSerial = 0;

typedef enum
{
    //Vet_Init = 0,
   Vet_Rot1 = 0,
   Vet_Rot2,
//    Vet_Pedal,
   Vet_MAP,
//    Vet_TmrBob,
    Vet_TmrInj1,
    Vet_TmrInj2,
//    Vet_TempAr,
//    Vet_TempAg,
    Vet_VB1,
    Vet_VB2,
    Vet_Total,
} Vet_Num;
unsigned int valor[Vet_Total]; /* Guarda os dados recebidos via interrupcao do SPI */
/*Prototypes*/
unsigned int Make16(unsigned char data1, unsigned char data2);
void PutRXRotation(unsigned int Rot);
void PutRXTempMotor(unsigned int Temp);
void TESTE_CAN(void);
//PROTOTIPOS SENSOR DE COMPOSIÇÃO DO COMBUSTÍVEL
//VARIAVEIS LEITURA SCC
unsigned int tmr = 0, percent;
unsigned int i = 0; // variavel que sera usada para rotina de teste
float tempo = 0, p_etanol;
bit logica = 0, shit = 0, flag = 0;

/***********Variaveis I2c***********/

unsigned char IgDnt, TmrInj1, TmrInj2, estado_controle_ign, IjDnt, InjTrigger1,
              InjTrigger2, TmrBob, IgTrigger, InjDntExtra, SetVetVel2;
// buffer de 0 a 10 para enviar ao sincronismo

unsigned char ref_vb;       //buffer 11         //Enviar para admissao

unsigned char Rot1, Rot2, Pedal, MAP, TempAr, TempAg, angle1, angle2; //buffer 12 em diante para dados serem mostrados no pc

volatile unsigned char dado_i2c = 0, address = 0, limpa = 0; 
volatile unsigned char buffer[20], index = 0, index_read = 0, buffer_already[20], buffer_ger[3];
volatile unsigned char buffer_read[20];
bit flag_send_sinc = 0, flag_send_ger = 0, flag_send_adm = 0;
bit final_i2c_sincronismo = 0, final_i2c_admissao = 0, final_i2c_gerenciamento = 0;

/***************************************** Interrupts *******************************************/
void interrupt isr(void)//vetor de interrupção
{
    
    if(PIR1bits.SSPIF || PIR1bits.SSP1IF)
    {      
        nLED_TEST = 0;
        SSP1CON1bits.CKP = 0;    //Mantem clock em nivel baixo
        if ((SSP1CON1bits.SSPOV) || (SSP1CON1bits.WCOL)) //Verifica se teve colisao, se verdadeiro, entra no if.
        {
            limpa = SSP1BUF; // limpa buffer
            SSP1CON1bits.SSPOV = 0; // limpa a flag overflow
            SSP1CON1bits.WCOL = 0; // Limpa o bit collision
            SSP1CON1bits.CKP = 1;
        }
        if (SSP1STATbits.R_nW == 0) //Testa se eh escrita, se verdadeiro, entra no if.
        {
            if (SSP1STATbits.D_nA == 0) //verifica se eh endereco
                address = SSP1BUF; // se for address do slave
            else
            {
                dado_i2c = SSP1BUF;
                buffer[index] = SSP1BUF; // se for dado comeca a armazenar no buffer
                index++; // indice de armazenamento do buffer
                
                if(buffer[ENDERECO_MASTER] == ENDERECO_GERENCIAMENTO)
                {
                    flag_send_ger = 1;
                    index_read = 0;
                    flag_send_adm = 0;
                    flag_send_sinc = 0;
                    if(index > 17)
                    {
                        IgDnt = buffer_already[0] = buffer[1];
                        TmrInj1 = buffer_already[1] = buffer[2];                //transformar em 16 bits
                        TmrInj2 = buffer_already[2] = buffer[3];                //transformar em 16 bits
                        estado_controle_ign = buffer_already[3] = buffer[4];
                        IjDnt = buffer_already[4] = buffer[5];
                        InjTrigger1 = buffer_already[5] = buffer[6];            //transformar em 16 bits
                        InjTrigger2 = buffer_already[6] = buffer[7];            //transformar em 16 bits
                        TmrBob = buffer_already[7] = buffer[8]; 
                        IgTrigger = buffer_already[8] = buffer[9];
                        // Vet_AddressRPM,
                        InjDntExtra = buffer_already[9] = buffer[10];
                        TempAg = buffer_already[10] = buffer[11];

                        // dados para o admissão
                        ref_vb = buffer_already[11] = buffer[12];

                        // alguns dados para o software de pc

                        Rot1 = buffer_already[12] = buffer[13];              //transformar em 16 bits
                        Rot2 = buffer_already[13] = buffer[14];              //transformar em 16 bits
                        map = buffer_already[14] = buffer[15];
                        angle1 = buffer_ger[0] = buffer[16];             //transformar em 16 bits
                        angle2 = buffer_ger[1] = buffer[17];             //transformar em 16 bits
                        index = 0;
                    }
                        
                }
                if(buffer[ENDERECO_MASTER] == ENDERECO_SINCRONISMO)
                {
                    flag_send_sinc = 1;
                    flag_send_adm = 0;
                    flag_send_ger = 0;
                    index_read = 0;
                    index = 0;
                }      
            }
            SSP1CON1bits.CKP = 1; // clock eh colocado em nivel alto
        }
        if(SSP1STATbits.R_nW)   //verifica se eh endereco e se eh leitura
        {  
            if(flag_send_ger == 1)
            {
                index = 0;
                SSP1BUF = buffer_ger[index_read];
                index_read++;
                if(index_read > 1)  //Mestre esta lendo dado 16 e 17 (angle1 e angle2)
                    index_read = 0;
            }
            if(flag_send_sinc == 1)
            {
                index = 0;
                SSP1BUF = buffer_already[index_read];
                index_read++;
                if(index_read > 10)  //Mestre esta lendo 11 dados
                {
                    index_read = 0;
                }   
            }
        }
        PIR1bits.SSP1IF = 0; // limpa flag de interrupcao
        SSP1CON1bits.CKP = 1; // clock eh colocado em nivel alto
    }

    if(RCIF == 1) //Teste se houve interrupção pelo RX (UART))
    {
        RX_COM1 = 0; //desabilita interrupçao pelo RX Serial
        RCIF = 0; // Zera flag de interrupçao pelo RX
        
        if (RCSTA1bits.OERR == 1 || RCSTA1bits.FERR == 1)           //garante que nao houve erro, se houver limpa o erro
        {
            RCSTA1bits.OERR = 0; // clear overrun if it occurs
            RCSTA1bits.CREN = 0;
            RCSTA1bits.CREN = 1;
        }
        
        char_serial_ant=char_serial;
        char_serial = RCREG1;       // manda o valor recebido pelo RX para a variavel char_serial
        flag_serial ++;

        
        RX_COM1 = 1; //habilita interrupção pelo pino RX
        
    }
//    __EXT_INTERRUPT2__{
//        // interrupção externa RB2, para detectar falha TIMER 1 está pegando a borda de descida( verificar no polo qual borda é)
//        Disable_ExtInt(nExt_Int2); //  disabilita a interrupção externa
//        Clear_ExtInt_Flag(nFLAG_ExtInt2); //  limpa flag de interrupção
//        if (cont_SCC < 1)
//        {
//            TMR0 = 0;
//            cont_SCC++;
//        }
//        else
//        {
//            SCC = Get_Timer0();
//            TMR0 = 0;
//        }
//        Enable_ExtInt(nExt_Int2);
//    }
    //        __Interrupt_TMR1__
    //        {
    //            Disable_Interrupt(nINT_TIMER1);
    //            ClearFlag_Interrupt(nFLAG_TMR1);
    //            Set_Timer1(_10msTmr);
    //
    //            if(Select_Data >= Serial_DataMax)
    //            {
    //                Select_Data = Serial_Init;
    //            }
    //            else  Select_Data++;
    //
    //            //Enable_Interrupt(nINT_TIMER1);
    //            //Set_Timer1(_10msTmr);
    //            //UpdateRot = (unsigned int)rotacao;
    //            //PutRXRotation(UpdateRot);
    //
    //            //nLED_TEST = !nLED_TEST;
    //            Enable_Interrupt(nINT_TIMER1);
    //        }

    __Interrupt_TMR3__{
        ClearFlag_Interrupt(nFLAG_TMR3);
        Set_Timer3(_1msTmr);
        //nLED_TEST = !nLED_TEST;
        CountCANRot++;
        CountCANTemp++;
        CountSerial++;

        if (CountSerial >= 15)
        {
            Select_Data++;
            CountSerial = 0;
            //nLED_TEST = !nLED_TEST;
        }
    }
}

unsigned int Make16(unsigned char data1, unsigned char data2)
{
    return ((data1 << 8) | data2);
}

unsigned int Putc_TX1_Data(unsigned int data)
{
    unsigned char vet[5];

    vet[0] = (data / 10000) + 0x30;
    data = data % 10000;
    vet[1] = (data / 1000) + 0x30;
    data = data % 1000;
    vet[2] = (data / 100) + 0x30;
    data = data % 100;
    vet[3] = (data / 10) + 0x30;
    vet[4] = (data % 10) + 0x30;

    Putc_TX1(vet[0]);
    Putc_TX1(' ');
    Putc_TX1(vet[1]);
    Putc_TX1(' ');
    Putc_TX1(vet[2]);
    Putc_TX1(' ');
    Putc_TX1(vet[3]);
    Putc_TX1(' ');
    Putc_TX1(vet[4]);
    Putc_TX1(' ');
    Putc_TX1(')');
    Putc_TX1(' ');
    Putc_TX1('\r');
    Putc_TX1(' ');
    Putc_TX1('\n');

}

void No_Analogs(void)
{
    ANSELA = 0x00;
    ANSELB = 0x00;
    ANSELC = 0x00;
    ANSELD = 0x00;
    ANSELE = 0x00;
}

void ConfigureHW(void)
{
    No_Analogs();
    Pin_RX1 = Pin_IN; //configura portB  B1 (pino RX) como entrada
    Pin_TX1 = Pin_OUT;
    TRISB7 = 0;
    TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;
    TRISAbits.TRISA4 = 0;
    PORTB = 0b00000010; // limpar as portas que estão configuradas como saidas
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB5 = 0;
    TRISC1 = 0;
    TRISC0 = 0;
    TRISC2 = 0;
    TRISE = 0xFF;

    Init1_RS232(38400, 1);
//    SPI_SLAVE1();
//    SPI_MASTER2();
    Config_Timer1();
    Config_Timer3();
//    RESET_CAN();
//    __delay_ms(1);
//    CONFIG_MCP2515();
//    __delay_ms(1);
}

short Update_CurrentValue(unsigned int Old, unsigned int Current)
{
    short boReturn = nOFF;
    boReturn = (Current != Old);
    return boReturn;
}

void Update_OldValue(unsigned int index, unsigned int NewValue)
{
    Old_Value[index] = NewValue;
}

void TESTE_CAN(void)
{
    //nLED_TEST = !nLED_TEST;
    CAN_SPI2_WRITE(0x31, 0x46); //ID = 233h      0100 0110
    CAN_SPI2_WRITE(0x32, 0x60); //ID = 233h      0110 0000
    CAN_SPI2_WRITE(0x35, 0x01); //DLC = 1 byte
    CAN_SPI2_WRITE(0x36, 0x01); //Envia msg 0x01 pela rede CAN
    CAN_SPI2_WRITE(0x30, 0x0B); //Máxima prioridade, força envio da msg
}

void PutRXRotation(unsigned int Rot)
{
    //Rot = 1000;//rotacao = 1000;
    rotacao_4 = Rot << 2;

    CAN_SPI2_WRITE(0x31, 0x50); //ID = 280h
    CAN_SPI2_WRITE(0x32, 0x00); //ID = 280h
    CAN_SPI2_WRITE(0x35, 0x08); //DLC = 8 byte

    CAN_SPI2_WRITE(0x36, 0xA9); //Envia msg 0x01 pela rede CAN
    CAN_SPI2_WRITE(0x37, 0xA4); //Envia msg 0x01 pela rede CAN
    CAN_SPI2_WRITE(0x38, (unsigned char) rotacao_4); //Envia msg 0x01 pela rede CAN
    CAN_SPI2_WRITE(0x39, (unsigned char) (rotacao_4 >> 8)); //Envia msg 0x01 pela rede CAN
    CAN_SPI2_WRITE(0x3A, 0x38); //Envia msg 0x01 pela rede CAN
    CAN_SPI2_WRITE(0x3B, 0x00); //Envia msg 0x01 pela rede CAN
    CAN_SPI2_WRITE(0x3C, 0x32); //Envia msg 0x01 pela rede CAN
    CAN_SPI2_WRITE(0x3D, 0x38); //Envia msg 0x01 pela rede CAN
    CAN_SPI2_WRITE(0x30, 0x08); //Máxima prioridade, força envio da msg
    CAN_SPI2_WRITE(0x30, 0x00); //Máxima prioridade, força envio da msg
}

void PutRXTempMotor(unsigned int Temp)
{
    //Temp += 12;
    TempMotor = (unsigned char) ((Temp * 4) / 3 + 48);
    //TempMotor = ((unsigned char)(Temp + 48) / 0.75);

    CAN_SPI2_WRITE(0x31, 0x51); //ID = 288h
    CAN_SPI2_WRITE(0x32, 0x00); //ID = 288h
    CAN_SPI2_WRITE(0x35, 0x08); //DLC = 8 byte

    CAN_SPI2_WRITE(0x36, 0x5D); //Envia msg 0x01 pela rede CAN
    CAN_SPI2_WRITE(0x37, TempMotor); //Envia msg 0x01 pela rede CAN
    CAN_SPI2_WRITE(0x38, 0x10); //Envia msg 0x01 pela rede CAN
    CAN_SPI2_WRITE(0x39, 0x00); //Envia msg 0x01 pela rede CAN
    CAN_SPI2_WRITE(0x3A, 0xFF); //Envia msg 0x01 pela rede CAN
    CAN_SPI2_WRITE(0x3B, 0x64); //Envia msg 0x01 pela rede CAN
    CAN_SPI2_WRITE(0x3C, 0x67); //Envia msg 0x01 pela rede CAN
    CAN_SPI2_WRITE(0x3D, 0x9C); //Envia msg 0x01 pela rede CAN
    CAN_SPI2_WRITE(0x30, 0x08); //Máxima prioridade, força envio da msg
    CAN_SPI2_WRITE(0x30, 0x00); //Máxima prioridade, força envio da msg
}

unsigned int VarTest = 0;

void main(void)
{
    ConfigureHW();
    __delay_ms(10);

    Enable_Interrupt(nINTERRUPT_GLOBAL);
    Enable_Interrupt(nINTERRUPT_PERIPHERAL);
//    Enable_Interrupt(nSPI1);
    //Enable_Interrupt(nINT_TIMER1);
    Enable_Interrupt(nINT_TIMER3);
    RCONbits.IPEN = 0; // SEM PRIORIDADES DE INTERRUPÇÃO
    //Confi_Int_Ext2();
    Config_Timer0();
    //Set_Timer1(_10msTmr);
    Set_Timer3(_1msTmr);

    nFT_CTS = 0;
    nFT_RTS = 0;
    nLED_TEST = nON;
    nSS_MCP2515 = nPin_HIGH;
    Disable_ExtInt(nExt_Int2);
    

   // hwConfigurarLcd();
    //lcdLimparTela();
    I2C_Slave_Init(ENDERECO_COMUNICACAO); 
    
    while (1)
    {
        
        if(flag_serial == 2)
        {
            if (char_serial_ant == 'R') // Se o caracter recebido anteriormente é 'R', ele entra para aquisitar os dados
            {
                switch(char_serial)     //testa o caracter recebido agora
                {
                     case '1': // "R1" - Leitura das versoes de firmware
                       //  nLED_TEST = 1;

                         Putc_TX1(206);
                         Putc_TX1(125);
                         Putc_TX1(124);
                        // Enable_ExtInt(nExt_Int2);
                         break;
                     case '2': // "R2" - Manda todos os parametros
                        // nLED_TEST = 0;
//                         GIE = 0;
                         rotacao = (Rot1 << 8) | Rot2;
//                         rotacao = 257;
                        // rotacao = 1234;
                         Putc_TX1_16bits(rotacao); // 1
                         Putc_TX1_16bits(513); //2
                         Putc_TX1_16bits(1025); //3  //valvula
                         Putc_TX1(53);  //6 TEMPERATURA DO AR
                         Putc_TX1(54);  //7
                         Putc_TX1(55);  //8
//                        Putc_TX1(53);  //6 TEMPERATURA DO AR
//                         Putc_TX1(54);  //7
//                         Putc_TX1(55);  //8
                         Putc_TX1_16bits(2049);     //12
//                         GIE = 1;
//                         
//                         Putc_TX1(BAT);  //9   //bateria
//                         Putc_TX1_16bits(0);  //10
//                         Putc_TX1(ij_dente_14);           //11
//                 
//                         Putc_TX1(0);    //13
//                         Putc_TX1_16bits(ig_tmr_disparo);     //14
//                         Putc_TX1_16bits(tempo_inj);     //15
//                         Putc_TX1_16bits(tempo_bob);     //16
//                         Putc_TX1(0);    //17
//                         Putc_TX1(0);    //18
//                         Putc_TX1(0);    //19
//                         Putc_TX1(0);    //20
//                         Putc_TX1(0);    //21
//                         Putc_TX1(0);    //22
//                         Putc_TX1(0);    //23
//                         Putc_TX1(0);    //24
//                         Putc_TX1(0);    //25
//                         Putc_TX1(0);    //26
//                         Putc_TX1_16bits(0);     //27
//                         Putc_TX1_16bits(0);     //28
//                         Putc_TX1(0);    //29
//                         Putc_TX1(0);    //30
//                         Putc_TX1_16bits(0);     //31
//                         Putc_TX1_16bits(0);     //32
//                         Putc_TX1(0);    //33
//                         Putc_TX1(0);    //34
//                         Putc_TX1(0);    //35
//                         Putc_TX1(0);    //36
//                         Putc_TX1(0);    //37
//                         Putc_TX1(0);    //38
//                         Putc_TX1_16bits(0);     //39
//                         Putc_TX1(0);    //40


//
//                         Putc_TX1_16bits(rotacao); // 1
//                         Putc_TX1_16bits(0); //2
//                         Putc_TX1_16bits(0); //3  //valvula
//                         Putc_TX1_16bits(0);  //4 // referencia torque
//                         Putc_TX1_16bits(0);  //5
//                         Putc_TX1(pedal_ad);  //6 TEMPERATURA DO AR
//                         Putc_TX1(map);  //7
//                         Putc_TX1(0);  //8
//                         Putc_TX1(0);  //9   //bateria
//                         Putc_TX1_16bits(0);  //10
//                         Putc_TX1(0);           //11
//                         Putc_TX1_16bits(0);     //12
//                         Putc_TX1(0);    //13
//                         Putc_TX1_16bits(0);     //14
//                         Putc_TX1_16bits(0);     //15
//                         Putc_TX1_16bits(0);     //16
//                         Putc_TX1(0);    //17
//                         Putc_TX1(0);    //18
//                         Putc_TX1(0);    //19
//                         Putc_TX1(0);    //20
//                         Putc_TX1(0);    //21
//                         Putc_TX1(0);    //22
//                         Putc_TX1(0);    //23
//                         Putc_TX1(0);    //24
//                         Putc_TX1(0);    //25
//                         Putc_TX1(0);    //26
//                         Putc_TX1_16bits(0);     //27
//                         Putc_TX1_16bits(0);     //28
//                         Putc_TX1(0);    //29
//                         Putc_TX1(0);    //30
//                         Putc_TX1_16bits(0);     //31
//                         Putc_TX1_16bits(0);     //32
//                         Putc_TX1(0);    //33
//                         Putc_TX1(0);    //34
//                         Putc_TX1(0);    //35
//                         Putc_TX1(0);    //36
//                         Putc_TX1(0);    //37
//                         Putc_TX1(0);    //38
//                         Putc_TX1_16bits(0);     //39
//                         Putc_TX1(0);    //40
                       //  Enable_ExtInt(nExt_Int2);
                         break;

                    case '3': // "R3" - Manda apenas os parametros do ensaio de identificacao do cruzeiro

                         Putc_TX1_16bits(rotacao);     //1
                         
                         Putc_TX1_16bits(0);     //2 //ref torque
                         Putc_TX1(0);    //3
                         Putc_TX1(0);    //4
                         Putc_TX1(0);    //5
                         Putc_TX1(0);    //6
                         Putc_TX1_16bits(0);     //7
                        // Enable_ExtInt(nExt_Int2);
                         break;

                    case '4': // "R4" - Manda apenas os parametros do ensaio de identificaco da VB

                        Putc_TX1_16bits(257);     //1
                        Putc_TX1_16bits(513);     //2
                       // Enable_ExtInt(nExt_Int2);

                        break;

                    case '5': // "R5" - Manda apenas os parametros do ensaio de identificaco da Marcha Lenta

                        Putc_TX1_16bits(rotacao);     //1
                        Putc_TX1_16bits(ref_vb);     //2
                        Putc_TX1_16bits(4);     //3
                    //    Enable_ExtInt(nExt_Int2);
                        break;

                }
            }
            else if (char_serial_ant == 'E') // Se o caracter recebido anteriormente é 'E', ele entra para enviar dados
            {

            }
            flag_serial = 0;
        }
        
        if (spi_ctr >= Vet_Total/*SPI_MAX*/)
        {
            spi_ctr = 0;
            rotacao = Make16(valor[Vet_Rot1], valor[Vet_Rot2]);
           // rotacao = valor[Vet_Rot1];
            //pedal_ad = valor[Vet_Rot2];
//            Rot2 = rotacao - 100;
//            pedal_ad = valor[Vet_Pedal];
            map = valor[Vet_MAP];
//            tempo_bob = valor[Vet_TmrBob];
            tempo_inj = Make16(valor[Vet_TmrInj1], valor[Vet_TmrInj2]);
//            t_ar_com = valor[Vet_TempAr];
//            t_agua = valor[Vet_TempAg] + 12;
            ref_vb = Make16(valor[Vet_VB1], valor[Vet_VB2]);
           // Putc_TX1(ref_vb); // alterado
            //              ref_rpm = Make16(valor[12],valor[13]);
            //              ig_tmr_disparo = valor[14];
            //              ij_tmr_disparo = Make16(valor[15],valor[16]);
        }
//        nLED_TEST =0;
        porc_etanol = (float) ((10000 / (SCC * 0.004)) - 50);
        // porc_etanol = 20;
        

    }
    return;
}
/*************************************
 *       END SW COMUNICAÇÃO          *
 ************************************/

/*
 *                                 PIC18F46K22
 *                          +---------:_:---------+
 *              VPP MCLR -> : 01 VPP       PGD 40 : <> RB7 PGD
 *                   RA0 <> : 02           PGC 39 : <> RB6 PGC LCD_RW
 *                   RA1 <> : 03           PGM 38 : <> RB5     LCD_RS
 *                   RA2 <> : 04               37 : <> RB4     LCD_E
 *                   RA3 <> : 05               36 : <> RB3     LCD_D7
 *                   RA4 <> : 06 T0CKI         35 : <> RB2     LCD_D6
 *                   RA5 <> : 07               34 : <> RB1     LCD_D5
 *                   RE0 <> : 08          INT0 33 : <> RB0     LCD_D4
 *                   RE1 <> : 09               32 : <----- VDD
 *                   RE2 <> : 10               31 : <----- VSS
 *              VDD ------> : 11               30 : <> RD7
 *              VSS ------> : 12               29 : <> RD6
 *    10MHZ_CRYSTAL OSC1 -> : 13 OSC1          28 : <> RD5
 *    10MHZ_CRYSTAL OSC2 <- : 14 OSC2          27 : <> RD4
 *                   RC0 <> : 15 T1CKI         26 : <> RC7
 *                   RC1 <> : 16               25 : <> RC6
 *                   RC2 <> : 17               24 : <> RC5
 *                   RC3 <> : 18               23 : <> RC4
 *                   RD0 <> : 19               22 : <> RD3
 *                   RD1 <> : 20               21 : <> RD2
 *                          +---------------------+
 *                                  DIP-40
 */