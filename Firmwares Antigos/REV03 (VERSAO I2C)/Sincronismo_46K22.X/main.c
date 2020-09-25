/*******************************************************************************************************
 * File:   main.c
 * Author: Hiroki
 *
 * Created on 10 de Julho de 2016, 22:38
 * SW SINCRONISMO VERSÃO 1.0
 * Autor: Gustavo Hiroki
 * Modificação: Início declarações
 * 10/07/2016
 ******************************************************************************************************/
/*******************************************************************************************************
 * File:   main.c
 * Author: Hiroki
 *
 * Created on 12 de Julho de 2016, 23:27
 * SW SINCRONISMO VERSÃO 1.0
 * Autor: Gustavo Hiroki
 * Modificação: Teste de interrupção por SPI e implementação
 * 12/07/2016
 ******************************************************************************************************/
/*******************************************************************************************************
 * File:   main.c
 * Author: Hiroki
 *
 * Created on 12 de Julho de 2016, 19:55
 * SW SINCRONISMO VERSÃO 1.0
 * Autor: Gustavo Hiroki
 * Modificação: Configurado e Finalizado
 * 14/07/2016
 ******************************************************************************************************/
/*******************************************************************************************************
 * File:   main.c
 * Author: Hiroki
 *
 * Created on 17 de Setembro de 2016, 14:19
 * SW SINCRONISMO VERSÃO 1.0
 * Autor: Gustavo Hiroki
 * Modificação: Configurado placa MARK1
 * 17/09/2016
 ******************************************************************************************************/

/**************************************** INCLUDE LIBRARY ********************************************/
#include <xc.h>
#include <math.h>
#include "Config_XC8.h"
#include "ADC_K22.h"
#include "EXT_INT.h"
#include "SPI_XC8.h"
#include "TIMERS_XC8.h"
#include "ConfigI2c.h"

/******************************************* DEFINES ************************************************/
/* Definicao dos pinos dos sinais de entrada e saida */
#define sinal_ent       LATBbits.LATB0         /* Sinal da Roda Fonica */
#define nROTATION_CCP3  PORTBbits.RB5

//#define sinal_inj1      LATDbits.LATD0        /* Injecao 1  DIN0 */
#define sinal_inj1      LATDbits.LATD4 
#define sinal_inj2      LATDbits.LATD2        /* Injecao 2  DIN1 */
#define sinal_inj3      LATDbits.LATD3        /* Injecao 3  DIN2 */ //Invertido D2 e D3 na placa
//#define sinal_inj4      LATDbits.LATD1        /* Injecao 4  DIN3 */
#define sinal_inj4      LATDbits.LATD5

//#define sinal_ig14      LATDbits.LATD4        /* Ignicao (do "fim") dos cilindros 1 e 4 */
//#define sinal_ig23      LATDbits.LATD5        /* Ignicao (do "meio") dos cilindros 2 e 3 - calculada */

#define sinal_falha     LATBbits.LATB7        /* Sinal de Falha */

#define LED_INJ_1       LATDbits.LATD2
#define LED_INJ_2       LATDbits.LATD3
//#define LED_INJ_3       LATDbits.LATD0
//#define LED_INJ_4       LATDbits.LATD1
#define LED_INJ_3       LATDbits.LATD4
#define LED_INJ_4       LATDbits.LATD5

//#define LED_IGN_1       LATDbits.LATD4
//#define LED_IGN_2       LATDbits.LATD5
#define LED_IGN_3       LATDbits.LATD6
#define LED_IGN_4       LATDbits.LATD7

#define OUTEN_33810     LATEbits.LATE0

#define nBOB_IGN1       LATCbits.LATC0      // leds da bobina de ignição,vão para o conector para acionamento do driver que se encontra no carro
#define nBOB_IGN2       LATCbits.LATC1      // leds da bobina de ignição , vão para o conector para acionamento do driver que se encontra no carro

#define L15             LATBbits.LATB1
#define PORT_L15        PORTBbits.RB1

#define I2c_MAX         12            /* Numero de bytes recebidos do Gererenciamento pelo SPI */
#define PORT_fase       PORTEbits.RE2

#define nVEL_VENT1      LATBbits.LATB3      //Dig Out Ventilador 1
#define nVEL_VENT2      LATBbits.LATB4      //Dig Out Ventilador 2
#define nCOLD_CRANK     LATBbits.LATB2      //Dig Out Partida a frio
#define nAIR_COND       LATCbits.LATC2      //Dig Out Ar condicionado

#define nMAXI_33810     LATCbits.LATC6      //Máximo valor de corrente
#define nNOMI_33810     LATCbits.LATC7      //Nominal valor de corrente

#define nBOB_PENCOIL1   LATAbits.LATA0
#define nBOB_PENCOIL2   LATAbits.LATA1
#define nBOB_PENCOIL3   LATAbits.LATA2
#define nBOB_PENCOIL4   LATAbits.LATA3

#define nPIN_TEST       LATBbits.LATB6
#define LED  LATAbits.LATA2
// EQUIPE FATEC SOROCABA SITUAÇÃO DE MARCHA LENTA
#define   n1000_RPM              50
#define   n900_RPM               100
#define   n800_RPM               150
#define   Desabilitado            0

#define RPM_cutoff              1800

#define nBOMB_COLDA_CRANK   LATAbits.LATA4
#define nPIN_LED3_TEST  LATAbits.LATA2
/*****************************************************************************************************/
#define     nPin_HIGH            1
#define     nPin_LOW             0
#define     nYes                 1
#define     nNo                  0
#define     nON                  1
#define     nOFF                 0

#define     nDigital_Pin         0
#define     nAnalog_Pin          1

/********************************** Variaveis globais *************************************************/
short int a, b, c, d, e; /* Variaveis de status do programa */
short int dm;
int dente; /* Incrementado a cada dente */
int ig_pos; // Variavel de controle da ignicao.
// 0-sem ignicao
// 1-sinal de ignicao ativo(2ms)
// 23-inicio da ignicao do "meio" (cil. 2 e 3)
// 14-inicio da ignicao do "fim" (cil. 1 e 4)

