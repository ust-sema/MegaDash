static uint8_t saveTCCR5A, saveTCCR5B;

static inline void capture_init5(void)
{
  saveTCCR5A = TCCR5A;
  saveTCCR5B = TCCR5B;
  TCCR5B = 0;
  TCCR5A = 0;
  TCNT5 = 0;
  TIFR5 = (1 << ICF5) | (1 << TOV5);
  TIMSK5 = (1 << ICIE5) | (1 << TOIE5);
}
static inline void capture_start5(void)
{
  TCCR5B = (1 << ICNC5) | (1 << ICES5) | (1 << CS50);
}
static inline uint16_t capture_read5(void)
{
  return ICR5;
}
static inline uint8_t capture_overflow5(void)
{
  return TIFR5 & (1 << TOV5);
}
static inline void capture_overflow_reset5(void)
{
  TIFR5 = (1 << TOV5);
}
static inline void capture_shutdown5(void)
{
  TCCR5B = 0;
  TIMSK5 = 0;
  TCCR5A = saveTCCR5A;
  TCCR5B = saveTCCR5B;
}

static uint16_t capture_msw5;
static uint32_t capture_previous5;

void startTimer5()
{
  capture_init5();
  capture_msw5 = 0;
  capture_previous5 = 0;
  capture_start5();
}

void endTimer5()
{
  capture_shutdown5();
}

ISR(TIMER5_OVF_vect)
{
  capture_msw5++;
}

ISR(TIMER5_CAPT_vect)
{
  //avg_time = micros();
  uint16_t capture_lsw;
  uint32_t capture, period;

  // get the timer capture
  capture_lsw = capture_read5();
  if (capture_overflow5() && capture_lsw < 0xFF00) {
    capture_overflow_reset5();
    capture_msw5++;
  }
  // compute the waveform period
  capture = ((uint32_t)capture_msw5 << 16) | capture_lsw;
  period = capture - capture_previous5;
  capture_previous5 = capture;

  if (dash_test) return;
  frequencyRPM = (float)F_CPU / (float)period;
  readRPM = true;
  //avg_time = micros() - avg_time;
}
