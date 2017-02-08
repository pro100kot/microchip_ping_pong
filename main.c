#include "Definitions.h"                // Подключение файла с определениями
#pragma config OSC       = INTIO67      // Internal oscillator block, port function on RA6 and RA7
#pragma config FCMEN     = OFF          // Fail-Safe Clock Monitor disabled
#pragma config IESO      = OFF          // Internal/External Oscillator Switchover bit, Two-Speed Start-up is disabled in INTIO osc mode
#pragma config PWRT      = OFF          // Power-up Timer disabled
#pragma config BOREN     = OFF          // Brown-out Reset disabled in hardware and software 
#pragma config BORV      = 0            // VBORMV is set to 3.0V
#pragma config WDT       = OFF          // WDT disabled in hardware; SWDTEN bit disabled
#pragma config CCP2MX    = PORTC        // ECCP2 is multiplexed with RC1
#pragma config PBADEN    = OFF          // PORTB<4:0> pins are configured as digital I/O on Reset
#pragma config MCLRE     = ON           // MCLR pin enabled; RE3 input pin disabled
#pragma config LVP       = OFF          // Single-Supply ICSP disabled 
#pragma config XINST     = OFF          // Instruction set extension and Indexed Addressing mode disabled (Legacy mode)          

unsigned int leftBorder;
unsigned int rightBorder;
unsigned int upBorder;
unsigned int downBorder;

unsigned int colorBlack;
unsigned int colorWhite;

unsigned int upRacketCentre;
unsigned int upRacketIndent;
unsigned int upRacketWidth;


void Int_Handle (void);

#pragma code HIGH_INT_VECTOR = 0x8 // Код помещается в ячейку 0x08
void high_ISR (void)
{
	_asm
	goto Int_Handle
	_endasm
}
#pragma code

#pragma interrupt Int_Handle
void Int_Handle (void)
{
	if (INTCONbits.TMR0IF == 1 && INTCONbits.TMR0IE == 1)
	{
		TMR0H = 0xE1; // Загрузка значения в старший байт 21
		TMR0L = 0x7A; // Загрузка значения в младший байт
		//secondsFromStart++;
		//isNeedToWriteTime = 1;
		INTCONbits.TMR0IF = 0; // Сброс флага прерывания Тimer0 24
}
	
	return;
}



void Init(void)
{
	unsigned char Seed;
	Seed = EEPROM_Read (EEPROM_ADDR);
    srand (Seed++);
    EEPROM_Write (EEPROM_ADDR, Seed); 

    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    PORTE = 0x00;
    
    TRISA = 0xF0;
    TRISB = 0xFF;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;
	
	TRISB = 0b00111111;
	// Настройка прерываний: разрешение, приоритет
	RCONbits.IPEN = 1; // Включение приоритетов прерываний
	INTCON2bits.TMR0IP = 1; // Высокий приоритет прерывания Timer0
	INTCONbits.TMR0IE = 1; // Разрешение прерывания от Timer0
	INTCONbits.TMR0IF = 0; // Сброс флага прерывания
	INTCONbits.GIEH = 1; // Разрешение при?ма всех прерываний
	INTCONbits.GIEL = 0; // Запрет прерываний низкого уровня
	// Побитовая настройка таймера-сч?тчика Timer0
	T0CONbits.T08BIT = 0; // Режим 16 бит
	T0CONbits.T0CS = 0; // Внутренний источник тактирования
	T0CONbits.PSA = 0; // Включение предделителя
	T0CONbits.T0PS2 = 1; // Задание коэффициента деления 1:256
	T0CONbits.T0PS1 = 1;
	T0CONbits.T0PS0 = 1;
	TMR0H = 0xE1; // Загрузка значения в старший байт
	TMR0L = 0x7A; // Загрузка значения в младший байт
	
    LCD_Init ();	// Инициализация дисплея 
    return;
}

//void newGame();
void drawBorders();
void drawUpRacket();
void buttonsHandler();

void main (void)
{
	unsigned int black = 0b0000000000000000;
	OSCCON = 0b01110010;
	OSCTUNE = 0b01000000;
	ADCON1 = 0b00001111;
    Init();

	drawBorders();
	drawUpRacket();
	
	while(1){
	buttonsHandler();
	}

}

void drawBorders()
{

Disp_Line(leftBorder,upBorder,rightBorder,upBorder,colorBlack); //upper border
Disp_Line(leftBorder,upBorder,leftBorder,downBorder,colorBlack); //left border
Disp_Line(rightBorder,upBorder,rightBorder,downBorder,colorBlack); //right border
Disp_Line(leftBorder,downBorder,rightBorder,downBorder,colorBlack); //down border

}

void drawUpRacket()
{
	Disp_Line(upRacketCentre - upRacketWidth/2,upRacketIndent,upRacketCentre + upRacketWidth/2,upRacketIndent,colorBlack); //upper border
}

void clearUpRacket()
{
	Disp_Line(upRacketCentre - upRacketWidth/2,upRacketIndent,upRacketCentre + upRacketWidth/2,upRacketIndent,colorWhite); //upper border
}

void changeUpRacketCoords(int val)
{
	if(val>0) //right
	{
		if(upRacketCentre + upRacketWidth/2 + val > rightBorder)
			upRacketCentre = rightBorder - upRacketWidth/2 - 1;
		else
			upRacketCentre += val;
	}
	else //left
	{
		if(upRacketCentre - upRacketWidth/2 + val < leftBorder)
			upRacketCentre = leftBorder + upRacketWidth/2 + 1;
		else
			upRacketCentre += val;
	}
}

void moveUpRacketRight()
{
	clearUpRacket();
	changeUpRacketCoords(4);
	drawUpRacket();	
}

void moveUpRacketLeft()
{
	clearUpRacket();
	changeUpRacketCoords(-4);
	drawUpRacket();
}
	
	

void buttonsHandler()
{
	unsigned char scan = ~BUTTONS_PORT & 0b00111111;
	if(scan & 0b00000010)
		{
			Delay10KTCYx (KEY_DELAY);
			//moveLeft();
			moveUpRacketLeft();
		}
	if(scan & 0b00100000)
		{
			Delay10KTCYx (KEY_DELAY);
			//moveRight();
			moveUpRacketRight();
		}
	if(scan & 0b00010000)
		{
			//Delay10KTCYx (KEY_DELAY);
			//moveDown();
		}
	if(scan & 0b00000100)
		{
			//Delay10KTCYx (KEY_DELAY);
			//moveUp();
		}
	if(scan & 0b00001000)
		{
			//Delay10KTCYx (KEY_DELAY);
			//openCell();
		}		
	if(scan & 0b00000001)
		{
			//Delay10KTCYx (KEY_DELAY);
			//setFlag();
		}	
}



/*

scan = ~BUTTONS_PORT & 0b00111111;

void gameMoveHandler(unsigned char scan)
{
	if(scan & 0b00000010)
		{
			Delay10KTCYx (KEY_DELAY);
			moveLeft();
		}
	if(scan & 0b00100000)
		{
			Delay10KTCYx (KEY_DELAY);
			moveRight();
		}
	if(scan & 0b00010000)
		{
			Delay10KTCYx (KEY_DELAY);
			moveDown();
		}
	if(scan & 0b00000100)
		{
			Delay10KTCYx (KEY_DELAY);
			moveUp();
		}
	if(scan & 0b00001000)
		{
			Delay10KTCYx (KEY_DELAY);
			openCell();
		}		
	if(scan & 0b00000001)
		{
			Delay10KTCYx (KEY_DELAY);
			setFlag();
		}	
}
*/