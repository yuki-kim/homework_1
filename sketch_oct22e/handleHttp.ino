// html->string 변환사이트
// http://davidjwatts.com/youtube/esp8266/esp-convertHTM.html#
// https://tomeko.net/online_tools/cpp_text_escape.php?lang=en
String webHead="<html> <head> <meta name='viewport' content='width=device-width, initial-scale=1.0'/> <meta http-equiv='Content-Type' content='text/html;charset=utf-8' /> <style> body { background: #bfff00; font-family: \\\"Lato\\\", sans-serif; } .button { border: none; color: white; padding: 20px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; } .button-on {border-radius: 100%; background-color: #4CAF50;} .button-off {border-radius: 100%;background-color: #707070;} .button-ledon {border-radius: 100%; padding: 10px; font-size: 8px; margin: 0px 0px; background-color: #ff4500;} .button-ledoff {border-radius: 100%; padding: 10px; font-size: 8px; background-color: #707070;} </style> </head> <body> <h3 style='color:SteelBlue;'>&emsp;PLC 제어 <a href='https://www.youtube.com/channel/UCL4K4o03Ux7W-lu35RxB3uQ?view_as=subscriber' style='color:SteelBlue;'>雪 유튜브 이동</a> </h3>";
String webTail="\n<p><a href='/wifi'>네트웍공유기변경-누른후 와이파설정부터 다시하세요</a></p></body>\n</html>";
String webHeadM="<html> <head> <meta http-equiv='refresh' content='2'/> <meta name='viewport' content='width=device-width, initial-scale=1.0'/> <meta http-equiv='Content-Type' content='text/html;charset=utf-8' /> <style> body { background: #bfff00; font-family: \\\"Lato\\\", sans-serif; } .button { border: none; color: white; padding: 20px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; } .button-on {border-radius: 100%; background-color: #4CAF50;} .button-off {border-radius: 100%;background-color: #707070;} .button-ledon {border-radius: 100%; padding: 10px; font-size: 8px; margin: 0px 0px; background-color: #ff4500;} .button-ledoff {border-radius: 100%; padding: 10px; font-size: 8px; background-color: #707070;} </style> </head> <body> <h3 style='color:SteelBlue;'>&emsp;PLC 제어 <a href='https://www.youtube.com/channel/UCL4K4o03Ux7W-lu35RxB3uQ?view_as=subscriber' style='color:SteelBlue;'>雪 유튜브 이동</a> </h3>";

void handleRoot() {
  String s=""; 

  s+="<form action='/on'>개별출력 : ";
  for(int i=0;i<4;i++) {
    if(p4[i]==1) 
      s+="<button type='submit' name='button' value='"+(String)i+"' class='button button-on' >4"+(String)i+"</button></a>"; //off
    else 
      s+="<button type='submit' name='button' value='"+(String)i+"' class='button button-off' >4"+(String)i+"</button></a>"; //on
  }
  s+="<br>전체제어 : ";
  if(p4[4] == 1 || (p4[0] == 1 && p4[1] == 1 && p4[2] == 1 && p4[3] == 1))
    s+="<button type='submit' name='button' value='4' class='button button-on' >AllOn</button></a>"; //All off
  else
    s+="<button type='submit' name='button' value='4' class='button button-off' >AllOn</button></a>"; //All off

  if(p4[0] == 1 || p4[1] == 1 || p4[2] == 1 || p4[3] == 1) 
  s+="<button type='submit' name='button' value='5' class='button button-off' >AllOff</button></a>"; //All on
  else if(p4[5] == 1 || (p4[0] == 0 && p4[1] == 0 && p4[2] == 0 && p4[3] == 0))
  s+="<button type='submit' name='button' value='5' class='button button-on' >AllOff</button></a>"; //All on
  
  s+="</form>";

  String sm="";
  //sm+="<div style='background-color:LightSalmon;width: 300px;'>";
  sm+="입력";
  for(int i=0;i<6 ;i++) {
    //Serial.println(p0[i]);
    if(p0[i]==1) 
      sm+="<button class='button button-ledon' >"+(String)i+"</button></a>"; 
    else 
      sm+="<button class='button button-ledoff' >"+(String)i+"</button></a>"; 
  }
  //sm+="</div>";

  sm+="<br><form action='/monit'>모니터링";
  if(monit==1) 
    sm+="<button type='submit' class='button button-on' >on</button></a>"; //off
  else 
    sm+="<button type='submit' class='button button-off' >off</button></a>"; //on
  sm+="</form>";

  sm+="통신에러";
  if(error==1) 
    sm+="<button class='button button-ledon' ></button></a>";
  else 
    sm+="<button class='button button-ledoff' ></button></a>"; 

  sm+="<br><br>AP & IP :&emsp;"+sChipId+"&emsp;"+WiFi.localIP().toString();
  if(monit==1)
    server.send(200, "text/html", webHeadM+s+sm+webTail);
  else
    server.send(200, "text/html", webHead+s+sm+webTail);
}

void handleWifi() {
  WiFiManager wm;
  wm.resetSettings();
  wm.resetSettings();
  ESP.reset();
}

void handleMonit() {
  if(monit==1)
    monit=0;
  else
    monit=1;
  GoHome();
}

void GoHome() {
  String s,ipS;
  //IPAddress ip;
  ipS=toStringIp(WiFi.localIP());
  s="<meta http-equiv='refresh' content=\"0;url='http://"+ipS+"/'\">";
  server.send(200, "text/html", s);
}

void handleOn() {
  String s="";
  int no=server.arg("button").toInt();
  int num1;
  int clear1;
  if(p4[no]==1){
    p4[no]=0;
  }
  else {
    p4[no]=1;
  }
  if(no<4) {
    int out=p4[0]+p4[1]*2+p4[2]*4+p4[3]*8;
    s = "\0";
    s +=char(5);
    s += "00WSS0104%PW4000";
    s += String(out, HEX);
    s +=char(4);
    Serial.println(" ");
    Serial.println(s); 
    Serial1.print(s);
    sendText=1;
    error=0;
    }
   if(no==4) {
     for(clear1 = 0; clear1 <4; clear1 ++)
     {
      p4[clear1]=1;
     }
     if(p4[5]==1)
     {
      p4[5] = 0;
     }
    s = "\0";
    s +=char(5);
    s += "00WSS0104%PW4000F";
    s +=char(4);
    Serial.println(" ");
    Serial.println(s); 
    Serial1.print(s);
    sendText=1;
    error=0;
    }
    if(no == 5){
     for(clear1 = 0; clear1 <4; clear1 ++)
     {
      p4[clear1]=0;
     }
     if(p4[4]==1)
     {
      p4[4] = 0;
     }
    s = "\0";
    s +=char(5);
    s += "00WSS0104%PW40000";
    s +=char(4);
    Serial.println(" ");
    Serial.println(s); 
    Serial1.print(s);
    sendText=1;
    error=0;
   }
  GoHome();
}

void handleFirebase() {
if(pageUp == 1) {
  delay(10);
  p4[0] = firebase_Value[0]; 
  p4[1] = firebase_Value[1];
  p4[2] = firebase_Value[2];
  p4[3] = firebase_Value[3];
  pageUp = 0;  
  Serial.println("Send ok");
  GoHome();
}
}



void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleScan() {
  String s;
  s="{\"chipId\":\""+sChipId+"\",\"ip\":\""+WiFi.localIP().toString()+"\"}";
  server.send(200, "text/html", s);
}
