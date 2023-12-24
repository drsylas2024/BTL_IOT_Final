#include "DHT.h"
#include <SoftwareSerial.h>


#define LIGHT_SENSOR 4
#define GAS_SENSOR A2
#define DHT_SENSOR 5
#define SOUND_SENSOR A3

const int DHTTYPE = DHT21;  
const int RX = 2;
const int TX = 3;
const float timeWaitLed = 0.15;
const float timeWaitSend = 0.3;

long last_send = 0;
long last_print = 0;
DHT dht(DHT_SENSOR, DHTTYPE);
SoftwareSerial mySerial(RX, TX); // Tạo cổng UART phần mềm trên chân (RX) và (TX)

bool isWait(long current, float seconds){
  return millis() - current >= seconds * 1000;
}

float sinVal;
int toneVal;
long last_tone;

void warn(){
  last_tone = millis();
  for(int x=0; x<180; x++){
      // convert degrees to radians then obtain value
      sinVal = (sin(x*(3.1412/180)));
      // generate a frequency from the sin value
      toneVal = 2000+(int(sinVal*1000));
      tone(8, toneVal);
      delay(2); 
  }  
}

void setup() {
  pinMode(8, OUTPUT);
  pinMode(SOUND_SENSOR, INPUT);    // Set sound sensor pin as an INPUT
  pinMode(LIGHT_SENSOR, INPUT_PULLUP);
  dht.begin();   //Start DHT sensor
  Serial.begin(9600);
  mySerial.begin(9600);
  delay(2000);
  Serial.println("Start");
}

float getSoundValue(){
  return analogRead(SOUND_SENSOR);
}

String getDHT(){
  float h = dht.readHumidity();    
  float t = dht.readTemperature(); 
  String s = "Nhiet do : " + String(t) + "\nDo am : " + String(h) + "\n";
  return s;             
}


float getMQ2(){
  return analogRead(GAS_SENSOR);
}

bool hasLight(){
  return digitalRead(LIGHT_SENSOR) == 0;
}

void loop() {
  if(isWait(last_print, timeWaitLed)){
    String s = ""; 
    s += "Sound : " + String(getSoundValue()) + "\n"; 
    s += getDHT();
    if(hasLight()){
      s += "Light : YES\n";
    }
    else{
      s += "Light : NO\n";
    }
    s += "Gas : " + String(getMQ2()) + "\n";
    Serial.println("DEBUG SERIAL : ON");
    Serial.println(s);
    last_print = millis();
  }
  
  if(isWait(last_send, timeWaitSend)){ 
    String send = "";
    send += String(getSoundValue()) + "|" ;
    send += String(dht.readHumidity()) + "|" + String(dht.readTemperature()) + "|" ;
    send += hasLight() ? "1|" : "0|"; 
    send += String(getMQ2());
    mySerial.println(send);
    mySerial.flush();
    Serial.println("Sent !");

    last_send = millis();
  }
  if(mySerial.available()){
    String rcv = mySerial.readStringUntil('\n');
    rcv.trim();
    if(rcv=="WARN") warn();
  }
  if(isWait(last_tone, 3)){
    noTone(8);  
  }
}
