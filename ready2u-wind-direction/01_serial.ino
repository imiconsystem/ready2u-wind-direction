const int numBytes = 8;
const byte modbusQuery[numBytes] = { 0x03, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC5, 0xE9 };  // RS FX wind direction
byte buffQuery[numBytes];

byte ByteArray[9];
int ByteData[9];

void serialSetup() {
  Serial2.begin(4800, SERIAL_8N1, RXD2, TXD2);  //
}

void serialLoop() {

  SerialProcessData();
  delay(5000);
}

void SerialProcessData() {

  for (int j = 0; j < numBytes; j++) {
    Serial2.write(modbusQuery[j]);
  }
  int a = 0;
  while (Serial2.available() > 0) {
    ByteArray[a] = Serial2.read();
    a++;
  }

  ByteData[0] = ByteArray[5] * 256 + ByteArray[6];

  float Data;
  Data = ByteData[0];

  Serial.print("Data ");
  Serial.println(Data);


  if (Data >= 0) {
    WINDIR = Data;

    Serial.print("DIR =  ");
    Serial.println(WINDIR);

    if (WINDIR >= 0 && WINDIR < 45) alarmlev = { "N", "" };
    else if (WINDIR >= 45 && WINDIR < 90) alarmlev = { "NE", "" };
    else if (WINDIR >= 90 && WINDIR < 135) alarmlev = { "E", "" };
    else if (WINDIR >= 135 && WINDIR < 180) alarmlev = { "SE", "" };
    else if (WINDIR >= 180 && WINDIR < 225) alarmlev = { "S", "" };
    else if (WINDIR >= 225 && WINDIR < 270) alarmlev = { "SW", "" };
    else if (WINDIR >= 270 && WINDIR < 315) alarmlev = { "W", "" };
    else if (WINDIR >= 315 && WINDIR < 360) alarmlev = { "NW", "" };
  }
  Serial.println(alarmlev.word);
}