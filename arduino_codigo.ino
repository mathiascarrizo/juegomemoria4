#include <SoftwareSerial.h>

// Definición del módulo Bluetooth (RX en pin 8, TX en pin 9)
SoftwareSerial sw(8, 9); 

#define   BUZZER   7
#define   ENTRADA_A  13
#define   ENTRADA_B  12
#define   ENTRADA_C  11
#define   ENTRADA_D  10
#define   SALIDA_A   2
#define   SALIDA_B   3
#define   SALIDA_C   4
#define   SALIDA_D   5

int melodia[ ] = {262, 196, 196, 220, 196, 0, 247, 262};
int duracionNotas[] = {4, 8, 8, 4, 4, 4, 4, 4};
int nivelActual = 1;
int velocidad = 500;
const int NIVEL_MAX = 100;
int secuencia[NIVEL_MAX];
int secuenciaUsuario[NIVEL_MAX];
bool estado = false;
char comando = 'Z';

void setup() {
   sw.begin(9600); // Inicialización del Bluetooth
   Serial.begin(9600); // Comunicación serial para depuración
   
   pinMode(ENTRADA_D, INPUT);
   pinMode(ENTRADA_C, INPUT);
   pinMode(ENTRADA_B, INPUT);
   pinMode(ENTRADA_A, INPUT);
   pinMode(SALIDA_A, OUTPUT);
   pinMode(SALIDA_B, OUTPUT);
   pinMode(SALIDA_C, OUTPUT);
   pinMode(SALIDA_D, OUTPUT);
   digitalWrite(SALIDA_A, LOW);
   digitalWrite(SALIDA_B, LOW);
   digitalWrite(SALIDA_C, LOW);
   digitalWrite(SALIDA_D, LOW);
}

void loop() {
   // Revisión de comandos Bluetooth
   

   // Lógica del juego
   if (nivelActual == 1) {
      generaSecuencia();
      muestraSecuencia();
      leeSecuencia();
   } else {
      muestraSecuencia();
      leeSecuencia();
   }
   
}

void muestraSecuencia() {
   digitalWrite(SALIDA_A, LOW);
   digitalWrite(SALIDA_B, LOW);
   digitalWrite(SALIDA_C, LOW);
   digitalWrite(SALIDA_D, LOW);

   for (int i = 0; i < nivelActual; i++) {
      int salida = secuencia[i];
      int tono = 0;
      if (salida == SALIDA_A) tono = 200;
      if (salida == SALIDA_B) tono = 300;
      if (salida == SALIDA_C) tono = 400;
      if (salida == SALIDA_D) tono = 500;

      tone(BUZZER, tono);
      delay(200);
      noTone(BUZZER);

      digitalWrite(salida, HIGH);
      delay(velocidad);
      digitalWrite(salida, LOW);
      delay(200);
   }
}

void leeSecuencia() {
   // Depuración del comando recibido

      // Mapear los comandos a las entradas físicas
      
   int flag = 0;
   for (int i = 0; i < nivelActual; i++) {
      flag = 0;
      while (flag == 0) {
          if (sw.available()) {
            comando = sw.read(); // Leer comando enviado desde el celular
            Serial.print("Comando recibido: ");
            Serial.println(comando);
            if (comando == 'A') {
              estado = true;
              revisaBotonBluetooth(estado, SALIDA_A, 200, i);
              estado = false;
              flag = 1;
              }
            if (comando == 'B'){
              estado = true;
              revisaBotonBluetooth(estado, SALIDA_B, 300, i);
              flag = 1;
              estado = false;
            } 
            if (comando == 'C') {
              estado = true;
              revisaBotonBluetooth(estado, SALIDA_C, 400, i);
              flag = 1;
              estado = false;
              }
            if (comando == 'D'){
              estado = true;
              revisaBotonBluetooth(estado, SALIDA_D, 500, i);
              flag = 1;
              estado = false;
            }
          }
         if (revisaBoton(ENTRADA_D, SALIDA_D, 500, i)) flag = 1;
         if (revisaBoton(ENTRADA_C, SALIDA_C, 400, i)) flag = 1;
         if (revisaBoton(ENTRADA_B, SALIDA_B, 300, i)) flag = 1;
         if (revisaBoton(ENTRADA_A, SALIDA_A, 200, i)) flag = 1;
      }
   }
   secuenciaCorrecta();
}

bool revisaBoton(int entrada, int salida, int tono, int indice) {
   if (digitalRead(entrada) == LOW) {
      digitalWrite(salida, HIGH);
      tone(BUZZER, tono);
      delay(300);
      noTone(BUZZER);
      secuenciaUsuario[indice] = salida;
      delay(200);
      digitalWrite(salida, LOW);

      if (secuenciaUsuario[indice] != secuencia[indice]) {
         secuenciaError();
         return true;
      }
      return true;
   }
   return false;
}

bool revisaBotonBluetooth(bool estado, int salida, int tono, int indice) {
    if (estado){
      digitalWrite(salida, HIGH);
      tone(BUZZER, tono);
      delay(300);
      noTone(BUZZER);
      secuenciaUsuario[indice] = salida;
      delay(200);
      digitalWrite(salida, LOW);
      comando = 'Z';
      if (secuenciaUsuario[indice] != secuencia[indice]) {
          secuenciaError();
          return true;
      }
      return true;
    }
    return false;
}

void generaSecuencia() {
   randomSeed(millis());
   for (int i = 0; i < NIVEL_MAX; i++) {
      secuencia[i] = random(2, 6);
   }
}

void secuenciaError() {
   digitalWrite(SALIDA_A, HIGH);
   digitalWrite(SALIDA_B, HIGH);
   digitalWrite(SALIDA_C, HIGH);
   digitalWrite(SALIDA_D, HIGH);
   delay(250);
   digitalWrite(SALIDA_A, LOW);
   digitalWrite(SALIDA_B, LOW);
   digitalWrite(SALIDA_C, LOW);
   digitalWrite(SALIDA_D, LOW);
   delay(250);
   melodiaError();
   nivelActual = 0;
   velocidad = 500;
}

void melodiaError() {
   for (int i = 0; i < 8; i++) {
      int duracionNota = 1000 / duracionNotas[i];
      tone(BUZZER, melodia[i], duracionNota);
      delay(duracionNota * 1.30);
      noTone(BUZZER);
   }
}

void secuenciaCorrecta() {
   if (nivelActual < NIVEL_MAX) nivelActual++;
   velocidad -= 50;
   delay(200);
}

// Función para simular entrada por Bluetooth
void simulaEntrada(int entrada) {
   pinMode(entrada, OUTPUT);
   digitalWrite(entrada, LOW);
   delay(50); // Simula el tiempo que se presiona un botón
   pinMode(entrada, INPUT);
}