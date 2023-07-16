// IMPORTAMOS LAS BIBLIOTECAS
#include <IRremote.h>
#include <LiquidCrystal.h>
#include <Servo.h>

// DEFINIMOS PINES
#define SENSOR A0
#define ASPERSOR 10 
#define LED_ESTADO 11
#define LED_ALARMA 12

// SENSOR DE TEMPERATURA
int lecturaSensor;
int temperaturaActual;
int pin;

// CONTROL INFRA ROJO
int receptor = 3;
IRrecv recv(receptor);
unsigned long decodedData = 0;

// LCD
LiquidCrystal Lcd(8, 9, 4, 5, 6, 7);

// ASPERSOR
Servo aspersor;
int pos = 0;

// BUZZER
const int duracionAlarma = 2000;

// MENU
bool flagMenu = false;

// SETUP
void setup() {
	pinMode(LED_ESTADO, OUTPUT);
  	pinMode(LED_ALARMA, OUTPUT);
  	Lcd.begin(16, 2);
  	Serial.begin(9600);
  	recv.enableIRIn();
  	obtenerTemp(pin);
  	aspersor.attach(ASPERSOR);
}

// FUNCIONES

int obtenerTemp(int pin) {
  	/*
    Esta función:
    - Lee el pin en el que está conectado el sensor de temperatura
    - Realiza el mapeo, ajustando el valor leído a un rango específico de temperaturas
    - El resultado se almacena en la variable temperaturaActual
  	*/
  	lecturaSensor = analogRead(pin);
  	temperaturaActual = map(lecturaSensor, 20, 358, -40, 125);
  	return temperaturaActual;
}

void initAspersor() {
  	// Esta función se encarga del movimiento del aspersor
  	for (pos = 0; pos <= 180; pos += 1) {
    	aspersor.write(pos);
    	delay(15);
  	}
  	for (pos = 180; pos >= 180; pos -= 1) {
    aspersor.write(pos);
    delay(15);
  	}
}

void incendio() {
  	/*
    Esta función es llamada cada vez que la temperatura sobrepasa el límite de grados determinado 
    para cada estación y se encarga de alertarlo por el LCD y de activar los otros sistemas de alerta
  	*/
  	Lcd.setCursor(6, 1);
  	Lcd.print("INCENDIO       "); 
  	digitalWrite(LED_ALARMA,HIGH);
  	initAspersor();
  	digitalWrite(LED_ALARMA,LOW);
  
}


bool menu() {
  	/*
    Esta función se encarga de generar el menú principal 
  	*/
  	while (recv.decode() == 0) {
    	Lcd.setCursor(0, 0);
    	Lcd.print("PRESIONE 1-2-3-4");
  	}
  	flagMenu = true; // Utilizamos una flag para que el menu no se ejecute en todas las iteraciones del bucle 
}

void encender() {
  	/*
    Esta función se encarga de encender nuevamente el sistema si es apagado
    - Se puede encender manteniendo presionado cualquier boton del control 
  	*/
  	while (true) {
    	Lcd.clear();
    
    	// APAGAMOS EL LED DE ESTADO
    	digitalWrite(LED_ESTADO, LOW);

    	if (recv.decode()) {
      	decodedData = recv.decodedIRData.decodedRawData, HEX;
      	recv.resume();

      	if (decodedData == 0) {
        	decodedData = 1; // Le asignamos el valor 1 para que no se vuelva a ingresar en la función
        	flagMenu = false; // Cambiamos la bandera para ingresar al menú al volver a prender el sistema
        	break;
		}
	 }
   }
}

void mainFun() {
  	/*
    Este bloque en conjunto con el bucle principal se encargan del funcionamiento principal de nuestro código
    - Escribe las estaciones en el Lcd
    - Obtiene y escribe la temperatura en el Lcd
  	*/
  	if (flagMenu == false) {
    	menu();
  	}

  	// PRENDEMOS EL LED DE ESTADO
  	digitalWrite(LED_ESTADO, HIGH);

  	// LEEMOS LA TEMPERATURA Y LA ESCRIBIMOS EN EL LCD
  	temperaturaActual = obtenerTemp(SENSOR);
  	Lcd.setCursor(0, 1);  

  	// SI SE RECIBE ALGUNA SEÑAL SE GUARDA EN NUESTRA VARIABLE decodedData
  	if (recv.decode()) {
    	decodedData = recv.decodedIRData.decodedRawData, HEX;
    	recv.resume();
  	} 

  	// INVIERNO
  	if (decodedData == 3994140416) {
    	Lcd.setCursor(0, 0);
    	Lcd.print("INVIERNO        "); // Se printea en el Lcd con espacios para limpiar la estación anterior
    	Lcd.setCursor(0, 1);
    	Lcd.print(temperaturaActual);
    	Lcd.print("                "); //Limpiamos la pantalla 
      if (temperaturaActual > 50) {
        incendio();
      }    	
  	}

    // OTOÑO
    if (decodedData == 4010852096) {
        Lcd.setCursor(0, 0); 
        Lcd.print("OTONO           "); // Se printea en el Lcd con espacios para limpiar la estación anterior
        Lcd.setCursor(0, 1);
        Lcd.print(temperaturaActual);
        Lcd.print("                "); //Limpiamos la pantalla 
        if (temperaturaActual > 60) {
          incendio();
        }
    }  

    // PRIMAVERA
    if (decodedData == 3977428736) {
        Lcd.setCursor(0, 0); 
        Lcd.print("PRIMAVERA       "); // Se printea en el Lcd con espacios para limpiar la estación anterior
        Lcd.setCursor(0, 1);
        Lcd.print(temperaturaActual);
        Lcd.print("                "); //Limpiamos la pantalla 
        if (temperaturaActual > 70) {
          incendio();
        }
    }

    // VERANO
    if (decodedData == 3944005376) {
      Lcd.setCursor(0, 0); 
      Lcd.print("VERANO          "); // Se printea en el Lcd con espacios para limpiar la estación anterior
      Lcd.setCursor(0, 1);
      Lcd.print(temperaturaActual);
      Lcd.print("                "); //Limpiamos la pantalla 
      if (temperaturaActual > 80) {
          incendio();
        }   
    }

    if (decodedData == 4278238976) {
        encender();
    }  
}

// BUCLE PRINCIPAL
void loop() {
	Serial.println("VUELVE ACA");
  	mainFun();
}

