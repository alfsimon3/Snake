
#include "ledDisplay.h"
#include "snakePiLib.h"
#include "teclado_TL04.h"
#include <stdbool.h>
#include <wiringPiSPI.h>
#include <string.h>

#define CHANNEL 0
#define SPI_FREQ 15000000 	//Frec. 15Mhz

#define DIN      10 		// GPIO 10
#define CLK      11 		// GPIO 11
#define CS       8 			// GPIO 8

unsigned char pantalla_ini []= {
	0b00000000,
	0b00110100,
	0b00110010,
	0b00000010,
	0b00000010,
	0b00110010,
	0b00110100,
	0b00000000,
	};

unsigned char pantalla_fin []= {
	0b00000000,
	0b00110010,
	0b00110100,
	0b00000100,
	0b00000100,
	0b00110100,
	0b00110010,
	0b00000000,
	};

int maxInUse = 1;          // Número de matrices que utilizamos
int ledState = LOW;        // Inicialización del LED

int e = 0;

// Definición de los registros en la MAX-7219
int max7219_reg_noop        = 0x00;	//Modo cascada
int max7219_reg_digit0      = 0x01;	//Columna 1
int max7219_reg_digit1      = 0x02;	//Columna 2
int max7219_reg_digit2      = 0x03;	//Columna 3
int max7219_reg_digit3      = 0x04;	//Columna 4
int max7219_reg_digit4      = 0x05;	//Columna 5
int max7219_reg_digit5      = 0x06;	//Columna 6
int max7219_reg_digit6      = 0x07;	//Columna 7
int max7219_reg_digit7      = 0x08;	//Columna 8
int max7219_reg_decodeMode  = 0x09;	//Modo decodificación
int max7219_reg_intensity   = 0x0a;	//Intensidad
int max7219_reg_scanLimit   = 0x0b;	//Modo selección de leds
int max7219_reg_shutdown    = 0x0c;	//Modo apagado
int max7219_reg_displayTest = 0x0f;	//Modo Test

//------------------------------------------------------
// PROCEDIMIENTOS DE INICIALIZACION DE LOS OBJETOS ESPECIFICOS
//------------------------------------------------------

// Necesario para interpretar el valor del registro y de la columna que queremos activar con maxAll
void putint(int data) {
  int i = 8;
  int mask;
  while(i > 0) {
    mask = 0x01 << (i - 1);      // Obtenemos la máscara de bit
    digitalWrite( CLK, LOW);
    if (data & mask) {
      digitalWrite(DIN, HIGH);// Enviamos 1
    } else {
      digitalWrite(DIN, LOW); // Enviamos 0
    }
    digitalWrite(CLK, HIGH);
    --i;                         // Continuamos con un bit menor
  }
}


// Método para imprimir enviar datos a la Matriz
void maxAll( int reg, int col) {
  int c = 0;
  digitalWrite(CS, LOW);
  for ( c =1; c<= maxInUse; c++) {
  putint(reg);             // Especifica el registro
  putint(col);             //((data & 0x01) * 256) + data >> 1); // Envía los datos
    }
  digitalWrite(CS,HIGH);
}

//Enciende el display de leds
void dispon() {
 maxAll(max7219_reg_shutdown, 0x01);               // Display on
}

//Apaga el display de leds
void dispoff() {
 maxAll(max7219_reg_shutdown, 0x00);              // Display off
}

void setup () {
	wiringPiSPISetup (CHANNEL, SPI_FREQ);
	pinMode(DIN, OUTPUT);
	pinMode(CLK,  OUTPUT);
	pinMode(CS,   OUTPUT);

  //Inicialización de la Max 7219
  maxAll(max7219_reg_displayTest, 0x00); // No display test
  maxAll(max7219_reg_scanLimit, 0x07);   // Todas las columnas en uso
  maxAll(max7219_reg_decodeMode, 0x00);  // Usamos una Matriz de LEDs (no mostrar dígitos)
  maxAll(max7219_reg_shutdown, 0x01);    // No en modo shutdown
  for (e=1; e<=8; e++) {                 // Apagamos todos los LEDs
    maxAll(e,0);
  }
  maxAll(max7219_reg_intensity, 0x01);  // Intensidad de los leds (low) (Rango: 0x00 a 0x0f)


  //Pinta la pantalla inicial con un delay de 0,5s (500ms)
  maxAll(max7219_reg_digit0, pantalla_ini[0]);
  delay(500);
  maxAll(max7219_reg_digit1, pantalla_ini[1]);
  delay(500);
  maxAll(max7219_reg_digit2, pantalla_ini[2]);
  delay(500);
  maxAll(max7219_reg_digit3, pantalla_ini[3]);
  delay(500);
  maxAll(max7219_reg_digit4, pantalla_ini[4]);
  delay(500);
  maxAll(max7219_reg_digit5, pantalla_ini[5]);
  delay(500);
  maxAll(max7219_reg_digit6, pantalla_ini[6]);
  delay(500);
  maxAll(max7219_reg_digit7, pantalla_ini[7]);
  delay(500);

  dispoff();
  for (e=1; e<=8; e++) {                 // Apagamos todos los LEDs
	    maxAll(e,0);
  }
}


