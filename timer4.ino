static uint8_t saveTCCR4A, saveTCCR4B;

static inline void capture_init4(void)
{
  saveTCCR4A = TCCR4A;
  saveTCCR4B = TCCR4B;
  TCCR4B = 0;
  TCCR4A = 0;
  TCNT4 = 0;
  TIFR4 = (1 << ICF4) | (1 << TOV4);
  TIMSK4 = (1 << ICIE4) | (1 << TOIE4);
}
static inline void capture_start4(void)
{
  TCCR4B = (1 << ICNC4) | (1 << ICES4) | (1 << CS40);
}
static inline uint16_t capture_read4(void)
{
  return ICR4;
}
static inline uint8_t capture_overflow4(void)
{
  return TIFR4 & (1 << TOV4);
}
static inline void capture_overflow_reset4(void)
{
  TIFR4 = (1 << TOV4);
}
static inline void capture_shutdown4(void)
{
  TCCR4B = 0;
  TIMSK4 = 0;
  TCCR4A = saveTCCR4A;
  TCCR4B = saveTCCR4B;
}

static uint16_t capture_msw4;
static uint32_t capture_previous4;

void startTimer4()
{
  capture_init4();
  capture_msw4 = 0;
  capture_previous4 = 0;
  capture_start4();
}

void endTimer4()
{
  capture_shutdown4();
}

ISR(TIMER4_OVF_vect)
{
  capture_msw4++;
}

ISR(TIMER4_CAPT_vect)
{
  uint16_t capture_lsw;
  uint32_t capture, period;

  // get the timer capture
  capture_lsw = capture_read4();
  if (capture_overflow4() && capture_lsw < 0xFF00) {
    capture_overflow_reset4();
    capture_msw4++;
  }
  // compute the waveform period
  capture = ((uint32_t)capture_msw4 << 16) | capture_lsw;
  period = capture - capture_previous4;
  capture_previous4 = capture;

  if (dash_test) return;
  frequencySPD = (float)F_CPU / (float)period;
  readSPD = true;
}
