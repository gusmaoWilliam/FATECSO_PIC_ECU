
//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "lcd4bitsmode.h"
#include "Rot_Delay.h"

//------------------------------------------------------------------------------
// Definições
//------------------------------------------------------------------------------

// TRIS (I/O direction) dos pinos que se comunicam com o LCD. Devem ser configurados
// aproriadamente se este arquivo for reutilizado em outro projeto.


#define TRIS_LCDBIT0 TRISBbits.TRISB2
#define TRIS_LCDBIT1 TRISBbits.TRISB3
#define TRIS_LCDBIT2 TRISBbits.TRISB4
#define TRIS_LCDBIT3 TRISBbits.TRISB5
#define TRIS_LCDRS   TRISEbits.TRISE0
#define TRIS_LCDRW   TRISEbits.TRISE1
#define TRIS_LCDEN   TRISEbits.TRISE2


// Estados do pino RS do LCD.

#define OUTPUT_LEVEL_HIGH 1
#define OUTPUT_LEVEL_LOW  0


// Direction definition

#define OUTPUT_PIN_MODE 0
#define INPUT_PIN_MODE  1


// Estados do pino RS do LCD

#define LCD_RS_DATA 1
#define LCD_RS_CMD  0

// Comandos que podem ser enviados ao LCD

#define LCD_CMD_CLEAR       0x01
#define LCD_CMD_SHIFT_LEFT  0x18
#define LCD_CMD_SHIFT_RIGHT 0x1C
#define LCD_CMD_LINE1       0x80
#define LCD_CMD_LINE2       0xC0

// habilita envio de comandos para LCD
#define LCD_ENABLE_CMD()    (PIN_LCDRS = LCD_RS_CMD)

// habilita envio de dados para LCD
#define LCD_ENABLE_DATA()   (PIN_LCDRS = LCD_RS_DATA)

//------------------------------------------------------------------------------
// IMPLEMENTAÇÃO DE FUNÇÕES PÚBLICAS
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Função de inicialização do LCD.
//------------------------------------------------------------------------------

void hwConfigurarLcd(void)
{
   // pinos configurados como saída

   TRIS_LCDBIT0 = OUTPUT_PIN_MODE;
   TRIS_LCDBIT1 = OUTPUT_PIN_MODE;
   TRIS_LCDBIT2 = OUTPUT_PIN_MODE;
   TRIS_LCDBIT3 = OUTPUT_PIN_MODE;
   TRIS_LCDEN = OUTPUT_PIN_MODE;
   TRIS_LCDRS = OUTPUT_PIN_MODE;
   TRIS_LCDRW = OUTPUT_PIN_MODE;
   PIN_LCDRW = OUTPUT_LEVEL_LOW;

   // Inicializa display com:
   // Modo 04 bits # 2L x 16C # Célula 5 x 7 dots # Sem cursor

   escreverDados(0x00);
   delayMs(20);

   escreverComando(0x03);
   delayMs(5);
   escreverComando(0x03);
   delayMs(11);
   escreverComando(0x03);

   escreverComando(0x02);
   escreverComando(0x02);
   escreverComando(0x08);
   escreverComando(0x00);
   escreverComando(0x0C);
   escreverComando(0x00);
   escreverComando(0x06);
}


//------------------------------------------------------------------------------
// Função para escrever um único caracter no LCD.
//------------------------------------------------------------------------------

void lcdEscreverChar(char ch)
{
   char low, high;
   low  = ch & 0x0F;
   high = (ch & 0xF0) >> 4;

   LCD_ENABLE_DATA();
   escreverDados(high);
   pulse();
   escreverDados(low);
   pulse();
}


//------------------------------------------------------------------------------
// Função para escrever uma string no LCD na linha X coluna informada.
//------------------------------------------------------------------------------

void lcdEscreverCharPosicao(unsigned char ch, unsigned char line, unsigned char col)
{
   lcdPosicionarCursor(line, col);
   lcdEscreverChar(ch);
}


//------------------------------------------------------------------------------
// Função para escrever uma string no LCD
//------------------------------------------------------------------------------

void lcdEscreverString(const char *str)
{
   while (*str) {
      lcdEscreverChar(*str);
      str++;
      delayMs(5);
   }
}


//------------------------------------------------------------------------------
// Função para escrever uma string no LCD na linha coluna informada.
//------------------------------------------------------------------------------

void lcdEscreverStringPosicao(const char *string, unsigned char line, unsigned char col)
{
   lcdPosicionarCursor(line, col);
   lcdEscreverString(string);
}

//------------------------------------------------------------------------------
// Função para posicionar o cursor em uma linha x coluna do LCD.
//------------------------------------------------------------------------------

void lcdPosicionarCursor(unsigned char line, unsigned char col)
{
   if (line > 2) line = 2;
   if (col > 16) col = 16;

   unsigned char position, posHigh, posLow;

   if (line == 1) {
      position = 0x80 + col - 1;
      posHigh = position >> 4;
      posLow = position & 0x0F;

      escreverComando(posHigh);
      escreverComando(posLow);
   } else if (line == 2) {
      position = 0xC0 + col - 1;
      posHigh = position >> 4;
      posLow = position & 0x0F;

      escreverComando(posHigh);
      escreverComando(posLow);
   }
}


//------------------------------------------------------------------------------
// Função para enviar comandos para limpar o LCD.
//------------------------------------------------------------------------------

