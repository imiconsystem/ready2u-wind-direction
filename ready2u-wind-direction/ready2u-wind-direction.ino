#define R1 23
#define R2 19
#define R3 18
#define R4 32

// กำหนดตัวแปรสำหรับการสื่อสารผ่านโปรโตคอล Modbus RTU
#define RXD2 16  // กำหนดหมายเลข GPIO 16 สำหรับขา Rx
#define TXD2 17  // กำหนดหมายเลข GPIO 17 สำหรับขา Tx

String APssid = "WINDIRDevkit";
String APpassword = "dddddddd";

int SETMODE = 0;

String WiFissid = "";
String WiFipassword = "";

String WiFiRSSI;
String myIP;
String webTitle = "Anemometer Alarm";
String device_name = "anemo";

int32_t WINDIR,setVar1, setVar2, setVar3, setVar4;


struct ALARMLEV {
  String word;
  String icon;
};
struct ALARMLEV alarmlev = {};

void setup() {

  Serial.begin(115200);  // For debug
  Serial.println("ESP start.");
  serialSetup();
  relaySetup();
  resetbuttonSetup();
  storageSetup();

  WINDIR = 0;

  if (SETMODE == 1)  // SET
  {
    SETMODE = true;
    wifiapSetup();



  } else if (SETMODE == 2) {  // RESET FACTORY
    storageClear();
    // ESP.restart();
  } else {
    // RUN
    webserverSetup();
  }
}

void controlRelay() {

  if (WINDIR >= (360-setVar1) && WINDIR >= (0+setVar1) || WINDIR == 0) relayHi(R1); // North wind
  else relayLo(R1);

  if (WINDIR >= (90-setVar2) && WINDIR <= (90+setVar2) ) relayHi(R2); // East wind
  else relayLo(R2);

  if (WINDIR >= (180-setVar3) && WINDIR <= (180+setVar3) ) relayHi(R3); // South wind
  else relayLo(R3);

  if (WINDIR >= (270-setVar4) && WINDIR <= (270+setVar4) ) relayHi(R4); // West wind
  else relayLo(R4);

}


void loop() {
  if (SETMODE == 0) {

    webserverLoop();
    clientLoop();
    serialLoop();
    controlRelay();
  } else if (SETMODE == 1)  // SET
  {
    Serial.println("SET MODE.");
    clientLoop();
    blinkSet();
  }
  else if (SETMODE == 2) {  //RESET
    blinkReset();
  }
  else {
    Serial.println("CLIENT MODE.");
    clientLoop();
  }
}