
void draw() {
  u8g.firstPage();
  do {
    if (uiMode > 0)
      drawMenues();
    else
      drawMainScreen();
  } while ( u8g.nextPage() );
}

void drawMainScreen() {
  int i;
  u8g.setDrawColor(1);
  //-- Speed
  drawNumber27(74, 21, iSpeed);

  //--Gear
  u8g.drawXBMP(1, 21, 16, 27, n27x16[gear]);

  //-- RPM
  u8g.drawXBMP(0, 0, 128, 16, tachBmp);
  u8g.setDrawColor(2);
  u8g.drawBox(0, 1, iRPM / 100, 14);

  // -- Grid
  u8g.setDrawColor(1);
  u8g.drawHLine(0, 53, 128);
  u8g.drawHLine(21, 30, 49);
  u8g.drawVLine(70, 16, 37);
  u8g.drawVLine(20, 16, 37);

  //fuel, temp
  u8g.drawXBMP(23, 33, 45, 18, tfBmp);
  u8g.drawBox(28, 33, temp, 7);
  u8g.drawBox(28, 44, fuel, 7);

  //u8g.setFont(u8g2_font_helvB08_tr);
  //u8g.drawStr(35, 42, String(temp).c_str());
  //u8g.drawStr(35, 52, String(fuel).c_str());

  //-- Temperature
  u8g.setFont(u8g2_font_helvB08_tr);
  i = u8g.getStrWidth(temp_os.c_str());
  if (temp_o != 0) {
    u8g.drawHLine(12 - i, 60, 3);
    if (temp_o > 0) u8g.drawVLine(13 - i, 59, 3);
  }
  u8g.drawStr(16 - i, 64, temp_os.c_str());

  //odo
  u8g.drawXBMP(16, 56, 62, 7, odoXBM);
  i = u8g.getStrWidth(odo_os.c_str());
  u8g.drawStr(126 - i, 64, odo_os.c_str());
  i = u8g.getStrWidth(odo_ts.c_str());
  u8g.drawStr(67 - i, 64, odo_ts.c_str());

  //--- clock
  u8g.drawXBMP(29, 18, 6, 10, n10x6[hours0]);
  u8g.drawXBMP(36, 18, 6, 10, n10x6[hours]);
  if (drawDots) {
    u8g.drawBox(44, 20, 2, 2);
    u8g.drawBox(44, 24, 2, 2);
  }
  u8g.drawXBMP(48, 18, 6, 10, n10x6[minutes0]);
  u8g.drawXBMP(55, 18, 6, 10, n10x6[minutes]);
  //-----------------

}

void drawNumber27(int x, int y, int number) {
  int d = number;
  u8g.drawXBMP(x + 38, y, 16, 27, n27x16[d % 10]);
  d /= 10;
  if (d > 0) u8g.drawXBMP(x + 19, y, 16, 27, n27x16[d % 10]);
  d /= 10;
  if (d > 0) u8g.drawXBMP(x, y, 16, 27, n27x16[d % 10]);
}
