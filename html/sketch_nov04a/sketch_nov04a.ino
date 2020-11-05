#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define SSID_AP "yuki-AP" // for AP mode
#define PASSWORD_AP "00000000" // for AP mode
#define LED_PIN D2 //16 pin 1 of (D0)led 2

IPAddress local_ip(192, 168, 4, 1); //IP for AP mode
IPAddress gateway(192, 168, 4, 1); //IP for AP mode
IPAddress subnet(255, 255, 255, 0); //IP for AP mode
ESP8266WebServer server(80);

bool LEDstatus = LOW;
int set_mode;


void setup() { 
Serial.begin(9600);
Serial.println("Wifi Control");
pinMode(LED_PIN, OUTPUT);
WiFi.softAP(SSID_AP, PASSWORD_AP); // AP mode
WiFi.softAPConfig(local_ip, gateway, subnet);
server.on("/", HTTP_GET, handle_OnConnect);
server.on("/LEDon", HTTP_GET, handle_LEDon);
server.on("/LEDoff", HTTP_GET, handle_LEDoff);
//server.onNotFound(handle_NotFound);
server.begin();
Serial.println("ESP8266 web server started...");
}


// handle HTTP requests and arduino control
void loop() {
server.handleClient();
ardu_action();
}

// HTTP request: on connect
void handle_OnConnect() {
Serial.println("Client connected");
server.send(200, "text/html", SendHTML());
}

void handle_LEDon() {
set_mode = 0; Serial.println("LED on...");
server.send(200, "text/html", SendHTML());

}
void handle_LEDoff() {
set_mode = 1; Serial.println("LED off...");
server.send(200, "text/html", SendHTML());

}

void ardu_action() {
switch (set_mode) {
case 0: digitalWrite(LED_PIN,HIGH); break;

case 1: digitalWrite(LED_PIN,LOW); break;
  }
}

String SendHTML() {
String html = "<!DOCTYPE html>\n";
html += "<html>\n";
html += "<head>\n";
html += "<title>Wifi Control</title>\n";
html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
html += "<style>\n";
html += "html {font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
html += "body {margin-top: 10px;} h2 {color: #444444;margin: 10px auto 10px;} h4 {color: #444444;margin-bottom: 10px;}\n";
html += ".button {border: 5px solid green; color: white; width: 120px; height: 50px; margin: 5px; padding: 15px 15px; text-align: center; textdecoration: none; font-weight: bold; font-size: 20px; border-radius: 10px; box-shadow: 0 6px #999; cursor: pointer;}\n";

html += ".button-on {background-color: #700000;}\n";
html += ".button-off {background-color: #000070;}\n";
html += ".button:active {background-color: #3e8e41; box-shadow: 0 5px #666; transform: translateY(4px);}\n";
html += ".center {display: flex; justify-content: center; align-items: center; margin: 10px;}\n";
html += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
html += "</style>\n";
html += "</head>\n";
html += "<body>\n";
html += "<div align=\"center\">\n";
html += "<h2>Wifi Control</h2> <br>\n";

html += "<form method=\"GET\">\n";
html += "<input class=\"button button-on\" type=\"button\" value=\"on\" onclick=\"window.location.href='/LEDon'\">\n";
html += "<br><br>\n";
html += "<input class=\"button button-on\" type=\"button\" value=\"off\" onclick=\"window.location.href='/LEDoff'\">\n";
html += "<br><br>\n";
html += "</form>\n";
html += "</div>\n";
html += "</body>\n";
html += "</html>\n";
return html; } 

