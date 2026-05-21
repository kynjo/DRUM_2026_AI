// ================================================================
//  MIDI handlers
// ================================================================

// ---- Note On ----
void handleNoteOn(byte channel, byte pitch, byte velocity) {
  if (velocity == 0) { handleNoteOff(channel, pitch, velocity); return; }

  int voice;
  if (pitch >= 36 && pitch <= 51) {
    voice = pitch - 36;       // GM Drums: нота 36-51 → голос 0-15
  } else {
    voice = pitch % 16;
  }
  synthESP32_mTrigger(voice, pitch);

  if (recording && playing) {
    bitWrite(pattern[voice], sstep, 1);
    melodic[voice][sstep] = pitch;
  }
}

// ---- Note Off ----
void handleNoteOff(byte channel, byte pitch, byte velocity) {
  // Драм-машина: короткие звуки, NoteOff игнорируется
}

// ---- Control Change ----
void handleCC(byte channel, byte ccnumber, byte value) {
  if (ccnumber >= 8) return;

  int voice  = constrain(channel - 1, 0, 15);
  int nvalue = (ccnumber == 6)
               ? map(value, 0, 127, -99, 99)
               : (int)value;

  if (nvalue > max_values[ccnumber]) return;
  if (nvalue < min_values[ccnumber]) return;

  ROTvalue[voice][ccnumber] = nvalue;
  setSound(voice);
  refreshOLED = true;
}

// ================================================================
//  Transport + Clock
// ================================================================

volatile bool externalClock = false;

// ---- MIDI Clock (24 тика/четверть) ----
void handleClock() {
  if (externalClock) uClock.clockMe();
}

// ---- Start (0xFA) ----
void handleStart() {
  externalClock = true;
  uClock.setMode(uClock.EXTERNAL_CLOCK);
  sstep     = firstStep;
  playing   = true;
  recording = false;
  uClock.start();
  refreshSTEP   = true;
  refreshMODES  = true;
}

// ---- Continue (0xFB) — при зацикливании паттерна ----
void handleContinue() {
  externalClock = true;
  uClock.setMode(uClock.EXTERNAL_CLOCK);
  playing = true;
  uClock.start();
  refreshSTEP  = true;
  refreshMODES = true;
}

// ---- Stop (0xFC) ----
void handleStop() {
  // 1. Сначала остановить тактирование
  externalClock = false;
  uClock.stop();
  uClock.setMode(uClock.INTERNAL_CLOCK);

  // 2. Сбросить флаги
  playing   = false;
  recording = false;
  sstep     = firstStep;

  // 3. Заглушить все голоса — ПРАВИЛЬНЫЙ способ:
  //    EPCW = 0x8000 → огибающая уже "за концом" → AMP становится 0
  //    НЕ вызываем synthESP32_mTrigger(v,0) — это рестарт, не стоп!
  for (byte v = 0; v < 16; v++) {
    EPCW[v] = 0x8000;
  }

  refreshSTEP  = true;
  refreshMODES = true;
}

// ---- Song Position Pointer (0xF2) ----
void handleSongPosition(unsigned int beats) {
  byte loopLen = (lastStep - firstStep + 1);
  byte newStep = firstStep + (beats % loopLen);
  sstep        = constrain(newStep, firstStep, lastStep);
  refreshSTEP  = true;
}