int ij14_pos, ij23_pos; // Variavel de controle da injecao.
// 0-sem injecao
// 1-sinal de injecao cilindro 1
// 2-sinal de injecao cilindro 2
// 3-sinal de injecao cilindro 3
// 4-sinal de injecao cilindro 4
// 5-sinal de injecao ativo (contador disparado p/ baixar o sinal)

long long periodo_peralta, t_periodo, t_periodo_ref; /* Variaveis para armazenar os tempos
                                    de patamar em alto */

int volta14, volta23; /* Booleana - Utilizada na injecao */
int estado_controle_ml = 0; // variável que armazena o valor de estado do controle da marcha lenta recebido da SPI

/* Variaveis que controlam a ignicao no "meio" e no "fim" */
int ig_tmr_disparo, ig_tmr_disparo_controle; /* Armazena o tempo de disparo do timer3 a partir
                          do dente de referencia (igual para as duas ignicoes */
int ig_dente_23, ig_dente_23_ant; /* Armazena o dente de referencia para a ignicao 23 */
int ig_dente_14, ig_dente_14_ant; /* Armazena o dente de referencia para a ignicao 14 */

long ij_tmr_disparo; /* Armazena o tempo de disparo para injecao a partir
                        do dente de referencia */
float Erro_RPM = 0, Erro_RPM_ant = 0, u = 0, teto_avanco = 21, u_ant = 0, SP_rpm = 1000, ig_tmr_disparo_controle_temp;
float K = 0.24546, z = 0.8921, dente_controle = 0, kp_inj = 1.0; // ganho  e zero do controlador de avanço de ignição 0.8921
float dente_subida = 0;
int dente_subida_inteiro = 0, rotacao_peralta = 0, n_controle = 0;
char flag_peralta = 1;

int ij_dente_14, ij_dente_14_ant;
int ij_dente_23, ij_dente_23_ant;

long int tempo_inj; /* recebe do Ger o tempo de injecao*/
int tempo_bob; /* tempo de carregamento da bobina de ign */
int valor[I2c_MAX]; /* Guarda os dados recebidos do regrenciamento via interrupcao do SPI */
int spi_ctr = 0; /* Indice de controle do vetor "valor[]" */
int rotacao; // Recebe a rotacao do GER (utilizada na deteccao de fase)
int rotacao_ant;
short fase_ok; // Indica que ja identificou a fase pelo menos uma vez no inicio
short falha_ok;
short sinal_fase;
short cutoff = 0;
int ij_dente_14_extra;
int ij_dente_23_extra;
bit Enable_control_ign = 0; // chave de controle para habilitar a amostragem
bit Chave_controle_geral = 0; // chave para habilitar o controle

unsigned int Set_Vent;
unsigned int pedal_ad = 0; // variavel para armazenar o valor do pedal que será lido no main.
unsigned char voltai2c = 0;
short Flag_I2c = 0;

int flag_marcus = 0;

void Control_ml_ign(void);

typedef enum
{
    Vet_Init = 0,
    Vet_IgDnt = Vet_Init,
    Vet_TmrInj1,
    Vet_TmrInj2,
    Vet_estado_controle_ign,
    Vet_IjDnt,
    Vet_InjTrigger1,
    Vet_InjTrigger2, //6
    Vet_TmrBob, //
    Vet_IgTrigger,
    // Vet_AddressRPM,
    Vet_InjDntExtra,
    Vet_SetVetVel2,

    Vet_Total
} Vet_Num;

typedef enum
{
    enReleInit = 0,
    enRele_1 = enReleInit,
    enRele_2,
    enRele_3,
    enRele_4,
    enRele_5,
    enRele_6,
    enRele_7,
    enRele_8,
    enReleMax
} EnumReles;

static union
{
    unsigned int u8All;

    struct
    {
        unsigned Rele1 : 1;
        unsigned Rele2 : 1;
        unsigned Rele3 : 1;
        unsigned Rele4 : 1;
        unsigned Rele5 : 1;
        unsigned Rele6 : 1;
        unsigned Rele7 : 1;
        unsigned Rele8 : 1;
    } StatusAllReles;
} UnionAllReles;

/*ex: SetBit(UnionAllReles.u8aAll, enRele_1)*/
#define SetBit(value,Bit)		((value) |= (1 << (Bit)))

/*ex: GetBit(UnionAllReles.u8aAll, enRele_1)*/
#define GetBit(value,Bit)		((value) >> (Bit) & 0x01)

/*ex: ClearBit(&UnionAllReles.u8aAll, enRele_1)*/
void ClearBit(unsigned char *value, unsigned char Bit)
{
    unsigned char aux = (1 << (Bit));
    *value = *value & (unsigned char) (~aux);
}

/*Prototype*/
void conta_dente();

unsigned int Make_16(unsigned char data1, unsigned char data2)
{
    return ((data1 << 8) | data2);
}

