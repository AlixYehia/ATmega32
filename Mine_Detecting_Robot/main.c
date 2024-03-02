#include <avr/io.h>
#include "LCD.h"
#define  F_CPU 8000000
#include <util/delay.h>


int main(void)
{
	LCD_vInit();
	unsigned short a,b,high,distance;
	DIO_vsetPINDir('D',7,1);    // trigger pin


    // Reed switch initialize
	DIO_vsetPINDir('D', 1, 0);  // Reed switch pin as input

	// Buzzer initialize
	DIO_vsetPINDir('D', 2, 1);  // Buzzer pin as output


	// Motors control
	DIO_vsetPINDir('C', 0, 1);
	DIO_vsetPINDir('C', 1, 1);   // Motors pins as output
	DIO_vsetPINDir('C', 2, 1);
	DIO_vsetPINDir('C', 3, 1);




    while(1)
    {
    	// ultrasonic activation
		TCCR1A = 0;
		TIFR = (1<<ICF1);  	/* Clear ICF (Input Capture flag)  */
		DIO_write('D',7,1);
		_delay_us(50);          // making 50 us pulse width on trigger
		DIO_write('D',7,0);

	    TCCR1B = 0xc1;  	/* Rising edge, no prescaler , noise canceler*/
	    while ((TIFR&(1<<ICF1)) == 0);
	    a = ICR1;  		/* Take value of capture register */
	    TIFR = (1<<ICF1);  	/* Clear ICF flag */
	    TCCR1B = 0x81;  	/* Falling edge, no prescaler ,noise canceler*/
	    while ((TIFR&(1<<ICF1)) == 0);
	    b = ICR1;  		/* Take value of capture register */
	    TIFR = (1<<ICF1);  	/* Clear ICF flag */
		TCNT1=0;
	    TCCR1B = 0;  		/* Stop the timer */
		high=b-a;
		distance= ((high*34600)/(F_CPU*2)) ;

		if (distance<80){
			LCD_movecursor(1,1);
			LCD_vSend_string("Obstacle at:");
			LCD_vSend_char((distance/10)+48);
			LCD_vSend_char((distance%10)+48);
			LCD_vSend_string("cm");
			_delay_ms(500);
			LCD_clear_first_row();
		}



		unsigned char reed_switch_Value = DIO_u8read('D', 1);       // reed switch is active low

		// mine detecting
		if( reed_switch_Value == 0 ){             // reed switch on
			// activate buzzer
			DIO_write('D',2,1);

			// car stops
			DIO_write('C',0,0);
			DIO_write('C',1,0);
			DIO_write('C',2,0);
			DIO_write('C',3,0);

			// LCD message
			LCD_clear_second_row();
			LCD_movecursor(2,4);
			LCD_vSend_string("Mine Found!");
			_delay_ms(500);

		}
		else{                                     // reed switch off
			// deactivate buzzer
			DIO_write('D',2,0);

			// LCD message
			LCD_clear_second_row();
     		LCD_movecursor(2,5);
			LCD_vSend_string("No Mine");
			_delay_ms(500);

			// obstacle avoidance

			if (distance >= 80 ){
				// car moves forward
				DIO_write('C',0,1);
				DIO_write('C',1,0);
				DIO_write('C',2,1);
				DIO_write('C',3,0);

			}
			else{
				// car moves right
				DIO_write('C',0,1);
				DIO_write('C',1,0);
				DIO_write('C',2,0);
				DIO_write('C',3,0);
				_delay_ms(600);

				// forward
				DIO_write('C',0,1);
				DIO_write('C',1,0);
				DIO_write('C',2,1);
				DIO_write('C',3,0);

			}

		}

	}
}
