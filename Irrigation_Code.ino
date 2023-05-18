#define DHTPIN 0 // GPIO0 means D3 Pin on ESP8266
#define DHTTYPE DHT11

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"

char auth[] = "9KprzOQtK_lSHz1qtq9AB0bZsssuc2wz";
char ssid[] = "Mark 51";
char pass[] = "onetwo34";

DHT dht(DHTPIN, DHTTYPE);
int isRaining;
int isSomeoneNear;
float waterLevel;
float celciusTemperature;
float fahrenheitTemperature;
float humidity;
int flag[2] = {0, 0};

BLYNK_WRITE(V3) {
  if(flag[1] != 1){
    if(param.asInt() == 1){
      digitalWrite(D2, HIGH);
      Blynk.virtualWrite(V2, HIGH);
      Blynk.logEvent("pumping_started", "Water is Flowing.");
      flag[0] = 1;
    }else{
      digitalWrite(D2, LOW);
      Blynk.virtualWrite(V2, LOW);
      flag[0] = 0;
    }
  }
}

void setup(){
  Serial.begin(9600);

  pinMode(A0, INPUT); //Soil Sensor
  pinMode(D1, INPUT); //Rain Sensor
  pinMode(D2, OUTPUT); //Relay
  pinMode(D4, INPUT); // Motion Sensor

  dht.begin();
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
}

void loop(){
  Blynk.run();
  delay(2500);

  // waterLevel=100-(analogRead(A0)/18.50);
  // waterLevel=100-((analogRead(A0)-350)/4);
  waterLevel=100-((analogRead(A0)-200)/7.5);
  isRaining=!digitalRead(D1);
  isSomeoneNear=digitalRead(D4);
  celciusTemperature = dht.readTemperature();
  fahrenheitTemperature = dht.readTemperature(true);
  humidity = dht.readHumidity();

  if (isnan(humidity) || isnan(celciusTemperature) || isnan(fahrenheitTemperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  
  if(flag[0] != 1){
    if(isRaining && waterLevel < 50){
      digitalWrite(D2, HIGH);
      Blynk.virtualWrite(V2, HIGH);
      Blynk.logEvent("pumping_started", "Water is Flowing.");
      flag[1] = 1;
    }else if(isRaining && waterLevel >= 50){
      digitalWrite(D2, LOW);
      Blynk.virtualWrite(V2, LOW);
      flag[1] = 0;
    }else if(!isRaining && waterLevel < 50){
      digitalWrite(D2, HIGH);
      Blynk.virtualWrite(V2, HIGH);
      Blynk.logEvent("pumping_started", "Water is Flowing.");
      flag[1] = 1;
    }else{
      digitalWrite(D2, LOW);
      Blynk.virtualWrite(V2, LOW);
      flag[1] = 0;
    }
  }
  

  Serial.print("Water Level: "); Serial.println(waterLevel);
  Serial.print("Is Someone Near: "); Serial.println(isSomeoneNear);
  Serial.print("Is Raining: "); Serial.println(isRaining);
  Serial.print("Celcius Temperature: "); Serial.println(celciusTemperature);
  Serial.print("Fahrenheit Temperature: "); Serial.println(fahrenheitTemperature);
  Serial.print("Humidity: "); Serial.println(humidity);
  Serial.println();
  
  Blynk.virtualWrite(V0, waterLevel);
  Blynk.virtualWrite(V1, isRaining);
  Blynk.virtualWrite(V4, isSomeoneNear);
  Blynk.virtualWrite(V5, celciusTemperature);
  Blynk.virtualWrite(V6, fahrenheitTemperature);
  Blynk.virtualWrite(V7, humidity);
}