void lcdLimparTela(void)
{
   escreverComando(0);
   escreverComando(1);
}


//------------------------------------------------------------------------------
// Rotaciona o texto em 'n' posições para esquerda.
//------------------------------------------------------------------------------

void lcdDeslocarTextoEsquerda(unsigned char n)
{
   unsigned char i;
   for (i = 0; i < n; ++i) {
      escreverComando(0x01);
      escreverComando(0x08);
   }
}


//------------------------------------------------------------------------------
// Rotaciona o texto em 'n' posições para direita.
//------------------------------------------------------------------------------

void lcdDeslocarTextoDireita(unsigned char n)
{
   unsigned char i;
   for (i = 0; i < n; ++i) {
      escreverComando(0x01);
      escreverComando(0x0C);
   }
}

//------------------------------------------------------------------------------
// Função para escrever um número de ponto flutuante no LCD.
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// IMPLEMENTAÇÃO DE FUNÇÕES PRIVADAS
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Função para escrever um byte nos pinos 04 pinos de dados do LCD.
//------------------------------------------------------------------------------

static void escreverDados(unsigned char ch)
{
   ch & 1 ? (PIN_LCDBIT0 = 1) : (PIN_LCDBIT0 = 0);
   ch & 2 ? (PIN_LCDBIT1 = 1) : (PIN_LCDBIT1 = 0);
   ch & 4 ? (PIN_LCDBIT2 = 1) : (PIN_LCDBIT2 = 0);
   ch & 8 ? (PIN_LCDBIT3 = 1) : (PIN_LCDBIT3 = 0);
}


//------------------------------------------------------------------------------
// Função para enviar um comando para o LCD.
//------------------------------------------------------------------------------

static void escreverComando(unsigned char cmd)
{
   LCD_ENABLE_CMD();
   escreverDados(cmd);
   pulse();
}


//------------------------------------------------------------------------------
// Função para pulsar o pino E do LCD.
//------------------------------------------------------------------------------

static void pulse(void)
{
   PIN_LCDEN = OUTPUT_LEVEL_HIGH;
   delayMs(4);
   PIN_LCDEN = OUTPUT_LEVEL_LOW;
}



void lcdEscreverInt(int data_value) 
{
	
    
	unsigned char i, dig[5], cont_dig;
	unsigned int a;
	
	if(data_value<1000)						  cont_dig = 4;
	if(data_value>=1000 && data_value<=99999) cont_dig = 5;
	
	a = (unsigned int)(data_value * 10.0 + 0.5);
	
	for(i =0; i<cont_dig; i++) {
		dig[i] = (unsigned char)(a % 10);
		a /= 10;
	}
	if(cont_dig == 4) {
		//if(!dig[3]) LCD_print_char(' ');
		//else		LCD_print_char(dig[3]  + '0');
		if(dig[3]) lcdEscreverChar(dig[3]  + '0');
		
		lcdEscreverChar(dig[2] + '0');
		lcdEscreverChar(dig[1] + '0');
	}
	if(cont_dig == 5) {
		//if(!dig[4]) LCD_print_char(' ');
		//else		LCD_print_char(dig[4] + '0');
		if(dig[4]) lcdEscreverChar(dig[4] + '0');
		
		lcdEscreverChar(dig[3] + '0');
		lcdEscreverChar(dig[2] + '0');
		lcdEscreverChar(dig[1] + '0');
	}

}

void lcdEscreverFloat(float data_value) {
	
	unsigned char i, dig[5], cont_dig;
	unsigned int a;
		
	if(data_value<1000.0)						  cont_dig = 4;
	if(data_value>=1000.0 && data_value<=10000.0) cont_dig = 5;
		
	a = (unsigned int)(data_value * 10.0 + 0.5);
		
	for(i =0; i<cont_dig; i++) {
		dig[i] = (unsigned char)(a % 10);
		a /= 10;
	}
	if(cont_dig == 4) {
		if(!dig[3]) lcdEscreverChar(' ');
		else		lcdEscreverChar(dig[3]  + '0');
			
		lcdEscreverChar(dig[2] + '0');
		lcdEscreverChar(dig[1] + '0');
		lcdEscreverChar('.');
		lcdEscreverChar(dig[0] + '0');
	}
	if(cont_dig == 5) {
		if(!dig[4]) lcdEscreverChar(' ');
		else		lcdEscreverChar(dig[4] + '0');
		
		lcdEscreverChar(dig[3] + '0');
		lcdEscreverChar(dig[2] + '0');
		lcdEscreverChar(dig[1] + '0');
		lcdEscreverChar('.');
		lcdEscreverChar(dig[0] + '0');
	}
}

void lcdEscreverHex(unsigned int x)
{    
    char msb = (x / 16 > 10) ? 55 + x / 16 : 48 + x / 16;
    if (msb == ':')
        msb = 'A';
    char lsb = (x % 16 > 10) ? 55 + x % 16 : 48 + x % 16;
    if (lsb == ':')
        lsb = 'A';
    lcdEscreverChar(msb);
    lcdEscreverChar(lsb);
}

void lcdEscreverHexPosicao(unsigned int x, unsigned char line, unsigned char col)
{
   lcdPosicionarCursor(line, col);
   lcdEscreverHex(x);
}

void lcdEscreverIntPosicao(unsigned int x, unsigned char line, unsigned char col)
{
    lcdPosicionarCursor(line, col);
    lcdEscreverInt(x);
}