// Envía la pantalla final a la matriz
void lose() {
	for (e=1; e<=8; e++) {
	    maxAll(e,0);
	  }
	  maxAll(max7219_reg_digit0, pantalla_fin[0]);
	  delay(500);
	  maxAll(max7219_reg_digit1, pantalla_fin[1]);
	  delay(500);
	  maxAll(max7219_reg_digit2, pantalla_fin[2]);
	  delay(500);
	  maxAll(max7219_reg_digit3, pantalla_fin[3]);
	  delay(500);
	  maxAll(max7219_reg_digit4, pantalla_fin[4]);
	  delay(500);
	  maxAll(max7219_reg_digit5, pantalla_fin[5]);
	  delay(500);
	  maxAll(max7219_reg_digit6, pantalla_fin[6]);
	  delay(500);
	  maxAll(max7219_reg_digit7, pantalla_fin[7]);
	  delay(500);
	for (e=1; e<=8; e++) {                 // Apaga todos los LEDSs
	    maxAll(e,0);
	  }
	dispoff();
}

void InicializaLedDisplay (TipoLedDisplay *led_display) {
	if (wiringPiSetupGpio () <0){
				printf("Unable to setup wiringPi\n");
				piUnlock(STD_IO_BUFFER_KEY);
				return;
			}
		dispoff();
		setup();
}

//------------------------------------------------------
// OTROS PROCEDIMIENTOS PROPIOS DE LA LIBRERIA
//------------------------------------------------------

void PintaPantallaPorTerminal (tipo_pantalla *p_pantalla) {
	int i=0, j=0;

	printf("\n[PANTALLA]\n");
	fflush(stdout);
	for(j=0;j<NUM_FILAS_DISPLAY;j++) {
		for(i=0;i<NUM_COLUMNAS_DISPLAY;i++) {
			printf("%d", p_pantalla->matriz[i][j]);
			fflush(stdout);
		}
		printf("\n");
		fflush(stdout);
	}
	fflush(stdout);
}

//--------------------------------------------------------------------------
// PROCEDIMIENTOS DE EXCITACIÓN Y TRANSMISIÓN DE DATOS A LA MATRIZ DE LEDS
//--------------------------------------------------------------------------

