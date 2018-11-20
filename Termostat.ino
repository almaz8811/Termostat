/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  You’ll need:
   - Blynk App (download from AppStore or Google Play)
   - ESP8266 board
   - Decide how to connect to Blynk
     (USB, Ethernet, Wi-Fi, Bluetooth, ...)

  There is a bunch of great example sketches included to show you how to get
  started. Think of them as LEGO bricks  and combine them as you wish.
  For example, take the Ethernet Shield sketch and combine it with the
  Servo example, or choose a USB sketch and add a code from SendData
  example.
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include "TM1637.h"


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "31f94ba459c14d2bbd24b8c90244aae7";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "QtechRouter-115e";
char pass[] = "Q6K7TGR5";
int tempSlider = 18;
int exSlider = 2;
int otoplen = 1;
int ventel = 0;
float h;
float t;


#define DHTPIN 2          // Назначить пин датчика температуры
// Расскомментировать строку с нужным датчиком
//#define DHTTYPE DHT11     // DHT 11
#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301
//Назначить порты дисплея
#define CLK 4
#define DIO 5
TM1637 disp(CLK, DIO);

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

BLYNK_WRITE(V10) //Слайдер температуры
{
  tempSlider = param.asInt();
  //Serial.println(tempSlider);
}

BLYNK_WRITE(V11) //Слайдер экспозиции
{
  exSlider = param.asInt();    
}

BLYNK_WRITE(V0) //Кнопка отопления
{
  otoplen = param.asInt();  
  
}

BLYNK_WRITE(V1) //Кнопка вентелятора
{
  ventel = param.asInt();  
  
}

//Эта функция отправляет данные датчика на 5 и 6 виртуальный пин, каждые несколько секунд
void sendSensor() {
  h = dht.readHumidity();
  t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, t);
  Blynk.virtualWrite(V6, h);
  
  disp.display(0,int(t)/10);
  disp.display(1,int(t)%10);

}

void relay() {
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  int tempMax = tempSlider + exSlider;
  int tempMin = tempSlider - exSlider;
  

  if (otoplen == 0 && ventel == 0){ //Off-Auto
    digitalWrite(12, LOW);
    Blynk.virtualWrite(V8, 0);
    if (t <= tempMin) {
    digitalWrite(14, HIGH);
    Blynk.virtualWrite(V9, 255);
  } else if (t >= tempMax) {
    digitalWrite(14, LOW);
    Blynk.virtualWrite(V9, 0);
  }
  } else if (otoplen == 1 && ventel == 0){ //Auto-Auto
    if (t <= tempMin) {
    digitalWrite(12, HIGH);
    digitalWrite(14, HIGH);
    Blynk.virtualWrite(V8, 255);
    Blynk.virtualWrite(V9, 255);
  } else if (t >= tempMax) {
    digitalWrite(12, LOW);
    digitalWrite(14, LOW);
    Blynk.virtualWrite(V8, 0);
    Blynk.virtualWrite(V9, 0);
  }
  } else if (otoplen == 1 && ventel == 1){ //Auto-On
    digitalWrite(14, HIGH);
    Blynk.virtualWrite(V9, 255);
    if (t <= tempMin) {
    digitalWrite(12, HIGH);
    Blynk.virtualWrite(V8, 255);
  } else if (t >= tempMax) {
    digitalWrite(12, LOW);
    Blynk.virtualWrite(V8, 0);
  }
  } else if (otoplen == 0 && ventel == 1) { //Off-On
    digitalWrite(12, LOW);
    Blynk.virtualWrite(V8, 0);
    digitalWrite(14, HIGH);
    Blynk.virtualWrite(V9, 255);
  }
  
//  if (t <= tempMin) {
//    digitalWrite(12, HIGH);
//    Blynk.virtualWrite(V8, 255);
//  } else if (t >= tempMax) {
//    digitalWrite(12, LOW);
//    Blynk.virtualWrite(V8, 0);
//  }
  //String s = String(tempMin) + " ℃ - " + String(tempMax) + " ℃";
  Blynk.virtualWrite(V3, String(tempMin) + " ℃ - " + String(tempMax) + " ℃");
  //Serial.println(digitalRead(4));
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  pinMode(12, OUTPUT);
  pinMode(14, OUTPUT);
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
  dht.begin();
  //Инициализация дисплея
  disp.init();  // инициализация
  disp.set(7);  // яркость, 0 - 7 (минимум - максимум)
  disp.point(0); // Отключить двоеточие
  Blynk.virtualWrite(V10, tempSlider);
  Blynk.virtualWrite(V11, exSlider);
  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensor);
  timer.setInterval(1000L, relay);



}

void loop()
{
  Blynk.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
  timer.run();
}
