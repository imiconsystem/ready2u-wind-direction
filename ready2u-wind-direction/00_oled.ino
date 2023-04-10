#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#include <U8g2lib.h>

U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

U8G2LOG u8g2log;

#define U8LOG_WIDTH 128
#define U8LOG_HEIGHT 32

uint8_t u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];

void oledSetup(void) {
  u8g2.begin();
}

void oledLogSetup(void) {
  u8g2.setFont(u8g2_font_squeezed_b7_tr);
  u8g2log.begin(u8g2, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
  u8g2log.setLineHeightOffset(0);
  u8g2log.setRedrawMode(0);
}


void oledDrawPage2() {

  u8g2.setFont(u8g2_font_siji_t_6x10);
  u8g2.drawGlyph(0, 8, 0xE219);   //wifi
  u8g2.drawGlyph(0, 16, 0xE0F2);  //rssi

  u8g2.setFont(u8g2_font_squeezed_b7_tr);
  u8g2.drawStr(15, 8, storageGetString("WiFissid").c_str());
  u8g2.drawStr(15, 17, WiFiRSSI.c_str());
  u8g2.drawStr(50, 17, myIP.c_str());
  u8g2.drawStr(50, 26, storageGetString("deviceName").c_str());
}


void oledDrawMain(void) {
  u8g2.setFont(u8g2_font_squeezed_b7_tr);
  u8g2.drawStr(0, 8, "DIR");

  u8g2.drawStr(91, 30, alarmlev.word.c_str());
  u8g2.setFont(u8g2_font_6x12_t_symbols);


  u8g2.setFont(u8g2_font_7_Seg_33x19_mn);
  u8g2.drawStr(30, 0, u8x8_u8toa(WINDIR > 99 ? 99 : WINDIR, 3));
}

uint8_t draw_state = 0;

void oledDrawState(void) {
  switch (draw_state) {
    case 0: oledDrawMain(); break;
    case 1: oledDrawPage2(); break;
  }
}
void oledLogLoop() {
  u8g2log.print(logString);
  u8g2log.print("\n");
  delay(1000);
}
void oledLoop(void) {

  u8g2.firstPage();
  do {
    oledDrawState();
  } while (u8g2.nextPage());

  draw_state++;
  if (draw_state > 1)
    draw_state = 0;
  delay(3000);
}