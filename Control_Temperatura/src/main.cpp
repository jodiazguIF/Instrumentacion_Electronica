#include <Arduino.h>

// variables varias no sé
int lectura_Termistor = A0; // Seleccionamos el Pin A0 para leer los datos correspondientes al voltaje
int lectura_PinTermistor = 0;
int voltaje_Promedio = 0 ; 

// Variables para el PID
float kp = 1.2; //Ganancia Proporcional
float ki = 0.5; //Ganancia Integral
float kd = 0.7; //Ganancia Derivativo
float integral = 0;
float prevError = 0;
unsigned long prevTime;
float set_temperature = 25; //°C

void setup() {
  Serial.begin(9600);
  pinMode(5,OUTPUT);
  prevTime = millis();
}

void loop() {
  int voltaje_Termistor = 0 ; //Reinicio del valor del voltaje
  for (int i = 0; i < 51; i++){
    lectura_PinTermistor = analogRead(lectura_Termistor); // Se lee el valor en el pin A0
    voltaje_Termistor += 5*lectura_PinTermistor/255;      // Se convierte el valor digital a un valor numérico
  }
  voltaje_Promedio = voltaje_Termistor/50; //Se calcula el voltaje promedio de las lecturas de voltaje
  float temperatura_actual =  31.951 * voltaje_Promedio - 6.02; //Funcion para hallar la temperatura asumiendo B=3100 del termistor
  float error = set_temperature - temperatura_actual;  //Calcula el error
  // Calcular el tiempo entre ciclos
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - prevTime) / 1000.0;
  // Componentes del PID
  integral += error * deltaTime;                                  //Calculamos la integral
  float derivative = (error - prevError) / deltaTime;             //Calculamos la derivada
  float output = kp * error + ki * integral + kd * derivative;    //Calculamos la salida PID
  output = constrain(output, 0, 255);                             //Limitar salida al rango del PWM (0 a 255)
  analogWrite(5,int(output));                                     //Salida de PWM en función del PID
  //Actualizar las variables previas
  prevError = error;
  prevTime = currentTime;
  
  String dato = Serial.readString(); //lee el dato recibido
  set_temperature = dato.toFloat();  //Transforma el dato leído a un float y se lo asigna a la temperatura deseada


  Serial.println(temperatura_actual); // Envia un mensaje a Python con el valor de la temperatura
  delay(25);
}
  

// put function definitions here:
