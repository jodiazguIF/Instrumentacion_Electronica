#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
 
// A4 SDA
// A5 SCL
LiquidCrystal_I2C lcd(0x27,20,4); // LCD 20x4 con dirección 0x27
const int Clock = 2;        //Pin Clock Encoder  
const int Data = 3;         //Pin Data Encoder
const int Switch = 4;       //Pin Switch Encoder
const int GiroMotor = 5;  //Control de giro del motor
const int pin_Galga = A6;
float lectura_galga = 0;
int menu_Actual = 0;        //Se usa para llevar rastro de los menús
int estado_ActualClock;     //Se utiliza para almacenar el estado actual del clock del encoder
int estado_PrevioClock;     //Se utiliza para almacenar el estado previo del clock del encoder y ser capaces de comparar un cambio de estado
float peso_Contador = 0.1;  //Variable que seguirá el rastro del peso de la mascota a lo largo del código  
String peso_lcd = "0.1";    //Variable auxiliar para poder hacer el display del peso cuando sea necesario
#define Intervalo 60000     //Se usa esta variable para llevar el minutero, recordar que en Python toca tiempo en millis()
int hora_Global = 0 ;       //Esta variable lleva el conteo de las horas del día
int minuto_Global = 0;      //Esta variable lleva el conteo de los minutos que pasan
unsigned long tiempo_Anterior = 0; //Se usa para llevar el conteo del tiempo

void actualizarHora(int &hora, int &minuto, unsigned long &tiempo_referencia) {
  // El & es para modificar tanto las variables locales de la función como las variables globales que se ingresan a la función, con eso se tiene mejor control DE LOCOS
  unsigned long tiempo_Actual = millis(); // Tiempo actual en ms

  if (tiempo_Actual - tiempo_referencia >= Intervalo) {
      minuto++; 
      tiempo_referencia = tiempo_Actual; // Se actualiza la referencia
  }

  if (minuto >= 60) { 
      minuto = 0;
      hora++;
  }

  if (hora >= 24) { 
      hora = 0;
  }
}

String obtener_HoraString(int hora, int minuto) {
  char buffer[6];  // "HH:MM" + '\0' //Crea un array de carácteres llamado buffer, tiene tamaño 6 debido a que se necesita el caracter nulo para inidicar el fin de la cadena
  sprintf(buffer, "%02d:%02d", hora, minuto);  //Llena buffer con el texto con formato %02d , número entero (d) con mínimo dos dígitos (02), que se rellena de "0" de ser necesario. Un ":" fijo también.
  return String(buffer); //Se devuelve un string
} 

void mostrar_Menu(String hora){
  lcd.clear();
  switch (menu_Actual) {
    case 0:
      lcd.setCursor(8,0);
      lcd.print(hora);
      lcd.setCursor(0,1);
      lcd.print("Peso de tu mascota <");
      lcd.setCursor(3,2);
      lcd.print("Config Hora");
      lcd.setCursor(6,3);
      lcd.print("Avanzado");
      break;
    case 1:
      lcd.setCursor(8,0);
      lcd.print(hora);
      lcd.setCursor(0,1);
      lcd.print("Peso de tu mascota");
      lcd.setCursor(5,2);
      lcd.print("Config Hora   <");
      lcd.setCursor(6,3);
      lcd.print("Avanzado");
      break;
    case 2:
      lcd.setCursor(8,0);
      lcd.print(hora);
      lcd.setCursor(0,1);
      lcd.print("Peso de tu mascota");
      lcd.setCursor(3,2);
      lcd.print("Comidas al dia");
      lcd.setCursor(6,3);
      lcd.print("Avanzado     <");
      break;
    case 3:
      lcd.setCursor(5,0);
      lcd.print("Peso en Kg");
      lcd.setCursor(9,1);
      lcd.print(peso_lcd);
      lcd.setCursor(1,3);
      lcd.print("Pulsa para elegir");
      break;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(Clock, INPUT);         // Se asigna el pin del Clock como Input
  pinMode(Data, INPUT);          // Se asigna el pin del Data como input
  pinMode(Switch, INPUT_PULLUP); // Pull-up interno para el botón, se asigna el pin del switch como input
  pinMode(pin_Galga, INPUT);
  estado_PrevioClock = digitalRead(Clock);  //Se verifica el estado actual del clock para tener una referencia adecuada para la primera medida
  lcd.init();       //Se inicia el protocolo de comunicación con la pantalla LCD
  lcd.backlight();  //Se enciende las luz de fondo
  mostrar_Menu(obtener_HoraString(hora_Global, minuto_Global)); //Se llama esta función para desplegar el menú principal por defecto al encender
}

void loop() {
  estado_ActualClock = digitalRead(Clock); // Lectura del Clock
  actualizarHora(hora_Global , minuto_Global , tiempo_Anterior);

  // **Rotación detectada** (Flanco de subida)
  if (estado_ActualClock != estado_PrevioClock && estado_ActualClock == HIGH) {
    if (menu_Actual <= 2) {  // Solo modifica si está en 0, 1 o 2
      if (digitalRead(Data) == HIGH) { 
        // **Giro en sentido horario (CW)**
        menu_Actual++;
        if (menu_Actual > 2) menu_Actual = 0;  // Vuelve a 0 después de 2
      } else {
        // **Giro en sentido antihorario (CCW)**
        if (menu_Actual > 0) {
          menu_Actual--;
        } else {
          menu_Actual = 2;  // Si está en 0, regresa a 2
        }
      }
      mostrar_Menu(obtener_HoraString(hora_Global, minuto_Global)); // Actualiza la pantalla solo cuando cambia el menú
    }
  }

  // **Botón presionado (para entrar al menú 3)**
  if (digitalRead(Switch) == HIGH && menu_Actual == 0) {
    menu_Actual = 3;
    mostrar_Menu(obtener_HoraString(hora_Global, minuto_Global));
    delay(10);  // Evita doble detección del botón
  }

  // **Manejo del peso en menú 3**
  if (menu_Actual == 3 && estado_ActualClock != estado_PrevioClock && estado_ActualClock == HIGH) {
    bool pesoCambiado = false;
    if (digitalRead(Data) == HIGH) { 
      if (peso_Contador > 0.1){
      peso_Contador -= 0.1; // Disminuye peso
      pesoCambiado = true;
      }
    } else { 
        peso_Contador += 0.1; // Aumenta peso
        pesoCambiado = true;
      }

    if (pesoCambiado) {
      peso_lcd = String(peso_Contador, 1); // Convierte a String con 1 decimal
      mostrar_Menu(obtener_HoraString(hora_Global, minuto_Global)); // Actualiza solo cuando cambia el peso
    }
  }

  // **Salir del menú 3 con el botón**
  if (menu_Actual == 3 && digitalRead(Switch) == HIGH) {
    menu_Actual = 0;
    mostrar_Menu(obtener_HoraString(hora_Global, minuto_Global));
    delay(10); // Evita rebotes
  }
  estado_PrevioClock = estado_ActualClock; // Guarda el estado anterior
  lectura_galga = 0;
  for (int i = 0; i < 50; i++){
    lectura_galga+=analogRead(pin_Galga);
  }
  lectura_galga = lectura_galga/50;
  lectura_galga = lectura_galga*5/1023;
  Serial.println(lectura_galga);
  delay(100);
}
