//Andrés Lemus
#include <Wire.h> //Incluir librería para comunicación I2C
#include "config.h" //Incluir configuración del WiFi
#define I2C_DEV_ADDR 0x30 //Dirección del esclavo

uint8_t numero; //Variable para guardar numero de paquetes
float distancia; //Variable para guardar distancia
float temperatura; //Variable para guardar la temperatura

void onRequest(){ //Función para enviar los datos que pide el maestro
  Wire.print(i++); //Número de bytes
  Wire.print(" Packets.");
  Serial.println("onRequest");
}

void onReceive(int len){ //Función para recibir datos
  while(Wire.available()){ //Ejecutar cada que esté disponible
    distancia = Wire.read(); //Leer distancia
    numero = Wire.read(); //Leer número de paquetes
    temperatura = Wire.read(); //Leer la temperatura 
  }
}

AdafruitIO_Feed *temp = io.feed("temperatura"); //Indicar las feeds de Adafruit IO
AdafruitIO_Feed *dis = io.feed("distancia"); //Indicar las feeds de Adafruit IO
AdafruitIO_Feed *no = io.feed("no_de_paquetes"); //Indicar las feeds de Adafruit IO

void setup() {
  // put your setup code here, to run once:
  Wire.onReceive(onReceive); //Definir función para recibir
  Wire.onRequest(onRequest); //Definir función para enviar
  Wire.begin((uint8_t)I2C_DEV_ADDR); //Definir en modo esclavo y su dirección
  Serial.begin(115200); //Serial a 115200
  while(! Serial); 

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect(); //Función para conectar Adafruit IO

  // wait for a connection
  while(io.status() < AIO_CONNECTED) { 
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
}

void loop() {
  // put your main code here, to run repeatedly:
  io.run(); //Mantener Adafruit IO actualizando datos
  Serial.println(distancia); //Ver datos en el monitor serial
  Serial.println(numero); //Ver datos en el monitor serial
  Serial.println(temperatura); //Ver datos en el monitor serial
  
  temp->save(temperatura); //Enviar la variable temperatura a la feed de temperatura
  dis->save(distancia); //Enviar la variable distancia a la feed de distancia
  no->save(numero); //Enviar la variable numero de paquetes a la feed de no. de paquetes

  delay(5000); //Enviar cada 5s
}
