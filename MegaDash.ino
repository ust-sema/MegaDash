#include <U8g2lib.h>
#include "DS3231.h"
#include <Wire.h>
#include <util/atomic.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TimerThree.h>
#include <MsTimer2.h>
#include <EEPROM.h>

#define pinSPD 2         //Speed
#define pinCap A3        // Power loss sensor
#define ONE_WIRE_BUS 10  // ambient temperature
#define pinFuel A0       //Fuel level
#define pinTemp A1       //Temperature
#define pinBuzzer 40     //Buzzer :)
#define pinLCDLight 39
#define pinDashLight 41
#define pinTachLight 43

#define btn_up 18
#define btn_down 17
#define btn_select 3

U8G2_KS0108_128X64_2 u8g(U8G2_R0, 22, 23, 24, 25, 26, 27, 28, 29, 30, 32, 33, 34, U8X8_PIN_NONE, U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

volatile unsigned long odo_int;
volatile unsigned int iRPM, iSpeed;
unsigned int maxSpeed = 0;

byte temp_o, temp_sign, gear;
String odo_ts, odo_os, temp_os;
float odo_t, odo_o, odo_a;
float epr_odo_o, epr_odo_t;
int fuel, temp;
byte fuel_d = 0, temp_d = 0;  // values to display
byte bkg_light;
float runTime;

RTClib RTC;
DateTime now;
uint8_t hours0;
uint8_t minutes0;
uint8_t hours;
uint8_t minutes;
uint8_t seconds;
uint8_t hours_r;
uint8_t minutes_r;

bool drawDots;
int gearRatio;
byte uiMode = 0;
volatile byte button_command = 0;
bool runStarted = false;
bool bikeStopped = true;

bool btn_up_pressed, btn_down_pressed, btn_select_pressed;

unsigned long currentMillis, start200Millis, start1000Millis, start3000Millis, runMillis;

volatile uint16_t frequencySPD = 300 * 10.4;
volatile uint16_t frequencyRPM = 12000 / 30;
volatile bool readRPM = false, readSPD = false;
bool dash_test = true;
byte lights_duty_count;
float speed_correction = 10.4;

#define READINGS_NUMBER_FUEL 10
#define READINGS_NUMBER_TEMP 10

static uint16_t readingsFUEL[READINGS_NUMBER_FUEL];
static uint8_t readIndexFUEL = 0;
static uint32_t totalFUEL = 0;

static uint16_t readingsTEMP[READINGS_NUMBER_TEMP];
static uint8_t readIndexTEMP = 0;
static uint32_t totalTEMP = 0;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {

  //Serial.begin(9600);
  pinMode(pinDashLight, OUTPUT);
  digitalWrite(pinDashLight, HIGH);

  pinMode(pinTachLight, OUTPUT);
  digitalWrite(pinTachLight, LOW);

  Wire.begin();
  startTimer5();  //RPM pin 48, Analog write disabled on 44, 45, 46
  startTimer4();  //speed pin 49, Analog write disabled on 6, 7, 8

  //setup_rotary();
  pinMode(btn_up, INPUT_PULLUP);
  pinMode(btn_down, INPUT_PULLUP);
  pinMode(btn_select, INPUT_PULLUP);

  pinMode(pinSPD, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinSPD), speedSensor, FALLING);

  u8g.begin();
  u8g2.begin();

  //EEPROM.get(10, bkg_light);
  //bkg_light = 0;
  pinMode(pinLCDLight, OUTPUT);
  //digitalWrite(pinLCDLight, bkg_light);
  digitalWrite(pinLCDLight, LOW);

  Timer3.initialize(60000);
  Timer3.attachInterrupt(processReadings);  // run every 0.06 seconds

  EEPROM.get(0, odo_o);
  EEPROM.get(4, odo_t);
  epr_odo_o = odo_o;
  epr_odo_t = odo_t;

  if (isnan(odo_o)) odo_o = 0;
  if (isnan(odo_t)) odo_t = 0;

  MsTimer2::set(1, lightsPWM);
  MsTimer2::start();

  drawOLED();

  start200Millis = start1000Millis = start3000Millis = millis();
}

void lightsPWM() {
  if (dash_test) return;

  if (lights_duty_count > 2) {
    digitalWrite(pinDashLight, HIGH);
  }
  if (lights_duty_count > 9) {
    lights_duty_count = 0;
    digitalWrite(pinDashLight, LOW);
  }
  lights_duty_count++;
}

void speedSensor() {
  odo_int++;
}

