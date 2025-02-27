#define sinPin1 6 //AIN1
#define sinPin2 7 //AIN2
#define cosPin1 8 //BIN1 
#define cosPin2 9 //BIN2 
#define sinPWM 12 //PWMA 
#define cosPWM 13 //PWMB 

#define READINGS_NUMBER_RPM 7
#define READINGS_NUMBER_SPD 10

static uint16_t readingsRPM[READINGS_NUMBER_RPM];
static uint8_t readIndexRPM = 0;
static uint32_t totalRPM = 0;

static uint16_t readingsSPD[READINGS_NUMBER_SPD];
static uint8_t readIndexSPD = 0;
static uint32_t totalSPD = 0;


void processReadings() {
  //avg_time = micros();

  totalRPM = totalRPM - readingsRPM[readIndexRPM];
  totalSPD = totalSPD - readingsSPD[readIndexSPD];

  readingsRPM[readIndexRPM] = frequencyRPM;
  readingsSPD[readIndexSPD] = frequencySPD;

  totalRPM = totalRPM + readingsRPM[readIndexRPM];
  totalSPD = totalSPD + readingsSPD[readIndexSPD];

  readIndexRPM++;
  if (readIndexRPM >= READINGS_NUMBER_RPM) readIndexRPM = 0;

  readIndexSPD++;
  if (readIndexSPD >= READINGS_NUMBER_SPD) readIndexSPD = 0;

  iRPM = (totalRPM / READINGS_NUMBER_RPM) * 30;
  iSpeed = (totalSPD / (float)READINGS_NUMBER_SPD) / speed_correction;

  //float pos = 4.7124 + map(iRPM, 0, 12500, 0, 471) / (float)100;
  float pos = 4.64 + map(iRPM, 0, 12500, 0, 471) / (float)100;
  // Calculate the voltage on the PWM pins based on the angle we want
  float sinCoilValue = 255 * sin(pos);
  float cosCoilValue = 255 * cos(pos);

  // change the polarity of the coil depending on the sign of the voltage level
  if (sinCoilValue <= 0) {
    digitalWrite(sinPin1, LOW);
    digitalWrite(sinPin2, HIGH);
  } else {
    digitalWrite(sinPin1, HIGH);
    digitalWrite(sinPin2, LOW);
  }
  if (cosCoilValue <= 0) {
    digitalWrite(cosPin1, LOW);
    digitalWrite(cosPin2, HIGH);
  } else {
    digitalWrite(cosPin1, HIGH);
    digitalWrite(cosPin2, LOW);
  }

  analogWrite(sinPWM, abs(sinCoilValue));
  analogWrite(cosPWM, abs(cosCoilValue));

 // avg_time = micros() - avg_time;
}
