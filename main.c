#include <msp430.h>
#include "Lab5_LCD_GAME.h"
#include "buttons/button.h"
#include "LCD/Lab4_LCD.h"
#include "random/rand.h"

/*
 * main.c
 */

void init_timer()
{
    // do timer initialization work
	TACTL &= ~(MC1|MC0);        // stop timer

	TACTL |= TACLR;             // clear TAR

	TACTL |= TASSEL1;           // configure for SMCLK - what's the frequency (roughly)?

	TACTL |= ID1|ID0;           // divide clock by 8 - what's the frequency of interrupt?

	TACTL &= ~TAIFG;            // clear interrupt flag

	TACTL |= MC1;               // set count mode to continuous

	TACTL |= TAIE;              // enable interrupt

	__enable_interrupt();       // enable maskable interrupts

}

void init_buttons()
{
    // do button initialization work
	configureP1PinAsButton(BIT1|BIT2|BIT3|BIT4);
	P1IE |= BIT1|BIT2|BIT3|BIT4;                 // enable the interrupts
	P1IES |= BIT1|BIT2|BIT3|BIT4;                   // configure interrupt to sense falling edges
}

char flag = 0;
char isGameOver = 1;
unsigned char player = 0;
char direction = 0;
char myString1[] = "YOU     ";
char myString2[] = "WIN!      ";
char myString3[] = "YOU     ";
char myString4[] = "LOSE!     ";
char myString5[] = "BOOM!    ";
char myString6[] = "Set Mine";
char myString7[] = "Button 1";
char myString8[] = "Again    ";

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    init_timer();
    init_buttons();
    __enable_interrupt();
    initSPI();
    LCDinit();

    while(1)
    {
			player = 0;
    		char mine1 = 0;
			char mine2 = 0;
    		flag = 0;

			LCDclear();
			TACTL &= ~(MC1|MC0);      // pause timerA clock
			mine1 = generateMines(0, myString6, myString7);
			mine2 = generateMines(mine1, myString6, myString8);
			LCDclear();
			printMines(mine1, mine2);
			TACTL |= MC1;				// resume timerA clock

			player = initPlayer();
         	printPlayer(player);
         	isGameOver = 1;

           	while (isGameOver)
           	{

           		isGameOver = didPlayerWin(player, isGameOver, myString1, myString2);

           		if((player == mine1) || (player == mine2))
				{
					isGameOver = 0;
					LCDclear();
					writeString(myString5, 8);
					__delay_cycles(1000000);

				}

           		if(flag >= 4)
           		    {
           		    	isGameOver = 0;
           				LCDclear();
           				writeString(myString3, 8);
           				cursorToLineTwo();
           				writeString(myString4, 8);
           				__delay_cycles(1000000);
           		    }
           	}

           	LCDclear();

    }
	
	return 0;
}

// Flag for continuous counting is TAIFG
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR()
{
    TACTL &= ~TAIFG;            // clear interrupt flag
    flag += 1;

}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1_ISR(void)
{
    if (P1IFG & BIT1)
    {
    	if((isP1ButtonPressed(BIT1)) && (player != 0) && (isGameOver == 1)) // player !=0 prevents
    																		// interrupting during mine setting
		{
			direction = RIGHT;
			player = movePlayer(player, direction);
			TAR = 0;
			flag = 0;
		}


    	P1IFG &= ~BIT1;                            // clear flag

    }
    if (P1IFG & BIT2)
        {
        	if((isP1ButtonPressed(BIT2)) && (player != 0) && (isGameOver == 1))
    		{
    			direction = LEFT;
    			player = movePlayer(player, direction);
    			TAR = 0;
    			flag = 0;
    		}
        	P1IFG &= ~BIT2;                            // clear flag

        }
    if (P1IFG & BIT3)
        {
        	if((isP1ButtonPressed(BIT3)) && (player != 0) && (isGameOver == 1))
    		{
    			direction = UP;
    			player = movePlayer(player, direction);
    			TAR = 0;
    			flag = 0;
    		}
        	P1IFG &= ~BIT3;                            // clear flag

        }
    if (P1IFG & BIT4)
        {
        	if((isP1ButtonPressed(BIT4)) && (player != 0) && (isGameOver == 1))
    		{
    			direction = DOWN;
    			player = movePlayer(player, direction);
    			TAR = 0;
    			flag = 0;
    		}
        	P1IFG &= ~BIT4;                            // clear flag

        }

}