/***************************************** Interrupts *******************************************/
void interrupt high_priority isr2(void)
{

    __Interrupt_TMR0__{
        Disable_Interrupt(nINT_TIMER0);
        ClearFlag_Interrupt(nFLAG_TMR0);
        
        /* Pulsos de injecao p sequencial*/
        switch (ij14_pos)
        {

        case 0:
            break;

        case 1:
            
            if (tempo_inj > 20)
            {

                //sinal_inj1 = nPin_HIGH; /* Ativa Sinal da injecao 1 */
                LED_INJ_1 = nPin_HIGH; //nPIN_TEST = 1;

                /* Conta e entao baixa o sinal de injecao por interrupcao */
                Set_Timer0(tempo_inj);
                ClearFlag_Interrupt(nFLAG_TMR0);
                Enable_Interrupt(nINT_TIMER0);

                ij14_pos = 10; // Proxima interrupcao vai desligar o sinal
            }
            else
                ij14_pos = 0;
            break;

        case 4:
            if (tempo_inj > 20)
            {

                //sinal_inj4 = nPin_HIGH; /* Ativa Sinal da injecao 4 */
                LED_INJ_4 = nPin_HIGH;

                /* Conta e entao baixa o sinal de injecao por interrupcao */
                Set_Timer0(tempo_inj);
                ClearFlag_Interrupt(nFLAG_TMR0);
                Enable_Interrupt(nINT_TIMER0);

                ij14_pos = 40; // Proxima interrupcao vai desligar o sinal
            }
            else
                ij14_pos = 0;
            break;

        case 10:
            //sinal_inj1 = nPin_LOW; /* Baixa o Sinal da Injecao Ativa */
            LED_INJ_1 = nPin_LOW; //nPIN_TEST = 0;

            if ((rotacao >= 6) && (rotacao_ant >= 6))
            {
                volta14 = !volta14;

                // Verifica a logica da fase...
                // Neste caso o sinal de injecao 1 SEMPRE desce DEPOIS da falha
                if (sinal_fase == 0) // a fase estava errada!!!
                    volta14 = !volta14;
            }

            ij14_pos = 0;
            break;

        case 40:
            //sinal_inj4 = nPin_LOW;
            LED_INJ_4 = nPin_LOW;

            if ((rotacao >= 6) && (rotacao_ant >= 6))
                volta14 = !volta14;

            ij14_pos = 0;
            break;

        }
        b = 1;

    }
    __Interrupt_TMR5__{
        PIR5bits.TMR5IF = 0; // DESLIGA O FLAG DE INTERRUPÇÃO

        // nPIN_TEST = ~nPIN_TEST;
        Enable_control_ign = 1; // habilita o controle de ignição
        Set_Timer5(15535); // carrega o timer para estourar com 20milissegundos
//        nPIN_LED3_TEST = !nPIN_LED3_TEST;
    }
//    __Interurpt_SPI1__ /* Recebe os avancos e a rotacao do Gerenciamento */{
//        Disable_IntSPI(nSPI1);
//        Clear_IntFlag_SPI(nSPI1_Flag);
//        Clear_IntFlag_SPI(SSP1CON1bits.SSPOV);
//        
//        valor[spi_ctr] = SSP1BUF;
//        spi_ctr++;
//
//        if (spi_ctr < SPI_MAX)
//        {
//            //Clear_IntFlag_SPI(nSPI1_Flag); COMENTEI
//            Enable_IntSPI(nSPI1);
//            Flag_Spi = 1;
//        }
//    }

    __EXT_INTERRUPT0__{
        Disable_ExtInt(nExt_Int0);
        Clear_ExtInt_Flag(nFLAG_ExtInt0);

        //LATCbits.LATC2 = !LATCbits.LATC2;
        
        if (dente != 255) // só entra aqui depois da segunda interrupção externa, garante que o período é amostrado só depois do segundo pulsto
        {
            OUTEN_33810 = nPin_LOW; //Habilita MC33810 na placa
            Disable_Interrupt(nINT_TIMER1);

            t_periodo = Get_Timer1();


        }

        if ((t_periodo > t_periodo_ref) && (dente > 50))
        {

            /*------------------ Nova Volta -------------------------- */
            sinal_falha = nPin_HIGH; /* Sinal de Falha  mostrado no hardware*/
            falha_ok = 1; // sinaliza a flag de detecção de falha
            dente = 0; /* Reinicia contagem dos dentes */

            // Prepara a interrupcao SPI para a proxima volta
            spi_ctr = 0; // zera o index que carrega no vetor do spi
//            Clear_IntFlag_SPI(nSPI1_Flag);
            //Enable_IntSPI(nSPI1);

            a = b = c = d = e = 0;
            /*------------------ Fim Nova Volta ----------------------- */
        }
        //PERALTA

        //        if (dente == 3) // calcula a rotação caso não esteja na falha e o timer não tenha estourado
        //        {
        //            rotacao_peralta = 1 / (periodo_peralta * 0.0000004); // converte o valor do timer em rotação
        //        }


        switch (estado_controle_ml)
        {
            //                case 0:
            //                    Chave_controle_geral = 0; // se a rotação cai abaixo de 500 ele desliga o controle
            //                    Disable_Interrupt(nINT_TIMER5); // e também desliga a interrupção de timer5
            //                    break;


        case n1000_RPM:
            SP_rpm = 1150;
            //            if (Set_Vent > 50)
            //            {
            //                SP_rpm = 800;
            //            }
            //            if (Chave_controle_geral == 0)
            //            {
            //                Chave_controle_geral = 1;
            //                T5CONbits.TMR5ON = 1; // inicia a contagem do timer5
            //                Set_Timer5(15535); // carrega TIMER5 para estourar com 20 milissegundos
            //                Enable_Interrupt(nINT_TIMER5);
            //            }
            break;


        case n900_RPM:
            SP_rpm = 900;
            //            if (Set_Vent > 50)
            //            {
            //                SP_rpm = 800;
            //            }
            //            if (Chave_controle_geral == 0)
            //            {
            //                Chave_controle_geral = 1;
            //                T5CONbits.TMR5ON = 1; // inicia a contagem do timer5
            //                Set_Timer5(15535); // carrega TIMER5 para estourar com 20 milissegundos
            //                Enable_Interrupt(nINT_TIMER5);
            //            }
            break;

        case n800_RPM:
            SP_rpm = 800; // era 800
            //            if (Set_Vent > 50)
            //            {
            //                SP_rpm = 800;
            //            }
            //            if (Chave_controle_geral == 0)
            //            {
            //                Chave_controle_geral = 1;
            //                T5CONbits.TMR5ON = 1; // inicia a contagem do timer5
            //                Set_Timer5(15535); // carrega TIMER5 para estourar com 20 milissegundos
            //                Enable_Interrupt(nINT_TIMER5);
            //            }
            //            break;

        default: SP_rpm = 900;
            break;



        }
        //Lógica do switch do set point de marcha lenta comentada para teste, set point fixo em  1000 rpm

        //        if (estado_controle_ml == n900_RPM)
        //        {
        //
        //            SP_rpm = 900;
        //            if (Chave_controle_geral == 0) // ele só faz esse teste pra carregar o timer uma vez , depois as outras vezes ele carrega o timer só no estouro
        //            {
        //
        //                T5CONbits.TMR5ON = 1; // inicia a contagem do timer5
        //                Set_Timer5(15535); // carrega TIMER5 para estourar com 20 milissegundos
        //                Enable_Interrupt(nINT_TIMER5);
        //                Chave_controle_geral = 1;
        //
        //
        //            }
        //
        //        }
        //        else
        //        {
        //            Chave_controle_geral = 0; // se a rotação cai abaixo de 500 ele desliga o controle
        //            Disable_Interrupt(nINT_TIMER5); // e também desliga a interrupção de timer5
        //        }

        t_periodo = t_periodo << 1;
        t_periodo_ref = t_periodo;

        //if(input(L15))
        
        conta_dente();
        
        // PERALTA
        //if (dente != 2)
        //{
        //Set_Timer1(0);
        //}
        // PERALTA

        // Inicia medicao do t_low
        ClearFlag_Interrupt(nFLAG_TMR1);
        Enable_Interrupt(nINT_TIMER1);

        if (dente == 2) sinal_falha = nPin_LOW; /* Baixa o Pulso de Falha */

        e = 1;
        Clear_ExtInt_Flag(nFLAG_ExtInt0);
        Enable_Interrupt(nExt_Int0);
    }

    __Interrupt_TMR2__{
        Disable_Interrupt(nINT_TIMER2);
        ClearFlag_Interrupt(nFLAG_TMR2); /*ALTERADO DIA 30/10*/

        //LATCbits.LATC2 = !LATCbits.LATC2;

        if (dm == 0) //Isto para dobrar o funcionamento do timer senão falha ignicao
        {
            // Pulsos de ignicao
            switch (ig_pos)
            {

            case 0: break;

            case 1:
                //sinal_ig23 = nPin_LOW; // Baixa o Sinal da Ignicao Ativa
//                LED_IGN_2 = nPin_LOW;
                nBOB_IGN2 = nPin_LOW;
                //sinal_ig14 = nPin_LOW;
//                LED_IGN_1 = nPin_LOW;
                nBOB_IGN1 = nPin_LOW;
                ig_pos = 0;
                dm = 0;
                break;

            case 23:
                if (tempo_bob > 1)
                {
                    //sinal_ig23 = nPin_HIGH; // Ativa Sinal da ignicao 23
                    //LED_IGN_2 = nPin_HIGH;
                    nBOB_IGN2 = nPin_HIGH;

                    //Config_Timer2();                    //setup_timer_2(T2_DIV_BY_16,ig_tmr_disparo,16);
                    PR2 = tempo_bob;
                    Set_Timer2(0);
                    ClearFlag_Interrupt(nFLAG_TMR2);
                    Enable_Interrupt(nINT_TIMER2);
                    ig_pos = 1;
                    dm = 1;
                }
                else ig_pos = 0;
                break;

            case 14:
                if (tempo_bob > 1)
                {
                    //sinal_ig14 = nPin_HIGH; // Ativa Sinal da ignicao 14
                    //LED_IGN_1 = nPin_HIGH;
                    nBOB_IGN1 = nPin_HIGH;

                    //Config_Timer2();                    //setup_timer_2(T2_DIV_BY_16,ig_tmr_disparo,16);
                    PR2 = tempo_bob;
                    Set_Timer2(0);
                    ClearFlag_Interrupt(nFLAG_TMR2);
                    Enable_Interrupt(nINT_TIMER2);
                    ig_pos = 1;
                    dm = 1;
                }
                else ig_pos = 0;
                break;
            }
            c = 1;
        }

        else
        {
            Set_Timer2(0);
            dm = 0;
            ig_pos = 1;
            ClearFlag_Interrupt(nFLAG_TMR2);
            Enable_Interrupt(nINT_TIMER2);
        }
    }

    __Interrupt_TMR3__{
        ClearFlag_Interrupt(nFLAG_TMR3);
        Disable_Interrupt(nINT_TIMER3);

        /* Pulsos de injecao p sequencial*/
        switch (ij23_pos)
        {

        case 0:
            break;

        case 2:
            if (tempo_inj > 20)
            {
                //sinal_inj2 = nPin_HIGH; /* Ativa Sinal da injecao 2 */
                LED_INJ_2 = nPin_HIGH;

                /* Conta e entao baixa o sinal de injecao por interrupcao */
                Set_Timer3(tempo_inj);
                ClearFlag_Interrupt(nFLAG_TMR3);
                Enable_Interrupt(nINT_TIMER3);

                ij23_pos = 20; // Proxima interrupcao vai desligar o sinal
            }
            else
                ij23_pos = 0;
            break;

        case 3:
            if (tempo_inj > 20)
            {
                //sinal_inj3 = nPin_HIGH; /* Ativa Sinal da injecao 3 */
                LED_INJ_3 = nPin_HIGH;

                /* Conta e entao baixa o sinal de injecao por interrupcao */
                Set_Timer3(tempo_inj);
                ClearFlag_Interrupt(nFLAG_TMR3);
                Enable_Interrupt(nINT_TIMER3);

                ij23_pos = 30; // Proxima interrupcao vai desligar o sinal
            }
            else ij23_pos = 0;
            break;

        case 20:
            //sinal_inj2 = nPin_LOW;
            LED_INJ_2 = nPin_LOW;

            if ((rotacao >= 6) && (rotacao_ant >= 6))
                volta23 = !volta23;

            ij23_pos = 0;
            break;

        case 30:
            //sinal_inj3 = nPin_LOW; /* Baixa o Sinal da Injecao Ativa */
            LED_INJ_3 = nPin_LOW;

            if ((rotacao >= 6) && (rotacao_ant >= 6))
            {
                volta23 = !volta23;

                // Verifica a logica da fase...
                // Neste caso o sinal de injecao 3 SEMPRE desce DEPOIS da falha
                if (sinal_fase == 0) // a fase estava errada!!!
                    volta23 = !volta23;
            }

            ij23_pos = 0;
            break;

        }
        d = 1;
    }


}

