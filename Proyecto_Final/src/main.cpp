#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4); // LCD 20x4 con dirección 0x27
const int Clock = 2;  
const int Data = 3;
const int Switch = 4;
int menu_Actual = 0;
int estado_ActualClock;
int estado_PrevioClock;
float peso_Contador = 0.1; // Kilogramos
String peso_lcd = "0.1";

void mostrar_Menu(){
  lcd.clear();
  switch (menu_Actual) {
    case 0:
      lcd.setCursor(5,0);
      lcd.print("Bienvenido");
      lcd.setCursor(0,1);
      lcd.print("Peso de tu mascota <");
      lcd.setCursor(3,2);
      lcd.print("Comidas al dia");
      lcd.setCursor(6,3);
      lcd.print("Avanzado");
      break;
    case 1:
      lcd.setCursor(5,0);
      lcd.print("Bienvenido");
      lcd.setCursor(0,1);
      lcd.print("Peso de tu mascota");
      lcd.setCursor(3,2);
      lcd.print("Comidas al dia  <");
      lcd.setCursor(6,3);
      lcd.print("Avanzado");
      break;
    case 2:
      lcd.setCursor(5,0);
      lcd.print("Bienvenido");
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
  pinMode(Clock, INPUT);
  pinMode(Data, INPUT);
  pinMode(Switch, INPUT_PULLUP); // Pull-up interno para el botón

  estado_PrevioClock = digitalRead(Clock);
  
  lcd.init();
  lcd.backlight();
  mostrar_Menu();
}

void loop() {
  estado_ActualClock = digitalRead(Clock); // Lectura del Clock

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
      mostrar_Menu(); // Actualiza la pantalla solo cuando cambia el menú
    }
  }

  // **Botón presionado (para entrar al menú 3)**
  if (digitalRead(Switch) == HIGH && menu_Actual == 0) {
    menu_Actual = 3;
    mostrar_Menu();
    delay(300);  // Evita doble detección del botón
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
      mostrar_Menu(); // Actualiza solo cuando cambia el peso
    }
  }

  // **Salir del menú 3 con el botón**
  if (menu_Actual == 3 && digitalRead(Switch) == HIGH) {
    menu_Actual = 0;
    mostrar_Menu();
    delay(300); // Evita rebotes
  }

  estado_PrevioClock = estado_ActualClock; // Guarda el estado anterior
}
