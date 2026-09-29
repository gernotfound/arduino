# 5 buzzerz 1 song

An experimental Arduino project that uses **five buzzer modules as five independent musical voices**.

The current piece is an approximately **66.5 second reduction of Mozart's Lacrimosa from the Requiem in D minor, K.626**, arranged for the limitations of five monophonic buzzers.

## Hardware

- Arduino Nano compatible with ATmega328P
- 5 passive / tone-capable buzzer modules with VCC, GND and I/O
- Breadboard and jumper wires
- USB connection for power and programming

The tested Nano requires:

- **Board:** Arduino Nano
- **Processor:** ATmega328P (Old Bootloader)

See [SCHEMA.md](SCHEMA.md) for the wiring.

## What was tested and learned

### The buzzer modules can play pitched tones

Although the modules are marked as low-level-trigger buzzer modules, the tested units respond correctly to square-wave drive and can reproduce different pitches. Frequencies roughly from 200 Hz to 3000 Hz were tested successfully.

The modules do not behave like full-range loudspeakers. Their apparent volume changes strongly with frequency, and different physical buzzers can also sound louder or quieter than one another.

For the final build, the strongest physical buzzer should be assigned to **D2**, because D2 carries the principal melody.

### Standard tone() is not enough for five simultaneous voices

On the standard Arduino AVR core, Tone.cpp is configured with one available tone channel and uses a hardware timer. Calling tone() is therefore useful for testing one buzzer at a time, but not for this five-voice arrangement.

The final sketch uses a custom oscillator engine instead:

- Timer1 runs an interrupt at 20 kHz.
- Each buzzer has its own phase accumulator.
- Each voice can therefore generate a different square-wave frequency at the same time.
- D2 through D6 are all on PORTD on the ATmega328P, so the five outputs can be updated efficiently.

Reference: Arduino AVR core Tone.cpp:
https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/Tone.cpp

### Speech synthesis was tested and rejected for this hardware

Talkie speech synthesis was tested on the Nano. A speech-like signal could be heard, but it was quiet and not intelligible enough through these buzzer modules.

The experiment showed that these modules are much better suited to simple tones and musical material than to reproducing speech.

### Five voices work better when they have different musical jobs

An early version assigned several buzzers very similar chord tones. That created extra loudness but not much extra musical information.

The arrangement was therefore redesigned so that each buzzer has a distinct function:

| Pin | Musical role | Behaviour |
| --- | --- | --- |
| D2 | Principal melody | Most recognizable line; kept prominent and almost legato |
| D3 | Bass / harmonic foundation | Shorter and more marked |
| D4 | Countermelody | Smoother independent line |
| D5 | Rhythmic figuration / arpeggio | Short, staccato notes |
| D6 | Harmonic pad | Longer sustained notes with rests |

This is a reduction for the available hardware, not a literal five-part transcription of the original orchestral score.

## Timing

The piece is organized on a **12/8 grid**.

The sketch uses:

- 144 eighth-note slots
- 462 ms per slot
- total programmed musical duration of about 66.5 seconds

## Volume balancing

During testing, the buzzers connected to D3, D4 and D5 sounded relatively strong, while the units originally connected to D2 and D6 sounded weaker.

The intended physical assignment is therefore:

- strongest buzzer -> D2
- another strong buzzer -> D3
- another strong buzzer -> D4
- remaining buzzers -> D5 and D6

Software duty cycle is also varied between voices to reduce masking of the principal melody. This is only an approximate acoustic balance because piezo buzzer response is not flat.

## Files

- [5_buzzerz_1_song.ino](5_buzzerz_1_song.ino) — Arduino sketch
- [SCHEMA.md](SCHEMA.md) — wiring and electrical notes

## Musical sources and references

Mozart's Requiem K.626 is public-domain repertoire. The project used public score material and a MIDI/transcription reference while developing the reduction.

- IMSLP, Mozart Requiem K.626:
  https://imslp.org/wiki/Requiem_in_D_minor%2C_K.626_(Mozart%2C_Wolfgang_Amadeus)
- Arduino AVR Tone.cpp:
  https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/Tone.cpp

The goal of this project is experimental: determine how much recognizable polyphonic music can be produced with five inexpensive buzzer modules and an ATmega328P without adding an audio amplifier or speaker.