void interrupt low_priority isr(void)
{
    __Interrupt_TMR1__{
        Disable_Interrupt(nINT_TIMER1);
        ClearFlag_Interrupt(nFLAG_TMR1); /*ALTERADO DIA 30/10*/
//         nPIN_TEST = ~nPIN_TEST;
        
        rotacao = 0;
        fase_ok = 0;
        falha_ok = 0;
        t_periodo_ref = 65535; // Reseta deteccao da falha
        t_periodo = 65530;
        dente = 255;

        nVEL_VENT2 = nPin_LOW;

        Set_Timer1(0);
        flag_peralta = 1; // indica que o timer1 estourou para não executar a conta da rotação_peralta no estouro de interrupção externa
        Enable_Interrupt(nINT_TIMER1);
        //if(!PORT_L15)     nVEL_VENT2 = nPin_LOW;

    }
}

/* Funcao conta_dente */
void conta_dente(void)
{
    dente++; // Contagem das dentes
    if (dente == 3)
    {
        periodo_peralta = Get_Timer1(); // pega o período de um dente
        rotacao_peralta = 1 / (periodo_peralta * 0.0000004);
    }
    Set_Timer1(0);

    // Se nao ident. a falha uma vez nao deixa prosseguir...
    if (falha_ok == 0) return;


    //    if (dente == 2)
    //    {
    //        Set_Timer1(0);
    //    }

    if (dente == 3) // testa o sinal de fase
    {
        // periodo_peralta = Get_Timer1(); // pega o período de um dente
        if(voltai2c == 0)
            voltai2c = 1;
        else if(voltai2c == 1)
            voltai2c = 2;
        else if(voltai2c == 2)
            voltai2c = 3;
        else if(voltai2c == 3)
        {
            voltai2c = 2;
                    LED = 1;
        }
        // Armazena o sinal de fase
        if (!PORT_fase) // INVERTEMOS O TESTE DO PORT QUE RECEBE A FASE PARA INVERTER A LÓGICA INTEIRA DO SW, PORQUE O NOSSO SENSOR FUNCIONA AO CONTRARIO
            sinal_fase = 1; // sinaliza a flag de fase, se 1 é inj no cil 1 e ig no 4 (Polo Sorocaba)
        else sinal_fase = 0; // Sinaliza a flag de fase, se 0 inj no cil 4 e ign no 1 (Polo Sorocaba)
    }
    if (dente == 30)
    {
        // Guarda os adiantamentos da volta anterior antes de atualizar....
        ig_dente_14_ant = ig_dente_14;
        ig_dente_23_ant = ig_dente_23;
        ij_dente_14_ant = ij_dente_14;
        ij_dente_23_ant = ij_dente_23;
        rotacao_ant = rotacao;

        // Atualiza adiantamentos e tempos
        ig_dente_14 = 25; //teste



        // PERALTA
        if (Chave_controle_geral == 1) // se a chave geral de controle de ignição marcha lenta estiver habilitada
        { // ele pega o valor do dente calculado pelo meu controle
            ig_dente_14 = dente_subida_inteiro;

        }
        else // senão pega o valor recebido do gerenciamento
        {
            ig_dente_14 = buffer_read[0]; //valor[Vet_IgDnt];

        }


        if ((ig_dente_14 <= 1) || (ig_dente_14 >= 58))
        { // Verificacao de seguranca...
            //            ig_dente_14 = 255;
            //            ig_dente_23 = 255;
            //            ig_dente_14 = 2;
            //            ig_dente_23 = 32;
            ig_dente_14 = 1;
            ig_dente_23 = 31;
        }
        else
        {
            ig_dente_23 = ig_dente_14 + 30;
            if (ig_dente_23 > 60) ig_dente_23 -= 60;
        }

        //ij_dente_14 = (unsigned char)(valor[0] >> 8); // O dente de inj foi enviado no byte mais sig. do SPI (SPIxDRH)
        if (cutoff == 1)
        {
            ij_dente_14 = 255;
            ij_dente_23 = 255;
        }
        else
        {
            ij_dente_14 = buffer_read[4];//valor[Vet_IjDnt]; // O dente de inj foi enviado no byte mais sig. do SPI (SPIxDRH)
        }

        if (((ij_dente_14 <= 1) || (ij_dente_14 >= 58)) && cutoff == 0)
        { // Verificacao de seguranca...
            //            //            if (ij_dente_14 == 200) // indica corte de injecao...
            //            //                cutoff = 1;
            //            //            else cutoff = 0;
            //
            //            //            ij_dente_14 = 255;
            //            //            ij_dente_23 = 255;

            ij_dente_14 = 1;
            ij_dente_23 = 31;
            //nPIN_TEST = 1;
        }
        else
            if (cutoff == 0)
        {
            ij_dente_23 = ij_dente_14 + 30;
            if (ij_dente_23 > 60) ij_dente_23 -= 60;

            cutoff = 0;
            //nPIN_TEST = 0;
        }
        if(voltai2c == 3)
        {
            //LED = 0;
            LED = 0;
            I2C_Leitura_Multiplos_Endereco(ENDERECO_COMUNICACAO, DADOS_LIDOS, ENDERECO_SINCRONISMO);
            Flag_I2c = 1;
        }
        
        if (Flag_I2c)
        {
            // PERALTA
            if (Chave_controle_geral == 1)
            {
                ig_tmr_disparo = ig_tmr_disparo_controle;
            }
            else
            {
                ig_tmr_disparo = buffer_read[8]; //valor[Vet_IgTrigger];
            }

            // PERALTA

            //ig_tmr_disparo = valor[Vet_IgTrigger];
            ij_tmr_disparo = 65535 - Make_16(buffer_read[5], buffer_read[6]); //65535 - Make_16(valor[Vet_InjTrigger1], valor[Vet_InjTrigger2]);
            tempo_bob = buffer_read[7];//valor[Vet_TmrBob];
            tempo_inj = 65535 - Make_16(buffer_read[1], buffer_read[2]); //65535 - Make_16(valor[Vet_TmrInj1], valor[Vet_TmrInj2]);

            rotacao = rotacao_peralta / 100; //valor[Vet_AddressRPM];
            ij_dente_14_extra = buffer_read[9];//valor[Vet_InjDntExtra];
            ij_dente_23_extra = ij_dente_14_extra + 30;
            Set_Vent = buffer_read[10]; //valor[Vet_SetVetVel2];
            estado_controle_ml = buffer_read[3];//valor[Vet_estado_controle_ign];

            Flag_I2c = 0;
        }



        if (Set_Vent >= 78) //HIROKI
        {
            nVEL_VENT2 = nPin_HIGH;
        }
        else if (Set_Vent <= 72)
        {
            nVEL_VENT2 = nPin_LOW;
        }

        // Logica de Deteccao de Fase
        if (rotacao < 6 && rotacao_ant < 6)
        {
            if (sinal_fase == 1)
            {
                volta14 = 255; // Injeta no 4
                //volta23 = 255; // Injeta no 2 - Invertia abaixo de 600 >corrigido
                volta23 = 0; // Injeta no 2
            }
            else
            {
                volta14 = 0; // Injeta no 1
                //volta23 = 0; // Injeta no 3 - Invertia abaixo de 600 >corrigido
                volta23 = 255; // Injeta no 3
            }
            fase_ok = 1; // Ja detectou a fase pelo meno uma vez (requisito p/ a primeira inj.)
        }

        // Correcao para evitar perda de ignicao na transicao do dente 20 para o 19...
        //if (ig_dente_14 < 20 && ig_dente_14_ant >= 20 && (ig_dente_14_ant - ig_dente_14) < 30)
        if ((ig_dente_14 < 30) && (ig_dente_14_ant >= 30) && ((ig_dente_14_ant - ig_dente_14) < 30))
        {
            // Forca ignicao 14 neste dente...
            if (ig_tmr_disparo > 1 && ig_pos == 0)
            {
                //Config_Timer2();                    //setup_timer_2(T2_DIV_BY_16,ig_tmr_disparo,16);
                PR2 = ig_tmr_disparo; // QUANDO  o TIMER2 for igual o valor do PR2 ele da a interrupção
                Set_Timer2(0);
                ClearFlag_Interrupt(nFLAG_TMR2);
                Enable_Interrupt(nINT_TIMER2);
                //LATCbits.LATC2 = !LATCbits.LATC2;

                ig_pos = 14;
            }
        }

        //if (ig_dente_23 < 20 && ig_dente_23_ant >= 20 && (ig_dente_23_ant - ig_dente_23) < 30)
        if ((ig_dente_23 < 30) && (ig_dente_23_ant >= 30) && ((ig_dente_23_ant - ig_dente_23) < 30))
        {
            // Forca ignicao 23 neste dente...
            if (ig_tmr_disparo > 1 && ig_pos == 0)
            {
                //Config_Timer2();                    //setup_timer_2(T2_DIV_BY_16,ig_tmr_disparo,16);
                PR2 = ig_tmr_disparo;
                Set_Timer2(0);
                ClearFlag_Interrupt(nFLAG_TMR2);
                Enable_Interrupt(nINT_TIMER2);
                ig_pos = 23;
            }
        }

        // Correcao para evitar perda de injecao na transicao do dente 20 para o 19...
        if ((ij_dente_14 < 30) && (ij_dente_14_ant >= 30) && ((ij_dente_14_ant - ij_dente_14) < 30))
        {
            Set_Timer0(ij_tmr_disparo);
            ClearFlag_Interrupt(nFLAG_TMR0);
            Enable_Interrupt(nINT_TIMER0);
            if (!volta14) ij14_pos = 1;
            else ij14_pos = 4;
        }

        if ((ij_dente_23 < 30) && (ij_dente_23_ant >= 30) && ((ij_dente_23_ant - ij_dente_23) < 30))
        {
            Set_Timer3(ij_tmr_disparo);
            ClearFlag_Interrupt(nFLAG_TMR3);
            Enable_Interrupt(nINT_TIMER3);
            if (!volta23) ij23_pos = 3;
            else ij23_pos = 2;
        }
    }

    if ((dente == ig_dente_14) && (ig_pos == 0))
    {
        if (ig_tmr_disparo >= 1)
        {
            //Config_Timer2();                    //setup_timer_2(T2_DIV_BY_16,ig_tmr_disparo,16);
            PR2 = ig_tmr_disparo;
            Set_Timer2(0);
            ClearFlag_Interrupt(nFLAG_TMR2);
            Enable_Interrupt(nINT_TIMER2);

            //LATCbits.LATC2 = !LATCbits.LATC2;
            ig_pos = 14;
        }
    }

    if ((dente == ig_dente_23) && (ig_pos == 0))
    {
        if (ig_tmr_disparo >= 1)
        {
            //Config_Timer2();                    //setup_timer_2(T2_DIV_BY_16,ig_tmr_disparo,16);
            PR2 = ig_tmr_disparo;
            Set_Timer2(0);
            ClearFlag_Interrupt(nFLAG_TMR2);
            Enable_Interrupt(nINT_TIMER2);
            ig_pos = 23;
        }
    }

    // Se nao ident. a fase uma vez nao deixa dar os sinais de inj
    if (rotacao > 65 || fase_ok == 0) return; // O GER corta a inj, mas esta condicao reforca o corte (seguranca)
    if (PORT_L15 == 0) return;

    //    if ((dente == ij_dente_14_extra) && (ij23_pos==20))
    //    {
    //         //sinal_inj1 = nPin_HIGH;
    //    }
    //
    //    if ((dente == ij_dente_14_extra) && (ij23_pos==30))
    //    {
    //         //sinal_inj4 = nPin_HIGH;
    //    }
    //
    //    if ((dente == ij_dente_23_extra) && (ij14_pos==10))
    //    {
    //         //sinal_inj3 = nPin_HIGH;
    //    }
    //
    //    if ((dente == ij_dente_23_extra) && (ij14_pos==40))
    //    {
    //         //sinal_inj2 = nPin_HIGH;
    //    }

//    if(flag_marcus == 0)
//    {
//        dente = ij_dente_14;
//        ij14_pos = 0;
//    }

    if ((dente == ij_dente_14) && (ij14_pos == 0))
    {
        Set_Timer0(ij_tmr_disparo);
        ClearFlag_Interrupt(nFLAG_TMR0);
        Enable_Interrupt(nINT_TIMER0);
        if (!volta14) ij14_pos = 1;
        else ij14_pos = 4; // injeção no cilindro 4
//        flag_marcus = 1;
        
    }

    if ((dente == ij_dente_23) && (ij23_pos == 0))
    {
        Set_Timer3(ij_tmr_disparo);
        ClearFlag_Interrupt(nFLAG_TMR3);
        Enable_Interrupt(nINT_TIMER3);
        if (!volta23) ij23_pos = 3;
        else ij23_pos = 2;
    }
}

