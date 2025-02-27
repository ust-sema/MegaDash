
void drawOLED(void) {
  u8g2.firstPage();
  do {
    if (dash_test) {
      u8g2.drawXBMP(42, 0, ironman_width, ironman_height, ironman_bits);
    } else {
      u8g2.setFont(u8g2_font_profont22_tr);
      u8g2.drawStr(12, 15, ("SMAX: " + String(maxSpeed)).c_str());
      u8g2.drawStr(1, 35, "ZERO-SIXTY:");
      u8g2.drawStr(40, 52, (String(runTime, 2)).c_str());
    }
  } while (u8g2.nextPage());
}
