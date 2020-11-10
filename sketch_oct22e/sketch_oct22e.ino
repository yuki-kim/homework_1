#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <arduino-timer.h>
auto timer = timer_create_default(); // create a timer with default settings

#define FIREBASE_HOST "a5017-xudm-99ed9.firebaseio.com/"                     
#define FIREBASE_AUTH "HVebvmJHN2zLU0ePMusTpd8oYyroBqK1oMA9Z1So"  

//Define FirebaseESP8266 data object
FirebaseData firebaseData1;
FirebaseData firebaseData2;

unsigned long sendDataPrevMillis = 0;

String path = "/PLC";                //Firebase에서 검색할 이름
String F_Key;                        //Firebase에서 받을 PLC 번호                                
int firebase_Value[4] = {0,0,0,0};   //PLC에서의 출력값을 array로 저장
int pageUp = 0;
char emailID[45] = "APtest";         //입력한 WiFi 주소를 저장
char password[30] = "password";      //입력한 WiFi의 비밀번호를 저장

String sChipId="";
char cChipId[40]="";
ESP8266WebServer server(80);        //칩ID를 지정

String inputString = "";         // 받은 문자열

int sendText=0,error=0,count=0,countR=0;
int p4[8]={0},p0[10]={0},monit=0;   //html의 제어를 위해 사용할 변수

bool toggle_led(void *) {     //PLC 읽기모드 및 에러 체크
  if(monit==1) {
    String s = "\0";
    s +=char(5);
    s += "00RSS0104%PW0";
    s +=char(4);
    Serial.println(" ");
    Serial.println(s); 
    Serial1.print(s);
  }
  count++;
  if(count>2 && sendText==1) { //출력 에러체크
    error=1;
    count=0;
  }
  if(monit==1) { //모니터 에러체크
    if(countR>2)  //입력 에러체크
      error=1;
    else if (countR<2)
      error=0;
    countR++;
  }
  return true; 
}

void setup() {
  // call the toggle_led function every 1000 millis (1 second)
  timer.every(5000, toggle_led);    //5초마다 1번 작동

  Serial.begin(19200);             //아두이노 시리얼 모니터
  Serial1.begin(19200);            //PLC 시리얼 모니터
  //-------------------------------------------------------------------------------------------------
  connectWifi();
 
  //WiFiManager
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFiManager wm;

  if (!wm.autoConnect(cChipId)) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }
  //-------------------------------------------------------------------------------------------------
  Serial.println("connected... :)");

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/firebase", handleFirebase);
  server.on("/monit", handleMonit);
  server.on("/scan", handleScan);
  server.on("/wifi", handleWifi);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  DisplayPlc("00");
}

void connectWifi() {
  sChipId = "yuki-"+String(ESP.getChipId(),HEX);
  sChipId.toCharArray(cChipId,sChipId.length()+1);
  Serial.println(sChipId);                            //chipId 자동으로 생성

  WiFiManager wifiManager;
  // wifimanager를 통해 wifidirect 연결해서 ssid와 password 입력할 때 email 과 password 설정
  WiFiManagerParameter customEmailId("email_id","Email_ID", emailID, 50);
  WiFiManagerParameter customPassword("password","Password", password, 35);
  wifiManager.addParameter(&customEmailId);
  wifiManager.addParameter(&customPassword);
  // naming wifimanager

  wifiManager.autoConnect("Yuki_PLC");
  // 연결 성공했을 때 serial 포트로 알림
  Serial.println("Wifi connected !!!");
  
  strcpy(emailID, customEmailId.getValue());
  strcpy(password, customPassword.getValue());
   
  Serial.println(strcat(emailID, ""));
  Serial.println(strcat(password, ""));
  
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(emailID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                                      //print local IP address                                     //send initial string of led status

  //----------------------------firebase setup

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  firebaseData1.setBSSLBufferSize(1024, 1024);

  //Set the size of HTTP response buffers in the case where we want to work with large data.
  firebaseData1.setResponseSize(1024);


  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  firebaseData2.setBSSLBufferSize(1024, 1024);

  //Set the size of HTTP response buffers in the case where we want to work with large data.
  firebaseData2.setResponseSize(1024);

    if (!Firebase.beginStream(firebaseData1, path))
  {
    Serial.println("------------------------------------");
    Serial.println("Can't begin stream connection...");
    Serial.println("REASON: " + firebaseData1.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  Firebase.setStreamCallback(firebaseData1, streamCallback, streamTimeoutCallback);

}

void DisplayPlc(String s) {
  if(s.length()<=0)
    return;
  int i,j;
  j=0;
  for(i=0;i<8;i++) //초기화
    p0[i]=0;
  for(i=s.length()-1;i>=0;i--)
    p0[j++]=s.charAt(i)-0x30;
  GoHome();
}

// string hex를 string bin으로 전환
String HexToBin(String sIn) {
  //hex 문자를 비트문자로
  String s;
  char arr[50]="";
  sIn.toCharArray(arr,sIn.length()+1);
  s=strtol(arr, NULL, 16); //hex문자열을 dec문자열로
  s=String(s.toInt(), BIN); //dec문자열을 bin문자열로
  return s;
}

void loop() {
  timer.tick(); // tick the timer
  server.handleClient();
  serialEvent();  
}

void serialEvent() {
  if(Serial.available() == false)
    return;
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    Serial.print(inChar);
    // add it to the inputString:
    if(inChar==0x06)
      inputString="";
    inputString += inChar;
    if(inChar==0x03) {
      String s="";
      s=inputString.substring(12, 14);
      s=HexToBin(s);
      DisplayPlc(s);
      countR=0;
      checkReturn(inputString); //출력 메세지 에러체크
    }  
  }
}

bool checkReturn(String sI) {
  String s="";
  s=sI.substring(3, 6);
  if(sendText==1) {
    if( s.equals("WSS")) {
      error=0;
      sendText=0;
    }
    else
      error=1;
  }
}