void No_Analogs(void)
{
    ANSELA = 0x00;
    ANSELB = 0x00;
    ANSELC = 0x00;
    ANSELD = 0x00;
    ANSELE = 0x00;
}

void Configure_HW(void)
{
    /* Configuração das Portas */
    TRISA = 0b00100001; //TRISA = 0b01100000; //TRISA = 0b01100000;
    TRISB = 0b00100011;
    TRISC = 0b00011000; //TRISC = 0b11011000;
    TRISD = 0b00000000; //TRISD = 0b00000000;
    TRISE = 0b00000100; //TRISE = 0b00000110;
    __delay_us(50);
    //__delay_ms(10);
    //__delay_ms(10);

    No_Analogs();
    ANSELA = 0b00000001;

    //SPI_SLAVE1();
    ADCON0 = 0b00000001; // liga o conversor AD seleciona o canal 1
    ADCON1 = 0b00000000; // referencias do conversor são VDD e VSS
    ADCON2 = 0b10100101; // resultado justificado à direita, tempo de aquisição

    Confi_Int_Ext0();

    Config_Timer0();
    Config_Timer1();
    Config_Timer2();
    PR2 = 255;
    Config_Timer3();
    Config_Timer5();
}

void Delay_ms(unsigned int value)
{
    unsigned int i;
    for (i = 0; i < value; i++)
    {
        __delay_ms(1);
    }
}

