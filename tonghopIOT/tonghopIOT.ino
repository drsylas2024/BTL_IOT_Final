#include "DHT.h"
#include <SoftwareSerial.h>


#define LIGHT_SENSOR 4
#define GAS_SENSOR A2
#define DHT_SENSOR 5
#define SOUND_SENSOR A3

const int DHTTYPE = DHT21;  
//Chân RX
const int RX = 2;
//Chân TX
const int TX = 3;
//Chân TX -> RX cái kia , RX -> TX bên kia
const float timeWaitSend = 0.3;

long last_send = 0;
long last_print = 0;
DHT dht(DHT_SENSOR, DHTTYPE);
SoftwareSerial mySerial(RX, TX); // Tạo cổng UART phần mềm trên chân (RX) và (TX)

//Hàm đo thời gian
//current_time - last_time >= delay*1000
bool isWait(long current, float seconds){
  return millis() - current >= seconds * 1000;
}

float sinVal;
int toneVal;
long last_tone;

//Hàm kêu chuông
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

//Hàm chạy 1 lần
void setup() {
  pinMode(8, OUTPUT);
  pinMode(SOUND_SENSOR, INPUT);    // Mode INPUT : cho phép nhận dữ liệu đầu vào
  pinMode(LIGHT_SENSOR, INPUT_PULLUP); //Input_PULLUP : cho phép nhận du liệu đầu vào
  dht.begin();   //Start DHT sensor
  //Baud rates : 9600
  Serial.begin(9600);//Chạy serial với mySerial
  mySerial.begin(9600);
  delay(2000);
  Serial.println("Start");
}

//Lấy giá trị âm thanh
float getSoundValue(){
  return analogRead(SOUND_SENSOR);
}

//Lấy giá trị độ ẩm nhiệt độ
String getDHT(){
  float h = dht.readHumidity();    
  float t = dht.readTemperature(); 
  String s = "Nhiet do : " + String(t) + "\nDo am : " + String(h) + "\n";
  return s;             
}

//Lấy giá trị cảm biến khí gas
float getMQ2(){
  return analogRead(GAS_SENSOR);
}

//Kiểm tra có ánh sáng ko
bool hasLight(){
  return digitalRead(LIGHT_SENSOR) == 0;
}

//Hàm chạy liên tục, main
void loop() {
  //In dữ liệu ra màn hình sau mỗi 0.3s
  if(isWait(last_print, 0.3)){
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
  
  //Gửi dữ liệu cho ESP32
  if(isWait(last_send, timeWaitSend)){ 
    String send = "";
    send += String(getSoundValue()) + "|" ;
    send += String(dht.readHumidity()) + "|" + String(dht.readTemperature()) + "|" ;
    send += hasLight() ? "1|" : "0|"; 
    send += String(getMQ2());
    //a|b|c|d|e
    //Gửi dữ liệu
    mySerial.println(send);
    mySerial.flush();
    Serial.println("Sent !");

    last_send = millis();
  }
  //Nhận tín hiệu cảnh báo
  if(mySerial.available()){
    String rcv = mySerial.readStringUntil('\n');
    rcv.trim();
    if(rcv=="WARN") warn();
  }
  //Hàm tắt chuông
  if(isWait(last_tone, 3)){
    //Tắt chuông
    noTone(8);  
  }
}
