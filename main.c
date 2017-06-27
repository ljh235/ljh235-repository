#include <msp430.h> 

/*
 * main.c
 */



int main(void) {

	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	P2DIR |= BIT0 + BIT1 + BIT2;
	P2OUT &= ~(BIT0 + BIT1 + BIT2);



	BCSCTL3 |= LFXT1S_2;

	CCR0 = 12000;
	CCR1 = 24000;
	CCR2 = 36000;

			CCTL0 = CCIE;
			CCTL1 = CCIE;
			CCTL2 = CCIE;

	TACTL = TASSEL_1 + MC_2 + TAIE;

	_BIS_SR(GIE);
	
	for(;;){

	}
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
	P2OUT ^= BIT0;
	CCR0 += 12000;
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer_A1(void){
switch( TAIV )
	{
		case 2 : P2OUT ^= BIT1;
		CCR1 += 24000;
		break;

		case 4 : P2OUT ^= BIT2;
		CCR2 += 36000;
		break;
	}
}
