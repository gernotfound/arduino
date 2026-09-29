# Wiring — 5 buzzerz 1 song

## Components

- 1x Arduino Nano compatible, ATmega328P
- 5x tone-capable buzzer modules, each with **VCC / GND / I-O**
- Breadboard
- Jumper wires
- USB cable

No external amplifier or speaker is used.

## Connections

All five buzzer modules share the Nano's 5 V supply and ground. Each I/O pin is driven independently.

| Buzzer voice | Module I/O | Module VCC | Module GND | Musical role |
| --- | --- | --- | --- | --- |
| Buzzer 1 | D2 | 5V | GND | Principal melody |
| Buzzer 2 | D3 | 5V | GND | Bass |
| Buzzer 3 | D4 | 5V | GND | Countermelody |
| Buzzer 4 | D5 | 5V | GND | Rhythmic figuration / arpeggio |
| Buzzer 5 | D6 | 5V | GND | Harmonic pad |

## Connection diagram

```text
                         Arduino Nano
                    +--------------------+
                    |                    |
  Buzzer 1 I/O <----| D2                 |
  Buzzer 2 I/O <----| D3                 |
  Buzzer 3 I/O <----| D4                 |
  Buzzer 4 I/O <----| D5                 |
  Buzzer 5 I/O <----| D6                 |
                    |                    |
  +5 V rail   <-----| 5V                 |
  GND rail    <-----| GND                |
                    +--------------------+

          +5 V rail
             |
      +------+------+------+------+------+
      |      |      |      |      |
     VCC    VCC    VCC    VCC    VCC
      B1     B2     B3     B4     B5

          GND rail
             |
      +------+------+------+------+------+
      |      |      |      |      |
     GND    GND    GND    GND    GND
      B1     B2     B3     B4     B5
```

## Physical buzzer assignment

The five modules were not equally loud in testing.

Recommended arrangement:

1. Put the **strongest buzzer on D2**, because it carries the main melody.
2. Use another strong buzzer on D3.
3. Use another strong buzzer on D4.
4. D5 can use one of the remaining modules.
5. The weaker module can remain on D6, where it is used as a supporting pad.

Only the **I/O wires** need to be swapped when changing which physical buzzer is assigned to a voice. The common VCC and GND wiring can remain in place.

## Why D2-D6

On the ATmega328P, Arduino pins D2 through D6 correspond to PD2 through PD6 on PORTD.

Using five consecutive bits of the same hardware port allows the sketch to update all five outputs efficiently inside the audio interrupt.

## Low-level-trigger modules

The tested modules are labelled low-level trigger. In this project:

- HIGH is treated as the idle state.
- Rapid HIGH/LOW transitions generate the audible square wave.

The exact circuitry of inexpensive buzzer modules can vary, so this wiring description applies to the tested three-pin modules.

## Arduino IDE settings

Use:

```text
Board:     Arduino Nano
Processor: ATmega328P (Old Bootloader)
```

The tested Nano would not upload correctly with the newer bootloader selection.
