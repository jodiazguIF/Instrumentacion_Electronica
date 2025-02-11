#include <Arduino.h>
#include "Arduino_LED_Matrix.h"   // Include the LED_Matrix library
#include "frames.h.cpp"               // Include a header file containing some custom icons

ArduinoLEDMatrix matrix;          // Create an instance of the ArduinoLEDMatrix class


// variables varias no sé
float lectura_Termistor = A1; // Seleccionamos el Pin A0 para leer los datos correspondientes al voltaje
float lectura_PinTermistor = 0;
float voltaje_Promedio = 0 ; 

// Variables para el PID
float kp = 157; //Ganancia Proporcional
float ki = 0.3 ; //Ganancia Integral
float kd = 0; //Ganancia Derivativo
float integral = 0;
float prevError = 0;
unsigned long prevTime;
float set_temperature = 25; //°C

void setup() {
  Serial.begin(9600);
  matrix.begin();                 // Initialize the LED matrix
  pinMode(6,OUTPUT);
  matrix.loadFrame(LEDMATRIX_HEART_BIG);
}

void loop() {
  String dato = Serial.readString(); //lee el dato recibido
  set_temperature = dato.toFloat();  //Transforma el dato leído a un float y se lo asigna a la temperatura deseada
  float voltaje_Termistor = 0 ; //Reinicio del valor del voltaje
  for (int i = 0; i < 6; i++){
    lectura_PinTermistor = analogRead(lectura_Termistor); // Se lee el valor en el pin A0
    voltaje_Termistor += 5*lectura_PinTermistor/1024-0.242*5*lectura_PinTermistor/1024;      // Se convierte el valor digital a un valor numérico
  }
  voltaje_Promedio = voltaje_Termistor/5; //Se calcula el voltaje promedio de las lecturas de voltaje
<<<<<<< HEAD
  float temperatura_actual =  41.5532 * voltaje_Promedio - 6.85; //Funcion para hallar la temperatura asumiendo B=3100 del termistor
=======
  float temperatura_actual =  48.086 * voltaje_Promedio - 3.612; //Funcion para hallar la temperatura asumiendo B=3100 del termistor
>>>>>>> parent of 8848033 (Se corrige la curva para que tenga sentido con la temperatura. También se agrega el trabajo del proyecto final)
  float error = set_temperature - temperatura_actual+0.5;  //Calcula el error
  // Calcular el tiempo entre ciclos
  float deltaTime = 0.01;
  // Componentes del PID
  integral += error * deltaTime;                                  //Calculamos la integral
  float derivative = (error - prevError) / deltaTime;             //Calculamos la derivada
  float output = kp * error + ki * integral + kd * derivative ;    //Calculamos la salida PID
  output = constrain(output, 0, 255);                             //Limitar salida al rango del PWM (0 a 255)
  analogWrite(6,int(output));                                     //Salida de PWM en función del PID
  //Actualizar las variables previas
  prevError = error;  
  Serial.println(temperatura_actual); // Envia un mensaje a Python con el valor de la temperatura
  delay(10);
}