void loop() {

  currentMillis = millis();

  if (currentMillis - start200Millis >= 200) {
    start200Millis = currentMillis;

    if (iSpeed == 0) {
      if (epr_odo_o != odo_o) {
        epr_odo_o = odo_o;
        EEPROM.put(0, odo_o);
      }
      if (epr_odo_t != odo_t) {
        epr_odo_t = odo_t;
        EEPROM.put(4, odo_t);
      }
    }
  }

  if (currentMillis - start1000Millis >= 1000) {
    start1000Millis = currentMillis;
    now = RTC.now();
    hours0 = 0;
    hours = now.hour();
    if (hours > 9) {
      hours0 = hours / 10;
      hours = hours % 10;
    }
    minutes0 = 0;
    minutes = now.minute();
    if (minutes > 9) {
      minutes0 = minutes / 10;
      minutes = minutes % 10;
    }
    drawDots = !drawDots;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      odo_a = odo_int;
      odo_int = 0;
    }

    odo_o = odo_o + odo_a * 0.02458f;
    odo_t = odo_t + odo_a * 0.02458f;
    odo_ts = String(odo_t / 1000, 1);
    odo_os = String(odo_o / 1000, 1);
    temp_os = String(temp_o);

    //Serial.println("spd freq: " + String(frequencySPD) + ",rpm  freq: " + String(frequencyRPM) + ",isr time: " + String(avg_time));

    if (!readSPD) frequencySPD = 0;
    readSPD = false;
    if (!readRPM) frequencyRPM = 0;
    readRPM = false;

    dash_test = false;

    drawOLED();
  }

  if (currentMillis - start3000Millis >= 2000) {
    start3000Millis = currentMillis;

    sensors.requestTemperatures();
    float tmp = sensors.getTempCByIndex(0);
    temp_o = tmp + 0.5;

    ///----------------  FUEL, TEMP ---------------------
    totalFUEL = totalFUEL - readingsFUEL[readIndexFUEL];
    totalTEMP = totalTEMP - readingsTEMP[readIndexTEMP];

    readingsFUEL[readIndexFUEL] = analogRead(pinFuel);
    readingsTEMP[readIndexTEMP] = analogRead(pinTemp);

    totalFUEL = totalFUEL + readingsFUEL[readIndexFUEL];
    totalTEMP = totalTEMP + readingsTEMP[readIndexTEMP];

    readIndexFUEL++;
    if (readIndexFUEL >= READINGS_NUMBER_FUEL) readIndexFUEL = 0;

    readIndexTEMP++;
    if (readIndexTEMP >= READINGS_NUMBER_TEMP) readIndexTEMP = 0;

    fuel = totalFUEL / READINGS_NUMBER_FUEL;
    temp = totalTEMP / READINGS_NUMBER_TEMP;

    if (fuel > 350) {
      fuel = 0;
    } else {
      if (fuel < 50) fuel = 39;
      else fuel = 39 - ((fuel - 50) / 7.69f);
    }

    if (temp > 400) {
      temp = 0;
    } else {
      temp = 39 - (temp / 10.26f);
    }
    //----------------------------------------------------

    digitalWrite(pinBuzzer, LOW);
  }

  if (dash_test) {
    gear = 8;
  } else {
    if (iSpeed > 0) gearRatio = iRPM / iSpeed;
    else gearRatio = 0;

    if (gearRatio > 93 && gearRatio < 103) gear = 1;
    else if (gearRatio > 66 && gearRatio < 73) gear = 2;
    else if (gearRatio > 51 && gearRatio < 58) gear = 3;
    else if (gearRatio > 43 && gearRatio < 49) gear = 4;
    else if (gearRatio > 38 && gearRatio < 43) gear = 5;
    else if (gearRatio > 34 && gearRatio < 38) gear = 6;
    else gear = 0;

    if (readSPD) {

      if (iSpeed > maxSpeed) maxSpeed = iSpeed;

      if (iSpeed > 0 && !runStarted && bikeStopped) {
        runMillis = millis();
        runStarted = true;
        bikeStopped = false;
      } else {
        if (iSpeed >= 100 && runStarted) {
          runStarted = false;
          runTime = (float)(millis() - runMillis) / 1000;
          runTime = runTime > 10 ? 0 : runTime;
        }
      }
      if (iSpeed == 0) {
        bikeStopped = true;
        runStarted = false;
      }
    }
  }

  draw();

  if (!digitalRead(btn_up)) btn_up_pressed = true;
  if (!digitalRead(btn_down)) btn_down_pressed = true;
  if (!digitalRead(btn_select)) btn_select_pressed = true;

  if (digitalRead(btn_up) && btn_up_pressed) {
    btn_up_pressed = false;
    button_command = 1;
  }
  if (digitalRead(btn_down) && btn_down_pressed) {
    btn_down_pressed = false;
    button_command = 2;
  }

  if (digitalRead(btn_select) && btn_select_pressed) {
    btn_select_pressed = false;
    button_command = uiMode > 0 ? 3 : 250;
    uiMode = 1;
  }

  if (uiMode > 0 && button_command > 0) {
    navMenu(button_command);
    button_command = 0;
  }
}
