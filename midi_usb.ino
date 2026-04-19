# MIDI USB Support for ESP32 Lolin S2 Mini

This file demonstrates complete MIDI USB support using the TinyUSB library for the ESP32 Lolin S2 Mini.

## Required Libraries
- `TinyUSB` library for USB communications

## Setup
1. Install the TinyUSB library via the Arduino Library Manager.
2. Connect your ESP32 Lolin S2 Mini to your computer.

## Code Structure

```cpp
#include <Arduino.h>
#include <TinyUSB.h>
#include <MIDI.h>

// Create a MIDI object
MIDI_CREATE_INSTANCE(SerialMIDI, Midi);

void setup() {
  // Start the MIDI communication over USB
  TinyUSB.begin();
  Midi.begin(MIDI_CHANNEL_OMNI);
  Serial.begin(115200);
  Serial.println("MIDI USB Initialized.");
}

void loop() {
  // Handle MIDI input
  while (Midi.read()) {
    int type = Midi.getType();
    if (type == midi::NoteOn) {
      Serial.print("Note On: ");
      Serial.println(Midi.getData1());
    }
    else if (type == midi::NoteOff) {
      Serial.print("Note Off: ");
      Serial.println(Midi.getData1());
    }
    // Handle other MIDI messages here
  }
}

void midiEventHandler(midi::MidiEvent event) {
  // Handle MIDI events
  // Event handling code here
}

void sendMidiNoteOn(int note, int velocity) {
  Midi.sendNoteOn(note, velocity);
  Serial.print("Sent Note On: ");
  Serial.println(note);
}

void sendMidiNoteOff(int note) {
  Midi.sendNoteOff(note);
  Serial.print("Sent Note Off: ");
  Serial.println(note);
}