
/*
 * 				FILE PRINCIPALE DOVE EFFETUARE MODIFICHE
 * */

#include "Energia.h"

void setup();
void loop();
/// inizializza il pwm per i motori in continua.
void enablePWMTimer0(void);
/// inizializza il pwm pwr im otori passo passo
void initENstepper(void);
/// inizializza i fine corsa
void initFC();
/// solleva la pinza
void pinzaSu();
/// abbassa la pinza
void pinzaGiu();
/// regola la potenza per il sollevamento pinza
void powerSollPinza(int pwm);
/// regola la potenza della chiusura della pinza
void powerPinza(int pwm);
/// apre la pinza
void aprePinza();
/// chiude la pinza
void chiudePinza();

/// ritardo fatto empiricamente
void ritardo(volatile long int rit){
	while (rit > 0)
		rit--;
}

#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor


// initialize the stepper library on pins 8 through 11:
/// mappa i pin della scheda in modo da mandare la sequenza di avanzamento.
/// NOTA BENE: le classi vengono dichiarate GLOBALI in quanto le funzioni setup e loop non hanno parametri.
Stepper motore1(stepsPerRevolution, 7, 8, 9, 10);
/// maxz
Stepper motore2(stepsPerRevolution, 27, 28, 29, 30);

int stepCount = 0;         // number of steps the motor has taken

void setup() {
  // initialize the serial port:
  Serial.begin(9600);
  int motorSpeed;
  // map it to a range from 0 to 100:
  //int motorSpeed = map(sensorReading, 0, 1023, 0, 100);
  motorSpeed = 5;
  motore1.setSpeed(motorSpeed);
  motore2.setSpeed(motorSpeed);
  enablePWMTimer0();
  /// regola il pwm del motore passo passo. Questa funzione oltre ad abilitarlo
  /// lo pone a pontenza nulla (0%)
  initENstepper();
  /// imposta i pin per i motori della pinza e per il movimento dell stessa
  pinMode(P4_3, OUT);
  pinMode(P4_0, OUT);
  pinMode(P3_7, OUT);
  pinMode(P8_2, OUT);
  /// movimento verticale pinza: impostano la polarita' dell'alimentazione al motore, agendo su mezza parte
  /// dello L298.
  digitalWrite(P4_3, LOW);
  digitalWrite(P4_0, LOW);
  /// chiusura della pinza:  impostano la polarita' dell'alimentazione al motore, agendo su mezza parte
  /// dello L298.
  digitalWrite(P3_7, LOW);
  digitalWrite(P8_2, LOW);
  /// inizializza i fc mettendo le resteinze di pull-up
  initFC();
  pinzaSu();
  powerSollPinza(60);
  /// solleva la pinza
  while(digitalRead(P2_0));
  powerSollPinza(0);
  /// apre la pinza
  aprePinza();
  powerPinza(60);
  ///
  /// TODO:
  /// scrivere un ciclo di ritardo per permettere alla pinza di aprirsi.
  ritardo(1000000);
  powerPinza(0);
}

///
/*
 * 		CICLO PRINCIPALE
 * 		definizione dei pin: cfr. anche il file pins_energia.h
 * 		P3.2-P2.7-P4.2-P4.1 : FASI MOTORE STEPPER 1
 * 		P2.4: PWM SU EN_A, EN_B PER REGOLARE LA POTENZA DEL MOTORE ED EVITARE CHE VADA IN CORTO QUANDO E' FERMO
 *
 * 		P6.4-P7.0-P3.6-P3.5 :FASI MOTORE STEPPER 2
 * 		P2.5: PWM SU EN_A, EN_B PER REGOLARE LA POTENZA DEL MOTORE ED EVITARE CHE VADA IN CORTO QUANDO E' FERMO
 *
 * 		P4.3-P4.0 : polarita' del motore che muove la pinza. Vanno collegati a in1, in2 del driver L298.
 * 		se il movomento e' opposto a quanto comandato, vanno scambiati i due segnali
 * 		P1.2 imposta il pwm al motore che alza e abbassa la pinza e quindi regola la sua potenza. Va collegato al
 * 		pin EN_A del driver L298
 *
 * 		P3.7-P8.2 : polarita' del motore che apre la pinza. Controllare secondo la polarita' impostata.
 * 		P1.3 imposta il pwm al motore che apre e chiude la pinza. Va collegato al pin EN_B dello stesso driver ceh gestice
 * 		il motore di spostamento della pinza.
 *
 * 		P2.0: ingresso del fine corsa per la pinza in alto
 * 		P2.2: ingresso del fine corsa per la pinza in basso
 * 		Entrambi hanno un pull-up interno.
 *
 *
 * */

