/*
 * initPwm.cpp
 *
 *  Created on: 18/giu/2015
 *      Author: massimo
 */

#include "Energia.h"

#include <msp430.h>
//#include <pins_energia.h>

void enablePWMTimer0(void){


	TA0CCTL0 = CCIE;                           			// CCR0 interrupt enabled
	TA0CTL = TASSEL_2 + MC_1 + ID_3 + TACLR;           // prescale SMCLK/8, upmode

	// imposto i registri del timer:

	// registro fine conteggio (periodo PWM) 3125000
	TA0CCR0 = 400; //NC 3125000 / 390 = 8000
	//TA2CCR1 = (start_duty/100)*(freq/PWM_f);
	TA0CCR2 = 0;	//0%
	TA0CCR1 = 0;
	TA0CCTL2 = OUTMOD_7 ;
	TA0CCTL1 = OUTMOD_7 ;


	// imposto P1.2 e P1.3 come uscite

	P1SEL |= BIT3 + BIT2;
	P1DIR |= BIT3 + BIT2;
}


///
/// PWM sul timer 2
void enablePWMTimer2(void){

	TA2CCTL0 = CCIE;                           			// CCR0 interrupt enabled
	TA2CTL = TASSEL_2 + MC_1 + ID_3 + TACLR;           // prescale SMCLK/8, upmode

	// imposto i registri del timer:

	// registro fine conteggio (periodo PWM) 3125000
	TA2CCR0 = 3125; //NC 3125000 / 3125 = 1000
	//TA2CCR1 = (start_duty/100)*(freq/PWM_f);
	TA2CCR2 = 0;	//0%
	TA2CCR1 = 0;
	TA2CCTL2 = OUTMOD_7 ;
	TA2CCTL1 = OUTMOD_7 ;

	// imposto P2.5 e P2.4 come uscite del PWM per regolare la potenza dei motori stepper
	P2SEL |= BIT4 + BIT5;
	P2DIR |= BIT4 + BIT5;
}

/// inizializza i pin di abilitazione dei driver degli stepper
void initENstepper(void){
	enablePWMTimer2();
}

///
/// inizializza i fine corsa posti su P2.0 e P2.2
/// usa il pull up interno

void initFC(){
	//configure pin 5 as an input and enable the internal pull-up resistor
	pinMode(P2_0, INPUT_PULLUP);
	pinMode(P2_2, INPUT_PULLUP);
	/*P2DIR |= BIT0 + BIT2;
	P2REN |= BIT0 + BIT2;
	P2OUT |= BIT0 + BIT2;*/
}

///
/// imposta la potenza alla pinza
void powerPinza(int pwm){
	TA0CCR2 = 4 * pwm;
}

///
/// imposta la potenza del sollevamento della pinza
void powerSollPinza(int pwm){
	/// il comando della direzione arriva da P4_3 P4_0
	TA0CCR1 = 4 * pwm;
}

///
/// alza la pinza
void pinzaSu(){
	digitalWrite(P4_3, LOW);
	digitalWrite(P4_0, HIGH);
}

///
/// abbassa la pinza invertendo la polarità rispetto alla funzione precedente
void pinzaGiu(){
	digitalWrite(P4_3, HIGH);
	digitalWrite(P4_0, LOW);
}

void aprePinza(){
	digitalWrite(P3_7, LOW);
	digitalWrite(P8_2, HIGH);
}

void chiudePinza(){
	digitalWrite(P3_7, HIGH);
	digitalWrite(P8_2, LOW);
}