void main(void)
{
    Configure_HW();
    // T5CON = 0b00100010; //fonte de colck é o ciclo de máquina
    // prescaler de 1:4 , sem oscildor secundário
    // 16 bits de operação
    
    PIE5bits.TMR5IE = 1; // habilita chave individual de interrupção de timer5
    IPR5bits.TMR5IP = 1; // habilita alta prioridade para interrupção de TIMER5
    // Enable_Interrupt(nINT_TIMER5);
    Disable_Interrupt(nINT_TIMER5);
reset:
    /* Inicializacao das variaveis */
    t_periodo = 65530;
    t_periodo_ref = 65535; /* Garante que o pulso de detecção só acontecerá
                             após a comparação do segundo pulso de sinal */
    dente = 255;
    ig_pos = 0;
    ij23_pos = ij14_pos = 0;
    ig_tmr_disparo = 0;
    ij_tmr_disparo = 0;
    ig_dente_23 = 200;
    ig_dente_14 = 200;
    ij_dente_14 = 200;
    ij_dente_23 = 200;
    ig_dente_23_ant = 200;
    ig_dente_14_ant = 200;
    ij_dente_14_ant = 200;
    ij_dente_23_ant = 200;
    spi_ctr = 0;
    rotacao = 0;
    rotacao_ant = 0;

    volta14 = volta23 = 0;
    falha_ok = 0;
    fase_ok = 0;
    sinal_fase = 0;
    cutoff = 0;

    OUTEN_33810 = nPin_HIGH; //Desabilita 33810 no inicio

    nVEL_VENT1 = nOFF;
    nVEL_VENT2 = nOFF;
    nCOLD_CRANK = nOFF; //Dig Out Partida a frio
    nAIR_COND = nOFF;
    nBOMB_COLDA_CRANK = nOFF;
    nVEL_VENT2 = nPin_LOW;

    // Desliga Ignicoes e injecoes por seguranca
    //sinal_ig23 = nPin_LOW;
    //sinal_ig14 = nPin_LOW;
//    LED_IGN_1 = nPin_LOW;
//    LED_IGN_2 = nPin_LOW;
    LED_IGN_3 = nPin_LOW;
    LED_IGN_4 = nPin_LOW;

    nBOB_IGN1 = nPin_LOW;
    nBOB_IGN2 = nPin_LOW;

    RCONbits.IPEN = nON; // HABILITA PRIORIDADE DE INTERRUPÇÃO
    TMR0IP = nON;
    TMR1IP = nOFF; //  TIMER1 É BAIXA PRIORIDADE ??????????????????????
    TMR3IP = nON;
    TMR2IP = nON;

    //sinal_inj1 = nPin_LOW;
    //sinal_inj2 = nPin_LOW;
    //sinal_inj3 = nPin_LOW;
    //sinal_inj4 = nPin_LOW;
    LED_INJ_1 = nPin_LOW; //nPIN_TEST = 0;
    LED_INJ_2 = nPin_LOW;
    LED_INJ_3 = nPin_LOW;
    LED_INJ_4 = nPin_LOW;
    Chave_controle_geral = 0; // LEMBRAR DE TIRAR
    Enable_Interrupt(nINTERRUPT_GLOBAL); // HABILITA TODAS AS INTERRUPÇÕES DE ALTA PRIORIDADE
    Enable_Interrupt(nINTERRUPT_PERIPHERAL); // HABILITA DOAS AS INTERRUPÇÕES DE BAIXA PRIORIDADE
    Enable_ExtInt(nExt_Int0);
    Disable_Interrupt(nINT_TIMER5); // e também desliga a interrupção de timer5
    //   nPIN_TEST = nPin_LOW;
    //   Delay_ms(20);
    //   nPIN_TEST = nPin_HIGH;
    //   Delay_ms(20);
    //   nPIN_TEST = nPin_LOW;
    //   Delay_ms(20);
    //   nPIN_TEST = nPin_HIGH;
    //   Delay_ms(20);
    //   nPIN_TEST = nPin_LOW;
    //   Delay_ms(20);
    //   nPIN_TEST = nPin_HIGH;

    //Enable_IntSPI(nSPI1);      //TESTE
    
    dm = 0;
    STKFUL = 0;
    
    nPIN_LED3_TEST = nPin_HIGH;     //LED APAGADO 
    LED = 1;
    I2C_Master_Init();
    while (1)
    {
        if (rotacao_peralta > 500)
        {
            ADCON0 = 0b00000001; // liga o conversor AD seleciona o canal 0
            ADCON0bits.GO = 1; // inicia conversão
            while (ADCON0bits.GO == 1)
            {
            }; // espera

            pedal_ad = ADRESH * 256 + ADRESL; // valor ad do pedal

            if (pedal_ad > 155) // 155 se pedal pressionado desabilita o controle (pedal solto = 151)
            {
                
                cutoff = 0; // desliga o cut off para retomada
                //nPIN_TEST = ~nPIN_TEST;
                Chave_controle_geral = 0; // 0  para desabilitar o controle
                Disable_Interrupt(nINT_TIMER5); // e também desliga a interrupção de timer5
                // Enable_control_ign = 0;
            }
                //
            else // pedal solto
            {
               
                //
                //                if (rotacao_peralta > RPM_cutoff)
                //                {
                //                    cutoff = 1;
                //                }
                //                else
                //                {
                //                    cutoff = 0;
                //                }

                //SP_rpm = 1000;
                if (Chave_controle_geral == 0)
                {
                    Chave_controle_geral = 1;
                    // SP_rpm = 1000;
                    // fixa o set point em 1000 rpm na marcha lenta para teste (remover set point e comentario do case quando voltar para função normal)
                    T5CONbits.TMR5ON = 1; // inicia a contagem do timer5
                    Set_Timer5(15535); // carrega TIMER5 para estourar com 20 milissegundos
                    Enable_Interrupt(nINT_TIMER5);
                }
//               nPIN_LED3_TEST = ~nPIN_LED3_TEST;
                //nPIN_TEST = ~nPIN_TEST;
                // SP_rpm = 1000;
                //                    Disable_Interrupt(nINT_TIMER5); // e também desliga a interrupção de timer5

            }

            if (Chave_controle_geral == 1) // se 1 realiza o controle 1
            {

                if (Enable_control_ign == 1)
                {
                    
                    Enable_control_ign = 0;
                    Control_ml_ign();
                    // u = 18;
                    dente_controle = u / 6; // transforma o ângulo de controle em dentes
                    dente_controle = 14 - dente_controle; // transforma o dente em dente de referência (Descida do sinal da bobina))
                    n_controle = 10000 / periodo_peralta; // tempo_bob( 4ms)/(valor_timer1*prescaler*ciclo_máquina)
                    dente_subida = dente_controle - n_controle; // calcula o dente de subida do sinal da bobina
                    dente_subida_inteiro = (int) dente_subida;
                    ig_tmr_disparo_controle_temp = (dente_subida - (float) dente_subida_inteiro) * periodo_peralta;
                    ig_tmr_disparo_controle_temp = ig_tmr_disparo_controle << 1; // multiplica por dois para ficar na mesma formatação do valor do gerenciamento

                    if (ig_tmr_disparo_controle_temp < 10.0) // copiei do gerenciamento
                    {
                        ig_tmr_disparo_controle_temp = 10.0;
                    }
                    ig_tmr_disparo_controle_temp = ig_tmr_disparo_controle_temp / 52; // copiei do gerenciamento

                    if (ig_tmr_disparo_controle_temp < 1)
                    {

                        ig_tmr_disparo_controle_temp = 1.0; // copiei do gerenciamento
                    }

                    ig_tmr_disparo_controle = (int) ig_tmr_disparo_controle_temp << 1;

                } // fim do enable control
            } // fim da chave geral do controle

        }
        else
        {
            Chave_controle_geral = 0;
            cutoff = 0;
        }

        //nPIN_TEST = STKFUL; // teste de hardware caso a pilha estoure

    } // fim do while 1
    goto reset;

    return;
}

void Control_ml_ign(void)
{
    Erro_RPM = SP_rpm - rotacao_peralta; //SP_rpm
    // if ((Erro_RPM < 10.0) && (Erro_RPM > -10.0)) return;

    u = K * Erro_RPM - K * z * Erro_RPM_ant + u_ant;

    if (Erro_RPM > 60)
    {
        K = 0.44546;
        teto_avanco = 35;
    }
    else
    {
        K = 0.24546;
        teto_avanco = 21;

    }
    if (u > teto_avanco)
    {
        u = teto_avanco;
        Erro_RPM_ant = 0;
    }
    if (u < 0) u = 0;

    Erro_RPM_ant = Erro_RPM;
    u_ant = u;

}
/*************************************
 *       END SW SINCRONISMO          *
 ************************************/

/*
 *                                 PIC18F4550
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