void loop() {

	volatile unsigned int stato = 0;

	switch(stato){

	case 0:
		////abbassa la pinza fino al fine corsa e la apre
		pinzaGiu();
		powerSollPinza(50);
		/// attende il fine corsa
		while(digitalRead(P2_2));
		/// spegne il motore
		powerSollPinza(0);
		stato = 1;
	break;

	case 1:
		/// stringe la pinza
		chiudePinza();
		/// con forza al 60%
		powerPinza(60);
		ritardo(500000);
		powerPinza(0);
		stato = 2;
	break;

	case 2:
		/// solleva la pinza a fine corsa
		pinzaSu();
		// potenza al 60%
		powerSollPinza(60);
		/// solleva la pinza
		while(digitalRead(P2_0));
		/// motore spento
		powerSollPinza(0);
		ritardo(1000);
		stato = 3;
	break;

	case 3:
		/// ruota il primo braccio di 90°
		motore1.powerMotor(60);
		motore2.powerMotor(40);
		/// il numero di passi per giro e' 200, lo faccio girare di 90° (200/4 = 50)
		motore1.step(50);
		ritardo(1000);
		stato = 4;
	break;


	case 4:
		/// ruota il secondo braccio di 90°
		motore1.powerMotor(40);
		motore2.powerMotor(60);
		/// il numero di passi per giro e' 200, lo faccio girare di 90° (200/4 = 50)
		motore2.step(50);
		stato = 5;
	break;

	case 5:
		/// abbassa la pinza fino al fc
		/// tiene in coppia i motori stepper ma limita la corrente.
		motore1.powerMotor(40);
		motore2.powerMotor(40);
		pinzaGiu();
		powerSollPinza(50);
		/// attende il fine corsa
		while(digitalRead(P2_2));
		/// spegne il motore
		powerSollPinza(0);
		stato = 6;

	break;

	case 6:
		/// apre la pinza
		/// apre la pinza
		aprePinza();
		powerPinza(60);
		///
		/// scrivere un ciclo di ritardo per permettere alla pinza di aprirsi.
		ritardo(1000000);
		powerPinza(0);
		stato = 7;
	break;

	case 7:
		/// solleva la pinza fino al fc
		pinzaSu();
		// potenza al 60%
		powerSollPinza(60);
		/// solleva la pinza fino al segnale del fine corsa
		while(digitalRead(P2_0));
		/// motore spento
		powerSollPinza(0);
		ritardo(1000);
		stato = 8;

	break;

	case 8:
		/// ruota il secondo braccio di -90
		motore1.powerMotor(40);
		motore2.powerMotor(60);
		/// il numero di passi per giro e' 200, lo faccio girare di 90° (200/4 = 50)
		motore2.step(-50);
		stato = 9;

	break;


	case 9:
		/// ruota il primo braccio di -90
		motore1.powerMotor(60);
		motore2.powerMotor(40);
		/// il numero di passi per giro e' 200, lo faccio girare di 90° (200/4 = 50)
		motore1.step(-50);
		ritardo(1000);
		stato = 10;
	break;


	case 10:
	//	torna allo stato 0
		/// tiene in coppia i motori stepper ma limita la corrente.
		motore1.powerMotor(40);
		motore2.powerMotor(40);
		ritardo(1000);
		stato = 0;
	break;

	}
}
