# DRUM_2024_AI

All the changes were added using AI

ESP32 DRUM SYNTH MACHINE

DRUM SYNTH LOFI MACHINE.

![2026-02-03 22-22-08](https://github.com/user-attachments/assets/58913a9b-42ed-4cb6-b255-0ffcb2dc6695)


# Synth engine:

- Wavetable synthesizer based on DZL Arduino library ["The Synth"](https://github.com/dzlonline/the_synth)
- 16 sound polyphony
- Sound parameters: Table, Length, Envelope, Pitch, Modulation, + Volume, Pan and Filter.
- Filter (LowPassFilter) comes from [Mozzi Library](https://github.com/sensorium/Mozzi)
- Master delay (on 14 button).
- Wavetable's name on screen.
- BPM on main screen.

SEQUENCER:

- 16 step/pattern editor and random generators (pattern, sound parameters and notes)

# Hardware:

- Lolin S2 Mini (ESP32 S2)
- PCM5102A I2s dac
- 24 push buttons (8x3)
- Rotary encoder
- OLED display I2c
- 32 LED WS2812B

# Software:

IDE:
Arduino `1.8.19` 
Arduino `2.3.2`

Boards:
Expressif Systems `2.0.16` (Tested fine up to `2.0.17`)

Board: Lolin S2 Mini

Necessary Libraries:

- Sequencer Timer - [uClock](https://github.com/midilab/uClock)
- RGB Leds - [Adafruit Neopixel](https://github.com/adafruit/Adafruit_NeoPixel)
- OLED - [u8g2](https://github.com/olikraus/u8g2)
- Button input - [Keypad](https://github.com/Chris--A/Keypad)

# Notes:

STL 3D model uploaded.

Cheat sheet style PDF uploaded.

Join solder pads near SCK pin on PCM5102A module.

**Update**: 

- Solder pads on the back of PCM5102A module ([more info](https://github.com/pschatzmann/ESP32-A2DP/wiki/External-DAC#pcm5102-dac))
  
  - H1L: FLT - Low
  
  - H2L: DEMP - Low
  
  - H3L: XSMT - High
  
  - H4L: FMT - Low

Video demo of the prototype:

[![IMG_20240406_150231](https://img.youtube.com/vi/rXl1gpWJp-g/0.jpg)](https://www.youtube.com/watch?v=rXl1gpWJp-g)


# PCB, PROJECT & FINAL LOOK:

https://oshwlab.com/zircothc/esp32-drum-synth-lofi-machine

https://easyeda.com/editor#project_id=08f629c08b18444ca16accbb2242c0fb

![2026-02-03 22-22-23](https://github.com/user-attachments/assets/9173a16f-6bcb-432d-ac45-3aedb6622e3d)

![2026-02-03 22-22-41](https://github.com/user-attachments/assets/5598e8c7-5ca9-42a5-a6b4-0a3d87faff44)


