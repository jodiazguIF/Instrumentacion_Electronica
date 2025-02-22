#include <Arduino.h>
#include <analogWave.h> // Librería para generar las ondas análogas


// Se crea una instancia de tipo onda análoga, usando el pin DAC
const uint16_t frecuencia_Portadora = 1000;   //  Frecuencia de la onda portadora en Hz
const uint16_t frecuencia_Modulada = 100;     // Frecuencia de la onda modulada en Hz
const uint16_t tasa_Muestreo = 200;           // Same as analogWaveMod
float t_delta = 1/(frecuencia_Modulada*tasa_Muestreo); // Paso temperal entre muestras
float amplitud_OndaPortadora = 1.0; // Amplitud de la onda portadora
float amplitud_OndaModulada = 0.9; // Amplitude de la onda modulada
uint16_t muestras[tasa_Muestreo-1] = {0};  //Array que almacena las muestras de la señal AM 

void generateSamples(uint16_t* array){
  //Esta función calcula y almacena las muestras de la señal AM en el array muestras{}
  float tiempo = 0; //Controla el tiempo de muestreo
  float valores_Portadora = 0; // Contiene los valores dsicretos de la onda portadora
  float valores_Modulada = 0; // Contiene los valores dsicretos de la onda modulada
  float valores_SeñalAM = 0; // Contiene los valores dsicretos de la onda AM completa
  float amplitud_AM = amplitud_OndaModulada + amplitud_OndaPortadora;

  for(int i = 0; i < tasa_Muestreo-1; i++){
    valores_Portadora= amplitud_OndaPortadora*cos(2*PI*frecuencia_Portadora*tiempo);
    valores_Modulada = amplitud_OndaModulada*cos(2*PI*frecuencia_Modulada*tiempo);

    valores_SeñalAM = (1+amplitud_OndaModulada)*amplitud_OndaPortadora+amplitud_AM;// Muestreo de la señal AM
    valores_SeñalAM = valores_SeñalAM/(2*amplitud_AM); // Señal AM normalizada
    valores_SeñalAM = (valores_SeñalAM*1000 - 0) * (43253 - 11873) / (1000 - 0) + 11873;
    array[i] = (uint16_t) valores_SeñalAM;  // Almacena los valores de la señal AM en el buffer
    // Serial.println( array[i]);
    // delay(100);
    tiempo = tiempo + t_delta;
  }
}

void setup() {
  Serial.begin(115200);       // 115200 baudios para la comunicación serial
  generateSamples(muestras);  // Se llama la función que llena el array de muestras
}

void loop() {
  static analogWave wave(DAC, muestras,tasa_Muestreo-1,0); // DAD (Usa el DAC del micro), muestras (Apunt al buffer de la señal AM), tasa_Muestreo (Número de muestras en el Buffer) , 0 (Sin desplazamiento de fase) 
  wave.begin(frecuencia_Portadora); //Genera la onda AM a la frecuencia de la onda portadora
  int input_A1 = analogRead(A1);  //Se lee la señal demodulada del amplificador operacional
  float voltage_A1 = input_A1 * (5.0 / 1023.0); //Se convierte el input análogo que está entre 0-1023 a voltaje
  Serial.println(voltage_A1);   //Se envía al serial el valor de voltaje
  delay(5);
}

