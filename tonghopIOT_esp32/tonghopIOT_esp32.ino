
#include <SoftwareSerial.h>
#include<WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>

WebServer server(80);


const float timeSend = 1;
long last_send = 0;
long last_tone = 0;
String gateway ;
//                      RX  TX
SoftwareSerial mySerial(19, 21); // Tạo cổng UART phần mềm trên chân (RX) và (TX)

void turn(int PIN, int value){
  digitalWrite(PIN,value);
}

bool isWait(long current, float seconds){
  return (int)(millis() - current) >= seconds * 1000;
}
float sinVal;
int toneVal;
void warn(){
  last_tone = millis();
  for(int x=0; x<180; x++){
      // convert degrees to radians then obtain value
      sinVal = (sin(x*(3.1412/180)));
      // generate a frequency from the sin value
      toneVal = 2000+(int(sinVal*1000));
      tone(5, toneVal);
      delay(2); 
  }  
  server.send(200, "text/plain", "OK");
}

void setup() {
  delay(2000);
  pinMode(5,OUTPUT);
  Serial.begin(9600);  
  while(!Serial){}
  mySerial.begin(9600);  
  Serial.print("Waiting for connect UART ");
  while(!mySerial){
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected !");
  const char* ssid = "uaali";
  const char* pwd = "12345678";
  WiFi.begin(ssid, pwd);
  Serial.begin(9600);
  Serial.print("Connecting ");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWifi Connected !");
  gateway = WiFi.gatewayIP().toString();
  Serial.println(gateway+"_");

  server.on("/warn", warn);
  server.begin();
}

double* split_to_array(String t){
  double* arr = new double[5];
  int idxarr = 0;
  int idx = t.indexOf('|',0);
  while(idx != -1){
    String number = t.substring(0, idx);
    int len = t.length();
    t = t.substring(idx + 1, len);
    idx = t.indexOf('|', 0);
    *(arr + idxarr++) = number.toDouble();
  }
  *(arr + idxarr) = t.toDouble();
  return arr;
}

bool send2Cloud(String data2Send){
  if(WiFi.status() != WL_CONNECTED) return false;
  // Sound|Humidity|Temperature|Light|Gas
  double* arr = split_to_array(data2Send);
  String url = "https://dweet.io/dweet/for/nhom1cntt1504?sound=" + String(*(arr)) + "&humidity=" + String(*(arr+1)) + "&temperature=" + String(*(arr+2)) + "&light=" + String(*(arr+3)) + "&gas=" + String(*(arr+4));
  //Serial.println(url);
  HTTPClient httpDweet;
  httpDweet.begin(url);
  int codeDweet = httpDweet.GET();
  httpDweet.end();
  return codeDweet == 200;
}

void loop() { 
  if (mySerial.available()) {
    String rcv = mySerial.readStringUntil('\n');
    rcv.trim();
    Serial.print("Received from Arduino Uno: ");
    Serial.println(rcv);
    if(isWait(last_send, timeSend)){
      last_send = millis();
      String stt = send2Cloud(rcv) ? "SUCCESS" : "FAILED";
      Serial.println("Send2Cloud : " + stt);
    }
  }
  if(isWait(last_tone, 3)){
    noTone(5);
  }
  server.handleClient();
}
