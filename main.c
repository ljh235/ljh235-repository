#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW +WDTHOLD;                  // Stop Watchdog Timer

  if (CALBC1_8MHZ==0xFF)                         // If calibration constants erased
  {
    while(1);                                            // do not load, trap CPU!!
  }
  DCOCTL = 0;
  BCSCTL1 = CALBC1_8MHZ;                      // Set range
  DCOCTL = CALDCO_8MHZ;                      // Set DCO step + modulation*/

  P1DIR |= BIT5 + BIT4;                             // P1.x output
  P1SEL |= BIT5 + BIT4;                             // P1.x option select
  P1SEL2 |= BIT4;

  CCTL0 = OUTMOD_4;                             // Toggle Mode
  CCTL2 = OUTMOD_4;

  const int clk = 8;
  const int div = 1;
  const int frq = 150;

  const int step = ((clk*1000)/(div*frq))/2;

  CCR0 = step;                                         // Setting CCR value (Setting Step)
  CCR2 = 1;                                           // PWM Output_reverse

  TACTL = TASSEL_2 + MC_1;                    // SMCLK, upmode

}
