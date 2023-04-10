#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include <ESPmDNS.h>

WebServer server(80);

String authFailResponse = "Authentication Failed";

void wifiapSetup() {
  Serial.println("Configuring access point...");
  Serial.println("APssid");
  Serial.println(storageGetString("APssid"));

  Serial.println("APpassword");
  Serial.println(storageGetString("APpassword"));

  WiFi.softAP(storageGetString("APssid").c_str(), storageGetString("APpassword").c_str());

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  String dis = "";
  dis.concat("IP: ");
  dis.concat(myIP.toString());
  dis.concat("\n");
  dis.concat("SSID: ");
  dis.concat(storageGetString("APssid"));
  dis.concat("\n");
  dis.concat("PW: ");
  dis.concat(storageGetString("APpassword"));
  dis.concat("\n");
  logString = dis;


  server.on("/", configForm);
  server.on("/saveConfig", saveConfig);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("AP mode started");
}

unsigned long previousMillis = 0;
unsigned long interval = 30000;
void webserverSetup() {
  unsigned long currentMillis = millis();
  Serial.println("WiFissid");
  Serial.println(storageGetString("WiFissid"));
  Serial.println("WiFipassword");
  Serial.println(storageGetString("WiFipassword"));

  WiFi.mode(WIFI_STA);
  WiFi.begin(storageGetString("WiFissid").c_str(), storageGetString("WiFipassword").c_str());
  if (WiFi.status() != WL_CONNECTED) {
    blinkWiFiLoss();
    beepWiFiLoss();
    delay(1000);
  }
  //Wait for connection
  while (WiFi.status() != WL_CONNECTED && (currentMillis - previousMillis >= interval)) {

    //Serial.println(WiFi.status());
    blinkWiFiLoss();
    beepWiFiLoss();
    delay(1000);
    previousMillis = currentMillis;
  }

  Serial.println("");
  if (MDNS.begin(storageGetString("deviceName").c_str())) {
    Serial.println("MDNS responder started");
    Serial.println(storageGetString("deviceName").c_str());
  }

  server.on("/", handleRoot);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void webserverLoop() {

  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  while ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)) {
    //Serial.println(WiFi.status());
    blinkWiFiLoss();
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    delay(2000);
    previousMillis = currentMillis;
  }

  if (WiFi.status() == WL_CONNECTED) {
    blinkWiFiConnect();
    WiFiRSSI = WiFi.RSSI();
    myIP = WiFi.localIP().toString();

    Serial.print("Connected to ");
    Serial.println(storageGetString("WiFissid"));
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void clientLoop() {

  server.handleClient();
  // Serial.println("Client loop");
  delay(10);  // allow the cpu to switch to other tasks
}

void handleRoot() {

  char html[4150];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  int dir;
  if (WINDIR > 0) dir = WINDIR;
  else dir = 0;
  String relay1Text, relay2Text, relay3Text, relay4Text, setVar1Class, setVar2Class, Setvar3Class, setVar4Class, icon;
  relay1Text = "OFF";
  relay2Text = "OFF";
  relay3Text = "OFF";
  relay4Text = "OFF";

  if (relayIsOn(R1)) {
    setVar1Class = "alert";
    relay1Text = "ON";
  } else
    setVar1Class = "normal";

  if (relayIsOn(R2)) {
    setVar2Class = "alert";
    relay2Text = "ON";
  } else
    setVar2Class = "normal";

  if (relayIsOn(R3)) {
    Setvar3Class = "alert";
    relay3Text = "ON";
  } else
    Setvar3Class = "normal";

  if (relayIsOn(R4)) {
    setVar4Class = "alert";
    relay4Text = "ON";
  } else
    setVar4Class = "normal";


  snprintf(html, 4150,
           "<!DOCTYPE html><html><head> <meta charset=\"UTF-8\"/> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">  <meta http-equiv='refresh' content='3'/><style>.max-val span, h3{text-align: center}#pm25-num sup, body{font-family: Arial, Helvetica, sans-serif}html{width: 100vw; height: 100vh}body{margin: 0 30px; background: #001213; background: linear-gradient(126deg, #001213 0, #005157 100%%); color: #eee;}.val{font-size: 30px; display: block; margin-bottom: 10px}.pm-other .val{text-align: right}.max-val span{display: inline-block; background-color: #ffffff21; border-radius: 5px; padding: 5px; font-size: 10px; margin: 3px;}.alert{background-color: #00be8e !important}h1{font-size: 16px; margin: 0 0 3px; padding: 0; color: #89ffae}h3{font-size: 22px; margin-top: 0}*{box-sizing: border-box}.flex-container{display: flex; flex-direction: row; text-align: left}.flex-item-left{padding: 10px 20px 10px 0; flex: 50%%; border-right: 1px solid #ffffff52; position: relative; display: flex; align-items: center; justify-content: center; column-gap: 30px; row-gap: 30px;}.flex-item-right{padding: 10px 0 10px 20px; flex: 50%%;}.flex-item-right sup{font-size: 15px}#pm25-num{font-size: 100px; line-height: 100px}#pm25-num sup{font-size: 20px}.emoticon{font-size: 7rem; line-height: 9rem; filter: grayscale(0); right: 0; border: solid; border-radius: 100px; width: 180px; height: 180px; display: flex; justify-content: center; transform: rotate(%ddeg); margin-top: 0px; background-color: rgba(0, 0, 0, 0.5); transition-duration: 0.5s; transition-delay: 1s;}@media (max-width:660px){.emoticon{top: 0}.flex-container{flex-direction: column}.flex-item-left{padding: 10px 0; border-right: 0; border-bottom: 1px solid #ffffff52; justify-content: center; flex-wrap: wrap;}.flex-item-right{padding: 10px 0}}.container{max-width: 830px; margin: auto;}</style></head><body> <div class=\"container\"> <h3 style=\"display: flex;align-items: center;column-gap: 10px;justify-content: center;\"><span style='font-size:50px;filter: grayscale(1);'>&#10228;</span> %s</h3> <div class=\"flex-container\"> <div class=\"flex-item-left\"> <div style=\"text-align: center;\"> <p style=\"font-size:2rem; margin:0px 0;padding:0;\">N</p><div style=\"display:flex;flex-direction: row;flex-wrap: nowrap;align-items: center;column-gap: 5px;\"> <p style=\"font-size:2rem; margin:0px 0;padding:0;\">W</p><div class=\"emoticon\">↑</div><p style=\"font-size:2rem; margin:0px 0;padding:0;\">E</p></div><p style=\"font-size:2rem; margin:0px 0;padding:0;\">S</p></div><div class=\"pm25\" style=\"display: flex;flex-direction: row;justify-content: space-between;flex-wrap: nowrap;\"> <div class=\"\"> <h1>Acquisition value</h1> <div id=\"pm25-num\" class=\"val\">%d°</div><h1>Direction</h1> <span class=\"val\">%s</span> </div></div></div><div class=\"flex-item-right\"> <div class=\"\"> <div class=\"\" style=\"display: flex;flex-direction: column;\"> <div style=\"display: flex;flex-wrap: nowrap;justify-content: space-between;\"> <div class=\"relay-box\" style=\"text-align:center; padding-top:30px;\"> <div> <h1>RELAY1</h1> <span class=\"val\">%s</span> </div><div class=\"max-val\"> <span class=\"%s\">North ± %d°</span> </div></div><div class=\"relay-box\" style=\"text-align:center; padding-top:30px;\"> <div> <h1>RELAY2</h1> <span class=\"val\">%s</span> </div><div class=\"max-val\"> <span class=\"%s\">East ± %d°</span> </div></div></div></div></div><div class=\"\"> <div class=\"\" style=\"display: flex;flex-direction: column;\"> <div style=\"display: flex;flex-wrap: nowrap;justify-content: space-between;\"> <div class=\"relay-box\" style=\"text-align:center; padding-top:30px;\"> <div> <h1>RELAY3</h1> <span class=\"val\">%s</span> </div><div class=\"max-val\"> <span class=\"%s\">South ± %d°</span> </div></div><div class=\"relay-box\" style=\"text-align:center; padding-top:30px;\"> <div> <h1>RELAY4</h1> <span class=\"val\">%s</span> </div><div class=\"max-val\"> <span class=\"%s\">West ± %d°</span> </div></div></div></div></div></div></div><p style=\"text-align: center;\"><b>Domain:</b> %s.local</p></div></body></html>",

           dir, storageGetString("webTitle"), WINDIR, alarmlev.word, relay1Text, setVar1Class, setVar1, relay2Text, setVar2Class, setVar2, relay3Text, Setvar3Class, setVar3, relay4Text, setVar4Class, setVar4, storageGetString("deviceName"));
  server.send(200, "text/html", html);
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

void configForm() {
  char html[3000];
  snprintf(html, 3000,

           "<html>\
<head>\
<meta charset=\"UTF-8\" />\
<title>System configuration.</title>\
<style>\
   html{width:100%%;height:100%%}body{margin:0 30px;background:#005157;font-family:Arial,Helvetica,sans-serif;font-size:1.8rem;color:#eee}input:not([type=checkbox]){padding:20px;font-size:20px;width:100%%;margin:10px 0;font-weight:700;font-size:2rem;border:0;border-radius:5px;}legend h1{font-size:2.5rem;;color:#7fffd4}input[type=submit]{background-color:#32ab4e;}\
</style>\
</head>\
<body>\
<div style=\"width:95vw;margin:auto;\">\
<h1>System configuration.</h1>\
<form action=\"/saveConfig\" method=\"POST\">\
<fieldset>\
<legend>\
<h1>WiFi Router credential:</h1>\
</legend>\
<h3>SSID:</h3>\
<div><input type=\"text\" name=\"WiFissid\" value=\"%s\" maxlength=\"10\"></div>\
<h3>PASSWORD:</h3>\
<div><input type=\"text\" name=\"WiFipassword\" value=\"%s\" maxlength=\"10\"></div>\
</fieldset>\
<fieldset>\
<legend>\
<h1>Set values</h1>\
</legend>\
<h3>Relay1:</h3>\
<div><label>North ± °</label><input type=\"number\" min=\"0\" max=\"45\" name=\"setVar1\" value=\"%d\" size=\"5\"></div>\
<h3>Relay2:</h3>\
<div><label>East ± °</label><input type=\"number\" min=\"0\" max=\"45\" name=\"setVar2\" value=\"%d\" size=\"5\"></div>\
<h3>Relay3:</h3>\
<div><label>South ± °</label><input type=\"number\" min=\"0\" max=\"45\" name=\"setVar3\" value=\"%d\" size=\"5\"></div>\
<h3>Relay4:</h3>\
<div><label>West ± °</label><input type=\"number\" min=\"0\" max=\"45\" name=\"setVar4\" value=\"%d\" size=\"5\"></div>\
</fieldset>\
<fieldset>\
<legend>\
<h1>System setting:</h1>\
</legend>\
<h3>Web title:(No space)</h3>\
<div><input type=\"text\" name=\"web-title\" value=\"%s\" maxlength=\"10\"></div>\
<h3>Device name:</h3>\
<div><input type=\"text\" name=\"device-name\" value=\"%s\" maxlength=\"10\"></div>\
</fieldset>\
<fieldset>\
<legend>\
<h1>WiFi AP mode credential:</h1>\
</legend>\
<h3>SSID:</h3>\
<div><input type=\"text\" name=\"APssid\" value=\"%s\" maxlength=\"10\"></div>\
<h3>PASSWORD:</h3>\
<div><input type=\"text\" name=\"APpassword\" value=\"%s\" maxlength=\"10\"></div>\
</fieldset>\
<div><input type=\"submit\" value=\"SAVE\"></div>\
</form>\
<div style=\"text-align: center;\">\
<hr> Ready to use Wind direction indicator Devkit.</div>\
</div>\
</body>\
</html>",

           storageGetString("WiFissid"), storageGetString("WiFipassword"), setVar1, setVar2, setVar3, setVar4, storageGetString("webTitle"), storageGetString("deviceName"), storageGetString("APssid"), storageGetString("APpassword"));
  server.send(200, "text/html", html);
}

void saveConfig() {

  char setV1, setV2, setV3, setV4, webserveron;
  String message = "<h3>Data saved.<br></h3><hr>";
  String WIFIssid, WIFIpassword, APssid, APpassword, _webTitle, _deviceName;
  // message += (server.method() == HTTP_GET) ? "GET" : "POST";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " <b>" + server.argName(i) + ":</b> ______" + server.arg(i) + "______<br>\n";
    if (server.argName(i) == "APssid") {
      APssid = server.arg(i);
    } else if (server.argName(i) == "APpassword") {
      APpassword = server.arg(i);
    } else if (server.argName(i) == "WiFissid") {
      WIFIssid = server.arg(i);
    } else if (server.argName(i) == "WiFipassword") {
      WIFIpassword = server.arg(i);
    } else if (server.argName(i) == "setVar1") {
      setV1 = server.arg(i).toInt();
    } else if (server.argName(i) == "setVar2") {
      setV2 = server.arg(i).toInt();
    } else if (server.argName(i) == "setVar3") {
      setV3 = server.arg(i).toInt();
    } else if (server.argName(i) == "setVar4") {
      setV4 = server.arg(i).toInt();
    } else if (server.argName(i) == "web-title") {
      _webTitle = server.arg(i);
    } else if (server.argName(i) == "device-name") {
      _deviceName = server.arg(i);
    }
  }
  message += "<html><head><meta charset=\"UTF-8\" />\<style>\
   html{width:100%%;height:100%%}body{margin:30px;background:#005157;font-family:Arial,Helvetica,sans-serif;color:#eee}\
</style></head><body><script>alert('Saving data please wait until device restarted.');</script><br><br></body></html>";
  server.send(200, "text/html", message);

  storagePutString("APssid", APssid);
  storagePutString("APpassword", APpassword);
  storagePutString("WiFissid", WIFIssid);
  storagePutString("WiFipassword", WIFIpassword);
  storagePutString("webTitle", _webTitle);
  storagePutString("deviceName", _deviceName);
  storagePutInt("setVar1", setV1);
  storagePutInt("setVar2", setV2);
  storagePutInt("setVar3", setV3);
  storagePutInt("setVar4", setV4);
  ESP.restart();
}