//OCR0A = ( (25ms * 16,000,000) / (1024) )- 1
//3 diody co jakis okres czasu
#include <avr/io.h>
#include <avr/interrupt.h>

// Function prototypes
void setupTimer0();

// Global variables
volatile uint8_t counter1 = 0;
volatile uint8_t counter2 = 0;
volatile uint8_t counter3 = 0;

int main(void) {
    // Set PB0, PB1, and PB3 as output pins
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB3);

    // Setup Timer0
    setupTimer0();

    // Enable global interrupts
    sei();

    while (1) {
        // Main program loop
    }
}

void setupTimer0() {
    // Set Timer0 in CTC mode
    TCCR0A |= (1 << WGM01);

    // Set the prescaler to 1024
    TCCR0B |= (1 << CS02) | (1 << CS00);

    // Enable the compare match A interrupt
    TIMSK0 |= (1 << OCIE0A);

    // Set the compare match A value for the desired time intervals
    OCR0A = 25;    // 25ms  OCR0A = (25ms * 16,000,000) / (1024) - 1 zaokraglone w dol
}

// Interrupt service routine for Timer0 compare match A
ISR(TIMER0_COMPA_vect) {
    // Increment counters
    counter1++;
    counter2++;
    counter3++;

    // Check if it's time to toggle PB0
    if (counter1 == 1) {
        PORTB ^= (1 << PB0);    // Toggle PB0
        counter1 = 0;           // Reset counter
    }

    // Check if it's time to toggle PB1
    if (counter2 == 2) {
        PORTB ^= (1 << PB1);    // Toggle PB1
        counter2 = 0;           // Reset counter
    }

    // Check if it's time to toggle PB3
    if (counter3 == 4) {
        PORTB ^= (1 << PB3);    // Toggle PB3
        counter3 = 0;           // Reset counter
    }
}

//sterowanie 3 diodami
#include <avr/io.h>
#include <util/delay.h>
 
int main(void){
    TCCR0A = (1<<WGM00)|(1<<WGM01); //włączam tryb fast PWM
    TCCR0B = (1<<CS00)|(1<<CS02);   //włączam preskaler na 1024
    TCCR0A |= (1<<COM0A0)|(1<<COM0A1); //clear at top
    TCCR0A |= (1<<COM0B0)|(1<<COM0B1); //clear at top
 
    TCCR2A = (1<<WGM20)|(1<<WGM21); //włączam tryb fast PWM
    TCCR2B = (1<<CS20)|(1<<CS22);   //włączam preskaler na 1024
    TCCR2A |= (1<<COM2A0)|(1<<COM2A1); //clear at top
 
 
    OCR0A = 0; //ustaw wypełnienie impulsów
    OCR0B = 0;
    OCR2A = 0;
    DDRB = (1<<PB3)|(1<<PB4); //PB3 na wyjście
    DDRD = (1<<PD7);
    uint8_t i;
    for(;;){
        //rozjaśniaj
        for(i=0;i<255;i++){
            OCR0A = i;
            OCR0B = ~i;
            OCR2A = i;
            _delay_ms(30);
        }
        //ściemniaj
        for(i=255;i>0;i--){
            OCR0A = i;
            OCR0B = ~i;
            OCR2A = i;
            _delay_ms(30);
        }
    }
}

//NORMAL LED

//PA0 co 1Hz, PA1 co 30Hz
#include <avr/io.h>
#include <avr/interrupt.h>
 
volatile uint8_t licznik_przepelnien;
int main(void){
    TCCR0B = (1<<CS00)|(1<<CS02); //ustawiam prescaler na 1024
    DDRA = (1<<PA0)|(1<<PA1);   //ustaw PA0 i PA1 na wyjście
    TIMSK0 = (1<<TOIE0); //włączam przerwanie z przepełnienia timera0
    sei();      //włącza globalny system przerwań;
    for(;;){
        if(licznik_przepelnien>29) {
            PORTA ^= (1<<PA0);    //zmień stan końcówki PA0 na przeciwny
            licznik_przepelnien = 0;
        }
    }
}
 
ISR(TIMER0_OVF_vect){
    licznik_przepelnien++; //zliczam przepełnienia timera0
    PORTA ^= (1<<PA1);    //mrugaj druga LED
}

//CTC

