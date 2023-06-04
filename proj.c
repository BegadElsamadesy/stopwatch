#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

char pin[6];

void Timer_1_init(){
	TCNT1	=	0;								//initialize timer counter to 0
	OCR1A	=	978;								//every tick is 1 second
	TIMSK	|=	(1<<OCIE1A);							//Output Compare A Match Interrupt Enable
	TCCR1A	|=	(1<<FOC1A); 							//non pmw pin
	TCCR1B	|=	(1<<WGM12)	|	(1<<CS12)	|	(1<<CS10);	//compare mood ||  clock(1Mhz)/1024 pre-scaler
}

void INT0_init(){
	DDRD	&=	(~(1<<PD2));							//INT 0 PAUSE
	PORTB 	|= 	(1<<PD2);         						// Activate the internal pull up resistor at PD2
	MCUCR	|=	(1<<ISC01);							//INT 1 with falling edge
	GICR	|=	(1<<INT0);							//activate external interrupt of INT 0
}

void INT1_init(){
	DDRD	&=	(~(1<<PD3));							//INT 1 RESET
	MCUCR	|=	(1<<ISC11)	|	(1<<ISC10);				//INT 1 with rising edge
	GICR	|=	(1<<INT1);							//activate external interrupt of INT 1
}

void INT2_init(){
	DDRB	&=	(~(1<<PB2));							//INT 2 RESUME
	PORTB 	|= 	(1<<PB2);        						// Activate the internal pull up resistor at PB2
	MCUCSR	&=	(~(1<<ISC2));							//INT 2 with falling edge
	GICR	|=	(1<<INT2);							//activate external interrupt of INT 2
}

ISR(TIMER1_COMPA_vect){
	pin[0]++;
	if(pin[0]==10){									//digit 1 seconds
		pin[0]=0;
		pin[1]++;								//Increament seconds 2nd digit
		if(pin[1]==6){								//digit 2 seconds
			pin[1]=0;
			pin[2]++;							//Increament mins 1st digit
		}
	}
	if(pin[2]==10){									//digit 1 mins
		pin[2]=0;
		pin[3]++;								//Increament mins 2nd digit
		if(pin[3]==6){								//digit 2 mins
			pin[3]=0;
			pin[4]++;							//Increament hours 1st digit
		}
	}
	if(pin[4]==10){									//digit 1 hours
		pin[4]=0;
		pin[5]++;								//Increament hours 2nd digit
		if(pin[5]==6){								//digt 2 hours
			pin[5]=0;							//Reset all 
			pin[4]=0;
			pin[3]=0;
			pin[2]=0;
			pin[1]=0;
			pin[0]=0;
		}
	}
}

ISR(INT0_vect){											//Reset INT
	TCNT1=0;										//Reset counter
	pin[5]=0;
	pin[4]=0;
	pin[3]=0;
	pin[2]=0;
	pin[1]=0;
	pin[0]=0;
}

ISR(INT1_vect){											//Pause INT
	TCCR1B	&=(~(1<<CS12));									//Turn off clock
	TCCR1B	&=(~(1<<CS11));									//Turn off clock
	TCCR1B	&=(~(1<<CS10));									//Turn off clock
}

ISR(INT2_vect){											//Resume INT
	TCCR1B	|=	(1<<WGM12)	|	(1<<CS12)	|	(1<<CS10);		//Turn on clock again with same prescaller
}


int main(){
	unsigned char i=0;
	DDRC	|=	0x0F;								//output from AVR to decoder
	PORTC	&=	0xF0;								//Initialized b 0
	DDRA	|=	0x3F;								//7 segment enable
	PORTA	|=	0x3F;								//initialized on
	SREG	|=	(1<<7);								//enable IBIT
	Timer_1_init();
	INT0_init();
	INT1_init();
	INT2_init();
	while(1){
		for(i=0;i<6;i++){
			PORTA	=	(1<<i);					//7 segments always on
			PORTC	=	pin[i];					//print hours, mins and seconds
			_delay_ms(3);
		}
	}
}
