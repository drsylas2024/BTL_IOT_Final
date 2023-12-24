
#include <SoftwareSerial.h>
#include<WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>

WebServer server(80);

//Thời gian delay gửi lên cloud = 1s
const float timeSend = 1;
//Thời gian mốc đo delay send dữ liệu
long last_send = 0;
// 19 - 3
// 21 - 2
//                      RX  TX
SoftwareSerial mySerial(19, 21); // Tạo cổng UART phần mềm trên chân (RX) và (TX)

//Hàm đo thời gian
bool isWait(long current, float seconds){
  return (int)(millis() - current) >= seconds * 1000;
}

//Hàm cảnh báo
//Khi nhận yêu cầu cảnh báo thì gửi sang cho arduino
void warn(){
  last_tone = millis();
  //Gửi cảnh báo
  mySerial.println("WARN");
  mySerial.flush();
  server.send(200, "text/plain", "OK");
}

//Hàm setup
void setup() {
  delay(2000);
  Serial.begin(9600);  
  mySerial.begin(9600);  
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

  server.on("/warn", warn);
  server.begin();
}

//Hàm xử lý dữ liệu
//"a|b|c|d|e" => a,b,c,d,e
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

//Hàm gửi dữ liệu lên cloud
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
  //code = 200 thành công
  //code 404 = ko tìm thấy
  //code -1 lỗi wifi
  //code 403 bị chặn

  //So sánh
  return codeDweet == 200;
}

//Ham loop
void loop() { 
  //Kiểm tra có dữ lieu gui den tu arduino ko
  if (mySerial.available()) {
    //Doc chuoi tu arduino
    String rcv = mySerial.readStringUntil('\n');
    //Loai bo ki tu dac biet dau chuoi
    //abcde      -> abcde
    rcv.trim();
    //In ra man hinh chuoi da nhan
    Serial.print("Received from Arduino Uno: ");
    Serial.println(rcv);
    //Kiem tra thoi gian delay
    if(isWait(last_send, timeSend)){
      last_send = millis();
      String stt = send2Cloud(rcv) ? "SUCCESS" : "FAILED";
      Serial.println("Send2Cloud : " + stt);
    }
  }
  //Handle server
  server.handleClient();
}