//przerwanie
#include <avr/io.h>
#include <avr/interrupt.h>
volatile uint8_t przerwanie;
int main(void){
    TCCR0A = (1<<WGM01);  //ustawiam tryb CTC
    TCCR0B = (1<<CS00) | (1<<CS02); //ustawiam preskaler na 1024
    OCR0A = 77;
    TIMSK0 = (1<<OCIE0A); //włączam przerwanie z timera 0 w trybie CTC
    sei();      //włączam globalny system przerwań
    DDRA = (1<<PA0);
    for(;;){
        if(przerwanie){
            PORTA ^= (1<<PA0); // zmień stan diody na przeciwny
            przerwanie = 0;
        }
    }
}
 
ISR(TIMER0_COMPA_vect){
    przerwanie = 1;
}

//chuj wie co to robi
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main (void) 
{
	DDRA = (1<<PA0);
	PORTA = (1<<PA0);
	sei();

	TCCR0B |= (1<<CS02) | (1<<CS00);
	TCCR0A |= (1<<WGM01);    // TRYB CTC
	TIMSK0 |= (1<<1<<OCIE0A);
	OCR0A = 255;

	for(;;) 
	{
		for(uint8_t i = 0; i<255; i++) 
		{
			OCR0A = i;
			_delay_ms(1);
		}
		for(uint8_t i = 255; i>0; i--)
		 {
			OCR0A = i;
			_delay_ms(1);
		}
	}
	
}

ISR(TIMER0_COMPA_vect) 
{
	PORTA ^= (1<<PA0);
}

//PWM

//przycisk rozjasnianie i przyciemnianie
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define CH_0 (1<<PB3)
#define CH_1 (1<<PB4)


int main (void) {
	DDRB |= CH_0 | CH_1; //ustawiam wyjscia sygnalow PWM
	DDRA = 0;
	DDRA |= (1<<PA0) | (1<<PA1);



	//ustawenie TIM0
	TCCR0B |= (1<<CS02) | (1<<CS00); //ustawiam bity preskalera na 1024
	TCCR0A |= (1<<WGM00) | (1<<WGM01); //tryb pracy timera

	TCCR0A |= (1<<COM0A0) | (1<<COM0A1); //ustawiam zachowanie sie pinu wyjscia - KANAL A
	

	TCCR0A |= (1<<COM0B0) | (1<<COM0B1); //ustawiam zachowanie sie pinu wyjscia - KANAl B
	

	uint8_t i = 0;

	for(;;) {

		if(!(PINA & (PA0)))  // przycisk rozjaśniający
		{
			_delay_ms(80);
			if(!(PINA & (PA0)))
			{

					i++;
					OCR0A = i;
					_delay_ms(10);


			}
		}

		if(!(PINA & (PA1)))  // przycisk przyciemniający
		{
			_delay_ms(80);
			if(!(PINA & ~(PA1)))
			{
					i--;
					OCR0A = i;
					_delay_ms(10);


			}
		}

	}
}

//to samo tylko lepiej

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define CH_0 (1<<PB3)
#define CH_1 (1<<PB4)

#define KLAWISZ_PLUS   (1<<PB0)
#define KLAWISZ_PLUS_WCS  !(PINB & KLAWISZ_PLUS)

#define KLAWISZ_MINUS   (1<<PB1)
#define KLAWISZ_MINUS_WCS  !(PINB & KLAWISZ_MINUS)


int main (void) {
	DDRB |= CH_0 | CH_1; //ustawiam wyjścia sygnałó PWM


	//ustawenie TIM0
	TCCR0B |= (1<<CS02) | (1<<CS00); //ustawiam bity preskalera na 1024
	TCCR0A |= (1<<WGM00) | (1<<WGM01); //tryb pracy timera

	TCCR0A |= (1<<COM0A0) | (1<<COM0A1); //ustawiam zachowanie się pinu wyjścia - KANAŁ A
	TCCR0A |= (1<<COM0B0) | (1<<COM0B1); //ustawiam zachowanie się pinu wyjścia - KANAŁ B


	uint8_t jasnosc =128;
	OCR0A = jasnosc;

	for(;;) {

		if(KLAWISZ_MINUS_WCS) jasnosc--;
		if(KLAWISZ_PLUS_WCS) jasnosc++;
		_delay_ms(5);
		OCR0A = jasnosc;


	}
}

//to samo tylko ze jest gamma

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define CH_0 (1<<PB3)
#define CH_1 (1<<PB4)

#define KLAWISZ_PLUS   (1<<PB0)
#define KLAWISZ_PLUS_WCS  !(PINB & KLAWISZ_PLUS)

#define KLAWISZ_MINUS   (1<<PB1)
#define KLAWISZ_MINUS_WCS  !(PINB & KLAWISZ_MINUS)

uint8_t gamma[]= {0,16,32,64,96,128,192,255};
const uint8_t max_gamma = 8;