void ActualizaExcitacionDisplay (tipo_pantalla *p_pantalla) {
	  //dispon();
	  pinMode(DIN, OUTPUT);
	  pinMode(CLK,  OUTPUT);
	  pinMode(CS,   OUTPUT);

	int a, b, c, d, e, f, g, h;

	  //Inicialización de la Max 7219
	    maxAll(max7219_reg_displayTest, 0x00); // No display test
	    maxAll(max7219_reg_scanLimit, 0x07);   // Todas las columnas en uso
	    maxAll(max7219_reg_decodeMode, 0x00);  // Usamos una Matriz de LEDs (no mostrar dígitos)
	    maxAll(max7219_reg_shutdown, 0x01);    // No en modo shutdown
	    for (e=1; e<=8; e++) {                 // Apagamos todos los LEDs
	      maxAll(e,0);
	    }
	    dispon();
	    maxAll(max7219_reg_intensity, 0x01);  // Intensidad de los leds (low) (Rango: 0x00 a 0x0f)


	  /*
	a = 128*(p_pantalla->matriz[0][0])+ 64*(p_pantalla->matriz[1][0])+ 32*p_pantalla->matriz[2][0]+ 16*p_pantalla->matriz[3][0]+ 8*p_pantalla->matriz[4][0]+ 4*p_pantalla->matriz[5][0]+ 2*p_pantalla->matriz[6][0]+ p_pantalla->matriz[7][0];
	b = 128*(p_pantalla->matriz[0][1])+ 64*(p_pantalla->matriz[1][1])+ 12*p_pantalla->matriz[2][1]+ 16*p_pantalla->matriz[1][1]+ 8*p_pantalla->matriz[4][1]+ 4*p_pantalla->matriz[5][1]+ 2*p_pantalla->matriz[6][1]+ p_pantalla->matriz[7][1];
	c = 128*(p_pantalla->matriz[0][2])+ 64*(p_pantalla->matriz[1][2])+ 22*p_pantalla->matriz[2][2]+ 16*p_pantalla->matriz[2][2]+ 8*p_pantalla->matriz[4][2]+ 4*p_pantalla->matriz[5][2]+ 2*p_pantalla->matriz[6][2]+ p_pantalla->matriz[7][2];
	d = 128*(p_pantalla->matriz[0][3])+ 64*(p_pantalla->matriz[1][3])+ 32*p_pantalla->matriz[2][3]+ 16*p_pantalla->matriz[3][3]+ 8*p_pantalla->matriz[4][3]+ 4*p_pantalla->matriz[5][3]+ 2*p_pantalla->matriz[6][3]+ p_pantalla->matriz[7][3];
	e = 128*(p_pantalla->matriz[0][4])+ 64*(p_pantalla->matriz[1][4])+ 42*p_pantalla->matriz[2][4]+ 16*p_pantalla->matriz[4][4]+ 8*p_pantalla->matriz[4][4]+ 4*p_pantalla->matriz[5][4]+ 2*p_pantalla->matriz[6][4]+ p_pantalla->matriz[7][4];
	f = 128*(p_pantalla->matriz[0][5])+ 64*(p_pantalla->matriz[1][5])+ 52*p_pantalla->matriz[2][5]+ 16*p_pantalla->matriz[5][5]+ 8*p_pantalla->matriz[4][5]+ 4*p_pantalla->matriz[5][5]+ 2*p_pantalla->matriz[6][5]+ p_pantalla->matriz[7][5];
	g = 128*(p_pantalla->matriz[0][6])+ 64*(p_pantalla->matriz[1][6])+ 62*p_pantalla->matriz[2][6]+ 16*p_pantalla->matriz[6][6]+ 8*p_pantalla->matriz[4][6]+ 4*p_pantalla->matriz[5][6]+ 2*p_pantalla->matriz[6][6]+ p_pantalla->matriz[7][6];
	   */
	h = 128*(p_pantalla->matriz[0][0])+ 64*(p_pantalla->matriz[0][1])+ 32*p_pantalla->matriz[0][2]+ 16*p_pantalla->matriz[0][3]+ 8*p_pantalla->matriz[0][4]+ 4*p_pantalla->matriz[0][5]+ 2*p_pantalla->matriz[0][6]+ p_pantalla->matriz[0][7];
	g = 128*(p_pantalla->matriz[1][0])+ 64*(p_pantalla->matriz[1][1])+ 32*p_pantalla->matriz[1][2]+ 16*p_pantalla->matriz[1][3]+ 8*p_pantalla->matriz[1][4]+ 4*p_pantalla->matriz[1][5]+ 2*p_pantalla->matriz[1][6]+ p_pantalla->matriz[1][7];
	f = 128*(p_pantalla->matriz[2][0])+ 64*(p_pantalla->matriz[2][1])+ 32*p_pantalla->matriz[2][2]+ 16*p_pantalla->matriz[2][3]+ 8*p_pantalla->matriz[2][4]+ 4*p_pantalla->matriz[2][5]+ 2*p_pantalla->matriz[2][6]+ p_pantalla->matriz[2][7];
	e = 128*(p_pantalla->matriz[3][0])+ 64*(p_pantalla->matriz[3][1])+ 32*p_pantalla->matriz[3][2]+ 16*p_pantalla->matriz[3][3]+ 8*p_pantalla->matriz[3][4]+ 4*p_pantalla->matriz[3][5]+ 2*p_pantalla->matriz[3][6]+ p_pantalla->matriz[3][7];
	d = 128*(p_pantalla->matriz[4][0])+ 64*(p_pantalla->matriz[4][1])+ 32*p_pantalla->matriz[4][2]+ 16*p_pantalla->matriz[4][3]+ 8*p_pantalla->matriz[4][4]+ 4*p_pantalla->matriz[4][5]+ 2*p_pantalla->matriz[4][6]+ p_pantalla->matriz[4][7];
	c = 128*(p_pantalla->matriz[5][0])+ 64*(p_pantalla->matriz[5][1])+ 32*p_pantalla->matriz[5][2]+ 16*p_pantalla->matriz[5][3]+ 8*p_pantalla->matriz[5][4]+ 4*p_pantalla->matriz[5][5]+ 2*p_pantalla->matriz[5][6]+ p_pantalla->matriz[5][7];
	b = 128*(p_pantalla->matriz[6][0])+ 64*(p_pantalla->matriz[6][1])+ 32*p_pantalla->matriz[6][2]+ 16*p_pantalla->matriz[6][3]+ 8*p_pantalla->matriz[6][4]+ 4*p_pantalla->matriz[6][5]+ 2*p_pantalla->matriz[6][6]+ p_pantalla->matriz[6][7];
	a = 128*(p_pantalla->matriz[7][0])+ 64*(p_pantalla->matriz[7][1])+ 32*p_pantalla->matriz[7][2]+ 16*p_pantalla->matriz[7][3]+ 8*p_pantalla->matriz[7][4]+ 4*p_pantalla->matriz[7][5]+ 2*p_pantalla->matriz[7][6]+ p_pantalla->matriz[7][7];

	maxAll(max7219_reg_digit0, a);
	delay(1);

	maxAll(max7219_reg_digit1, b);
	delay(1);

	maxAll(max7219_reg_digit2, c);
	delay(1);

	maxAll(max7219_reg_digit3, d);
	delay(1);

	maxAll(max7219_reg_digit4, e);
	delay(1);

	maxAll(max7219_reg_digit5, f);
	delay(1);

	maxAll(max7219_reg_digit6, g);
	delay(1);

	maxAll(max7219_reg_digit7, h);
	delay(1);


	/*dispoff();
	  for (e=1; e<=8; e++) {                 // Apaga todos los leds
		    maxAll(e,0);
	  }*/
	  return;
}
