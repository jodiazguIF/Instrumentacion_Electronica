#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4); //set the LCD address to 0x27 for a 16 chars and 2 line display (Hace cosas)

void setup() {
  lcd.init(); // Se inicia la pantalla lcd
  // Aquí se imprime un mensaje al LCD
  lcd.backlight();                      //Mejora la visibilidad del texto
  lcd.setCursor(5,0);                   //Se elige dónde iniciar el texto
  lcd.print("Bienvenido");              //Mensaje en la primera línea
  lcd.setCursor(0,1);                   //Se elige dónde iniciar la segunda línea
  lcd.print("Peso de tu mascota <");
  lcd.setCursor(3,2);
  lcd.print("Comidas al dia");
}

void loop() {
}