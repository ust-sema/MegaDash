byte menuPos0, menuPos1, menuPos4, currentMenu;

void drawMenues() {
  u8g.setFont(u8g2_font_7x13_t_cyrillic);
  switch (currentMenu) {
    case 0:
      drawMenu0();
      break;
    case 1:
      drawMenu1();
      break;
    case 2:
      drawMenu2();
      break;
    case 3:
      drawMenu3();
      break;
    case 4:
      drawMenu4();
      break;
    case 42:
      drawMenu42();
      break;
  }
  //u8g.drawStr(100, 60, (String(currentMenu) + ", " + String(menuPos1)).c_str());
}

void drawMenu0() {
  u8g.setDrawColor(1);
  u8g.drawUTF8(1, 9, "сбросить пробег");
  u8g.drawUTF8(1, 19, "настроить время");
  u8g.drawUTF8(1, 29, "установки");
  u8g.drawUTF8(1, 39, "возврат");
  u8g.setDrawColor(2);
  u8g.drawBox(0, menuPos0 * 10, 127, 10);

  //u8g.setFont(u8g2_font_saikyosansbold8_8u);
  //u8g.drawStr(1, 52, "S-MAX: 300");
  //u8g.drawStr(1, 62, "0-100: 3.81");
}

void drawMenu1() {
  u8g.setDrawColor(1);
  u8g.drawUTF8(1, 9, "часы");
  u8g.drawUTF8(1, 19, "минуты");
  u8g.drawUTF8(1, 29, ("установить " + String(hours_r) + ":" + String(minutes_r)).c_str());
  u8g.drawUTF8(1, 39, "возврат");
  u8g.setDrawColor(2);
  u8g.drawBox(0, menuPos1 * 10, 127, 10);
}

void drawMenu2() {
  u8g.setDrawColor(1);
  u8g.drawUTF8(1, 9, "часы: ");
  u8g.drawUTF8(40, 9, String(hours_r).c_str());
}

void drawMenu3() {
  u8g.setDrawColor(1);
  u8g.drawUTF8(1, 9, "минуты: ");
  u8g.drawUTF8(50, 9, String(minutes_r).c_str());
}

void drawMenu4() {
  u8g.setDrawColor(1);
  u8g.drawUTF8(1, 9, "подсветка вкл/выкл");
  u8g.drawUTF8(1, 19, "коррекция скорости");
  u8g.drawUTF8(1, 29, "возврат");
  u8g.setDrawColor(2);
  u8g.drawBox(0, menuPos4 * 10, 127, 10);
}

void drawMenu42() {
  u8g.setDrawColor(1);
  u8g.drawUTF8(1, 9, "коррекция скорости: ");
  u8g.drawUTF8(50, 29, String(speed_correction).c_str());
}

void navMenu(byte cmd) {
  switch (currentMenu) {
    case 0:
      navMenu0(cmd);
      break;
    case 1:
      navMenu1(cmd);
      break;
    case 2:
      navMenu2(cmd);
      break;
    case 3:
      navMenu3(cmd);
      break;
    case 4:
      navMenu4(cmd);
      break;
    case 42:
      navMenu42(cmd);
      break;
  }
}

#define menuCount0 3
void navMenu0(byte cmd) {
  switch (cmd) {
    case 2:
      menuPos0++;
      menuPos0 = menuPos0 > menuCount0 ? 0 : menuPos0;
      break;
    case 1:
      menuPos0--;
      menuPos0 = menuPos0 > 250 ? menuCount0 : menuPos0;
      break;
    case 3:
      switch (menuPos0) {
        case 0:
          odo_t = 0;
          buzz(700, 100);
          uiMode = 0;
          break;
        case 1:
          currentMenu = 1;
          break;
        case 2:
          currentMenu = 4;
          break;
        case 3:
          uiMode = 0;
          break;
      }
      break;
  }
}

#define menuCount1 3
void navMenu1(byte cmd) {
  DS3231 Clock;
  switch (cmd) {
    case 2:
      menuPos1++;
      menuPos1 = menuPos1 > menuCount1 ? 0 : menuPos1;
      break;
    case 1:
      menuPos1--;
      menuPos1 = menuPos1 > 250 ? menuCount1 : menuPos1;
      break;
    case 3:
      switch (menuPos1) {
        case 0:
          currentMenu = 2;
          break;
        case 1:
          currentMenu = 3;
          break;
        case 2:
          Clock.setHour(hours_r);
          Clock.setMinute(minutes_r);
          Clock.setSecond(0);
          buzz(700, 100);
          break;
        case 3:
          currentMenu = 0;
          break;
      }
      break;
  }
}

#define menuCount2 23
void navMenu2(byte cmd) {
  switch (cmd) {
    case 2:
      hours_r++;
      hours_r = hours_r > menuCount2 ? 0 : hours_r;
      break;
    case 1:
      hours_r--;
      hours_r = hours_r > 250 ? menuCount2 : hours_r;
      break;
    case 3:
      currentMenu = 1;
      break;
  }
}

#define menuCount3 59
void navMenu3(byte cmd) {
  switch (cmd) {
    case 2:
      minutes_r++;
      minutes_r = minutes_r > menuCount3 ? 0 : minutes_r;
      break;
    case 1:
      minutes_r--;
      minutes_r = minutes_r > 250 ? menuCount3 : minutes_r;
      break;
    case 3:
      currentMenu = 1;
      break;
  }
}

#define menuCount4 2
void navMenu4(byte cmd) {
  switch (cmd) {
    case 2:
      menuPos4++;
      menuPos4 = menuPos4 > menuCount4 ? 0 : menuPos4;
      break;
    case 1:
      menuPos4--;
      menuPos4 = menuPos4 > 250 ? menuCount4 : menuPos4;
      break;
    case 3:
      switch (menuPos4) {
        case 0:
          bkg_light = bkg_light > 0 ? 0 : 1;
          digitalWrite(pinLCDLight, bkg_light);
          break;
        case 1:
          currentMenu = 42;
          break;
        case 2:
          currentMenu = 0;
          break;
      }
      break;
  }
}

void navMenu42(byte cmd) {
  switch (cmd) {
    case 2:
      speed_correction = speed_correction + 0.01;
      break;
    case 1:
      speed_correction = speed_correction - 0.01;
      break;
    case 3:
      currentMenu = 4;
      uiMode = 0;
      break;
  }
}
