#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4); //set the LCD address to 0x27 for a 16 chars and 2 line display (Hace cosas)
const int pulsadorUp = 4; //Se asignan los pines adeucados a los pulsadores
const int pulsadorSelect = 2;
const int pulsadorDown = 3;
int menu_Actual = 0 ; //Variable para llevar trazo del menú actal
bool botonUpPresionado = false ;
bool botonDowmPresionado = false;

void mostrar_Menu(){
  lcd.clear();
  switch (menu_Actual)
  {
  case 0:
    lcd.setCursor(5,0);                   //Se elige dónde iniciar el texto
    lcd.print("Bienvenido");              //Mensaje en la primera línea
    lcd.setCursor(0,1);                   //Se elige dónde iniciar la segunda línea
    lcd.print("Peso de tu mascota <");
    lcd.setCursor(3,2);
    lcd.print("Comidas al dia");
    lcd.setCursor(6,3);
    lcd.print("Avanzado");
    break;
  case 1:
    lcd.setCursor(5,0);                   //Se elige dónde iniciar el texto
    lcd.print("Bienvenido");              //Mensaje en la primera línea
    lcd.setCursor(0,1);                   //Se elige dónde iniciar la segunda línea
    lcd.print("Peso de tu mascota");
    lcd.setCursor(3,2);
    lcd.print("Comidas al dia  <");
    lcd.setCursor(6,3);
    lcd.print("Avanzado");
    break;
  case 2:
    lcd.setCursor(5,0);                   //Se elige dónde iniciar el texto
    lcd.print("Bienvenido");              //Mensaje en la primera línea
    lcd.setCursor(0,1);                   //Se elige dónde iniciar la segunda línea
    lcd.print("Peso de tu mascota");
    lcd.setCursor(3,2);
    lcd.print("Comidas al dia");
    lcd.setCursor(6,3);
    lcd.print("Avanzado     <");
    break;
  case 3:
    lcd.clear();
    lcd.setCursor(5,0);
    lcd.print("Peso en Kg");
    //Aquí debería ir el contador  
    break;
  }
};

void setup() {
  pinMode(pulsadorUp, INPUT);//Se asigna el modo de función de los pines que usarán los pulsadores
  pinMode(pulsadorSelect, INPUT);
  pinMode(pulsadorDown, INPUT);

  lcd.init(); //Se inicia la pantalla lcd
  lcd.backlight();//Mejora la visibilidad del texto

  mostrar_Menu(); //Muestra el menú inicial
}

void loop() {
  if (digitalRead(pulsadorUp) == HIGH ){
    if(menu_Actual == 0){
      menu_Actual = 2 ; 
    }else if(menu_Actual <= 2 && menu_Actual > 0){
      menu_Actual -= 1 ;
    }
    mostrar_Menu(); //Se llama la función que muestra los menús
    delay(200); //Evita rebotes
  };
  if (digitalRead(pulsadorDown)==HIGH){
    if(menu_Actual == 2){
      menu_Actual = 0;
    }
    else if(menu_Actual >= 0 && menu_Actual < 2){
      menu_Actual +=  1;
    }
    mostrar_Menu();
    delay(200);
  }
  if(digitalRead(pulsadorSelect) == HIGH && menu_Actual == 0){
    menu_Actual = 3;
    mostrar_Menu();
    delay(200);
  }
  if(digitalRead(pulsadorDown)==HIGH && digitalRead(pulsadorUp)==HIGH){
    menu_Actual = 0;
    mostrar_Menu();
    delay(200);
  }
  
};

