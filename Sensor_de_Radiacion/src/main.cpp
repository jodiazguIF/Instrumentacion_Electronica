#include <Arduino.h>
#include <analogWave.h> // Librería para generar las ondas análogas

// Parámetros de la onda
float frecuencia_Portadora = 1000;   // Frecuencia de la onda portadora en Hz
float frecuencia_Modulada = 100;     // Frecuencia de la onda modulada en Hz
const int tasa_Muestreo = 200;       // Tasa de muestreo
float t_delta = 1 / (frecuencia_Modulada * tasa_Muestreo); // Paso temporal
float amplitud_OndaPortadora = 1.0;  // Amplitud de la onda portadora
float amplitud_OndaModulada = 0.9;   // Amplitud de la onda modulada
uint16_t muestras[tasa_Muestreo - 1] = {0}; // Array de muestras

// Se declara la onda como variable global
static analogWave wave(DAC, muestras, tasa_Muestreo - 1, 0);

// Prototipo de función
void generateSamples(uint16_t* array);

void setup() {
  Serial.begin(9600);
  while (!Serial);  // Espera a que el puerto serial esté listo
  Serial.println("Arduino iniciado...");

  generateSamples(muestras); // Genera la señal AM
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);

  wave.begin(frecuencia_Portadora); // Se inicia la generación de la onda
  Serial.println("Generador de onda iniciado...");
}

void loop() {
  float input_OndaRecibida = analogRead(A1);
  float voltaje_OndaRecibida = input_OndaRecibida * (5.0 / 1023.0);
  float input_OndaGenerada = analogRead(A2);
  float voltaje_OndaGenerada = input_OndaGenerada * (5.0 / 1023.0);

  // Envío de datos en formato CSV para Python
  Serial.print(voltaje_OndaGenerada, 4);
  Serial.print(",");
  Serial.println(voltaje_OndaRecibida, 4);
  delay(5);  // Pequeño delay para evitar saturación del puerto serial
}

void generateSamples(uint16_t* array) {
  float tiempo = 0;
  float valores_Portadora = 0;
  float valores_Modulada = 0;
  float valores_SenalAM = 0;
  float amplitud_AM = amplitud_OndaModulada + amplitud_OndaPortadora;

  for (int i = 0; i < tasa_Muestreo - 1; i++) {
    valores_Portadora = amplitud_OndaPortadora * cos(2 * PI * frecuencia_Portadora * tiempo);
    valores_Modulada = amplitud_OndaModulada * cos(2 * PI * frecuencia_Modulada * tiempo);

    valores_SenalAM = (1 + valores_Modulada) * valores_Portadora + amplitud_AM;
    valores_SenalAM = valores_SenalAM / (2 * amplitud_AM); // Normalización
    valores_SenalAM = (valores_SenalAM * 1000 - 0) * (43253 - 11873) / (1000 - 0) + 11873;
    array[i] = (uint16_t) valores_SenalAM;  // Almacena la muestra

    tiempo += t_delta;
  }
}