int main (void) {
	DDRB |= CH_0 | CH_1; //ustawiam wyjścia sygnałó PWM


	//ustawenie TIM0
	TCCR0B |= (1<<CS02) | (1<<CS00); //ustawiam bity preskalera na 1024
	TCCR0A |= (1<<WGM00) | (1<<WGM01); //tryb pracy timera

	TCCR0A |= (1<<COM0A0) | (1<<COM0A1); //ustawiam zachowanie się pinu wyjścia - KANAŁ A
	TCCR0A |= (1<<COM0B0) | (1<<COM0B1); //ustawiam zachowanie się pinu wyjścia - KANAŁ B


	uint8_t jasnosc = 5;
	OCR0A = gamma[jasnosc];

	for(;;) {

		if(KLAWISZ_MINUS_WCS)
			if(jasnosc > 0)  jasnosc--;
		if(KLAWISZ_PLUS_WCS)
			if(jasnosc < max_gamma)
				 jasnosc++;
		_delay_ms(5);
		OCR0A = gamma[jasnosc];


	}
}

//2x tam i spowrotem

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define CH_0 (1<<PB3)
#define CH_1 (1<<PB4)

int main (void) {
	DDRB |= CH_0 | CH_1; //ustawiam wyjścia sygnałó PWM
	
	//ustawenie TIM0
	TCCR0B |= (1<<CS02) | (1<<CS00); //ustawiam bity preskalera na 1024
	TCCR0A |= (1<<WGM00) | (1<<WGM01); //tryb pracy timera

	TCCR0A |= (1<<COM0A0) | (1<<COM0A1); //ustawiam zachowanie się pinu wyjścia - KANAŁ A
	OCR0A = 255;

	TCCR0A |= (1<<COM0B0) | (1<<COM0B1); //ustawiam zachowanie się pinu wyjścia - KANAŁ B
	OCR0B = 255;

	for(;;) {
		for(uint8_t i = 0; i<255; i++) {
			OCR0A = i;
			_delay_ms(1);
		}
		for(uint8_t i = 255; i>0; i--) {
			OCR0A = i;
			_delay_ms(1);
		}

		for(uint8_t i = 0; i<255; i++) {
				OCR0B = i;
				_delay_ms(1);
		}
		for(uint8_t i = 255; i>0; i--) {
				OCR0B = i;
				_delay_ms(1);
		}
	}
}

//chyba tu???????
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
 
#define CYFRA_1 ~(1<<PB0)
#define CYFRA_2 ~(1<<PB1)
#define CYFRA_3 ~(1<<PB2)
#define CYFRA_4 ~(1<<PB3)
 
volatile uint8_t sekunda,cnt;
 
void wyswietl(uint16_t wartosc){
    uint8_t cyfra[10]={0x81,0xF3,0x49,0x61,0x33,0x25,0x05,0xF1,0x01,0x21};
    uint8_t ty,dz,je;
    uint16_t se;
    se = wartosc%1000;
    ty = wartosc/1000;
    dz = se%100;
    se = se/100;
    je = dz%10;
    dz = dz/10;
 
    PORTB = CYFRA_1;            //pal cyfre 1
    PORTA = cyfra[ty] & ~(1<<PA0); //wyświetl cyfre z kropką
    _delay_ms(5);
 
    PORTB = CYFRA_2;            //pal cyfre 2
    PORTA = cyfra[se] & ~(1<<PA0); //wyświetl cyfre z kropką
    _delay_ms(5);
 
    PORTB = CYFRA_3;            //pal cyfre 3
    PORTA = cyfra[dz] & ~(1<<PA0); //wyświetl cyfre z kropką
    _delay_ms(5);
 
    PORTB = CYFRA_4;            //pal cyfre 4
    PORTA = cyfra[je] & ~(1<<PA0); //wyświetl cyfre z kropką
    _delay_ms(5);
};
 
int main(void){
    TCCR0A = (1<<WGM01);  //ustawiam tryb CTC
    TCCR0B = (1<<CS00) | (1<<CS02); //ustawiam preskaler na 1024
    OCR0A = 154;
    TIMSK0 = (1<<OCIE0A); //włączam przerwanie z timera 0 w trybie CTC
    sei();      //włączam globalny system przerwań
    DDRA = 255;
    DDRB = (1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3); //port B na wy
    uint16_t licznik=0;
    for(;;){
        if(sekunda){
            licznik++;
            sekunda = 0;
        }
        wyswietl(licznik);
    }
}
ISR(TIMER0_COMPA_vect){
    if(++cnt>99){
        sekunda = 1;    //ustawiam flagę sekund
        cnt = 0;    //zeruj zmienną cnt
    }
}