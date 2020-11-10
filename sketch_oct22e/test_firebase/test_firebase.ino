#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>

#define FIREBASE_HOST "a5017-xudm-99ed9.firebaseio.com/"                         // the project name address from firebase id
#define FIREBASE_AUTH "HVebvmJHN2zLU0ePMusTpd8oYyroBqK1oMA9Z1So"  
#define WIFI_SSID "V50S"
#define WIFI_PASSWORD "dlskwmak11"

#define P40 D1    
#define P41 D2
#define P42 D5    //D3핀이 인식이 안되서 D5로 대체
#define P43 D4

String value_firebase = "";
int PB0, PB1, PB2, PB3, i;
int result_firebase[4] = {0,0,0,0};

//Define FirebaseESP8266 data object
FirebaseData firebaseData;
FirebaseJson json;

void printResult(FirebaseData &data);
  String path = "/PLC";

void setup()
{
  Serial.begin(115200);
  pinMode(P40,INPUT);
  pinMode(P41,INPUT);
  pinMode(P42,INPUT);
  pinMode(P43,INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  firebaseData.setBSSLBufferSize(1024, 1024);

  //Set the size of HTTP response buffers in the case where we want to work with large data.
  firebaseData.setResponseSize(1024);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
}

void printResult(FirebaseData &data)
{

    if (data.dataType() == "json")
    {
        Serial.println();
        FirebaseJson &json = data.jsonObject();
        //Print all object data
        Serial.println("Pretty printed JSON data:");
        String jsonStr;
        json.toString(jsonStr,true);
        Serial.println(jsonStr);
        Serial.println();
        Serial.println("Iterate JSON data:");
        Serial.println();
        size_t len = json.iteratorBegin();
        String key = ""; 
        String value = value_firebase;
        int type = 0;
        for (size_t i = 0; i < len; i++)
        {
            json.iteratorGet(i, type, key, value);
            Serial.print(i);
            Serial.print(", ");
            Serial.print("Type: ");
            Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
            if (type == FirebaseJson::JSON_OBJECT)
            {
                Serial.print(", Key: ");
                Serial.print(key);
            }
            Serial.print(", Value: ");
            Serial.println(value);
        }
        json.iteratorEnd();
    }
   
}

void loop()
{
  if(digitalRead(P40) == LOW)
  {
    i=0;
    PB0 ++;
    if(PB0 == 1){
    result_firebase[0] += 1;
    if(result_firebase[0] == 2)
    {
    result_firebase[0] = 0;
    }
     Serial.println(result_firebase[0]);
    }
  } 
  if(digitalRead(P40) == HIGH)
  {
  PB0 = 0;
  }

  if(digitalRead(P41) == LOW)
  {
    i=1;
    PB1 ++;
    if(PB1 == 1){
    result_firebase[1] += 1;
    if(result_firebase[1] == 2)
    {
    result_firebase[1] = 0;
    }
     Serial.println(result_firebase[1]);
    }
  } 
  if(digitalRead(P41) == HIGH)
  {
  PB1 = 0;
  }

  if(digitalRead(P42) == LOW)
  {
    i=2;
    PB2 ++;
    if(PB2 == 1){
    result_firebase[2] += 1;
    if(result_firebase[2] == 2)
    {
    result_firebase[2] = 0;
    }
     Serial.println(result_firebase[2]);
    }
  } 
  if(digitalRead(P42) == HIGH)
  {
  PB2 = 0;
  }

  if(digitalRead(P43) == LOW)
  {
    i=3;
    PB3 ++;
    if(PB3 == 1){
    result_firebase[3] += 1;
    if(result_firebase[3] == 2)
    {
    result_firebase[3] = 0;
    }
     Serial.println(result_firebase[3]);
    }
  } 
  if(digitalRead(P43) == HIGH)
  {
  PB3 = 0;
  }

  

  Serial.println("------------------------------------");
  Serial.println("Update test...");

if(PB0 == LOW ||PB1 == LOW || PB2 == LOW || PB3 == LOW)
  {
    json.set("4" + String(i), result_firebase[i]);
    if (Firebase.updateNode(firebaseData, path, json))
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.println("TYPE: " + firebaseData.dataType());
      //No ETag available
      Serial.print("VALUE: ");
      printResult(firebaseData);
      Serial.println("------------------------------------");
      Serial.println();
    }
  }

}
  /*
  if(Serial.available() == false)
    return;
  while (Serial.available()) {
  Serial.println("------------------------------------");
  Serial.println("Update test...");

  for (uint8_t i = 0; i < 4; i++)
  {
    json.set("4" + String(i), result_firebase[i]);
    if (Firebase.updateNode(firebaseData, path, json))
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.println("TYPE: " + firebaseData.dataType());
      //No ETag available
      Serial.print("VALUE: ");
      printResult(firebaseData);
      Serial.println("------------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
  }
  Serial.available() == false;
}*/
