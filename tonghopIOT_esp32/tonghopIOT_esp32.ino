
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

void warn(){
  last_tone = millis();
  mySerial.println("WARN");
  mySerial.flush();
  server.send(200, "text/plain", "OK");
}

void setup() {
  delay(2000);
  Serial.begin(9600);  
  mySerial.begin(9600);  
  Serial.print("Waiting for connect UART ");
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
  server.handleClient();
}
