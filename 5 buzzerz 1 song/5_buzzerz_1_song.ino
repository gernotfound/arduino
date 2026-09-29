#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

/*
   ============================================================
   MOZART - LACRIMOSA
   ARRANGIAMENTO A 5 BUZZER - VERSIONE 2

   Arduino Nano ATmega328P - 16 MHz

   D2 = MELODIA PRINCIPALE
   D3 = BASSO
   D4 = CONTROMELODIA
   D5 = FIGURAZIONE / ARPEGGIO
   D6 = SOSTEGNO ARMONICO LUNGO

   Durata musicale: circa 66,5 secondi

   Esecuzione UNA SOLA VOLTA.
   ============================================================
*/

const byte NUM_VOCI = 5;

const uint8_t BUZZER_MASK =
  _BV(PD2) |
  _BV(PD3) |
  _BV(PD4) |
  _BV(PD5) |
  _BV(PD6);

const uint32_t SAMPLE_RATE = 20000UL;

volatile uint16_t fase[NUM_VOCI] = {
  0, 0, 0, 0, 0
};

volatile uint16_t incremento[NUM_VOCI] = {
  0, 0, 0, 0, 0
};

volatile uint8_t gateAttivo[NUM_VOCI] = {
  0, 0, 0, 0, 0
};

const uint16_t dutyVoce[NUM_VOCI] = {
  32768,   // D2 50% - melodia
  21627,   // D3 33% - basso
  20316,   // D4 31% - contromelodia
  17039,   // D5 26% - arpeggio
  24248    // D6 37% - pad
};

const uint16_t gateMs[NUM_VOCI] = {
  440,   // D2 - quasi legato
  360,   // D3 - marcato
  452,   // D4 - legato
  245,   // D5 - staccato
  462    // D6 - sostenuto
};

const uint16_t frequenzeMIDI[] PROGMEM = {
   87, 93, 98, 104, 110, 117, 123, 131, 139, 147,
  156, 165, 175, 185, 196, 208, 220, 233, 247, 262,
  277, 294, 311, 330, 349, 370, 392, 415, 440, 466,
  494, 523, 554, 587
};

// D2 - melodia principale
const uint8_t parteD2[] PROGMEM = {
  0,61,62,0,69,70,0,62,61,0,72,70,
  0,69,74,0,70,67,0,64,65,0,69,64,
  0,61,62,0,69,70,0,62,61,0,70,69,
  0,61,62,0,69,70,0,62,61,0,70,69,
  0,50,45,0,52,45,0,53,50,0,55,48,
  57,53,0,59,52,0,60,57,0,62,55,0,
  0,64,60,0,64,60,0,65,60,0,66,57,
  0,67,59,0,69,58,0,69,57,0,57,45,
  0,52,45,0,58,57,0,50,45,0,65,62,
  0,55,46,0,62,64,0,57,45,0,53,62,
  0,63,51,0,67,63,0,63,53,0,62,58,
  0,62,50,0,65,64,0,64,53,0,62,68
};

// D3 - basso
const uint8_t parteD3[] PROGMEM = {
  50,0,0,53,0,0,52,0,0,52,0,0,
  53,0,0,49,0,0,45,0,0,45,0,0,
  53,57,62,53,65,62,50,50,57,52,52,55,
  53,57,62,53,65,62,50,50,57,52,52,55,
  41,45,50,45,49,52,45,45,50,48,52,55,
  48,53,57,44,47,50,45,52,57,50,55,62,
  48,52,55,48,52,55,48,53,57,48,51,57,
  50,50,55,53,57,62,53,57,62,52,55,61,
  45,52,57,45,58,57,45,53,57,45,65,62,
  52,55,61,52,62,53,58,64,53,58,62,53,
  51,58,63,51,67,63,51,58,63,53,58,62,
  53,57,62,53,65,62,53,58,64,53,57,56
};

// D4 - contromelodia
const uint8_t parteD4[] PROGMEM = {
  65,0,0,65,0,0,64,0,0,64,0,0,
  65,0,0,64,0,0,57,0,0,57,0,0,
  57,57,62,65,65,62,62,62,57,64,64,55,
  57,57,62,65,65,62,62,62,57,64,64,55,
  57,57,62,61,61,64,62,57,62,64,64,55,
  60,65,57,56,59,62,64,64,57,59,55,62,
  60,64,55,60,64,55,57,65,57,57,63,57,
  59,62,55,62,57,62,62,57,62,61,55,61,
  64,64,57,57,58,57,65,65,57,57,65,62,
  61,55,61,64,62,58,58,64,65,58,62,65,
  63,58,63,63,67,63,58,58,63,62,58,62,
  62,57,62,65,65,62,65,58,64,65,57,56
};

// D5 - arpeggio / figurazione
const uint8_t parteD5[] PROGMEM = {
  62,0,0,57,0,0,55,0,0,55,0,0,
  57,0,0,61,0,0,62,0,0,55,0,0,
  62,57,62,65,65,62,57,62,57,55,64,55,
  62,57,62,65,65,62,57,62,57,55,64,55,
  62,57,62,57,61,64,57,57,62,55,64,55,
  57,65,57,56,59,62,57,64,57,55,55,62,
  55,64,55,55,64,55,60,65,57,60,63,57,
  55,62,55,65,57,62,65,57,62,64,55,61,
  57,64,57,57,58,57,57,65,57,57,65,62,
  55,55,61,64,62,65,58,64,62,58,62,65,
  58,58,63,63,67,63,63,58,63,58,58,62,
  57,57,62,65,65,62,58,58,64,57,57,56
};

// D6 - pad / sostegno armonico
const uint8_t parteD6[] PROGMEM = {
  62,62,62,62,0,0,55,55,55,55,0,0,
  57,57,57,57,0,0,62,62,62,62,0,0,
  62,62,62,62,0,0,57,57,57,57,0,0,
  62,62,62,62,0,0,57,57,57,57,0,0,
  62,62,62,62,0,0,57,57,57,57,0,0,
  65,65,65,65,0,0,57,57,57,57,0,0,
  55,55,55,55,0,0,60,60,60,60,0,0,
  62,62,62,62,0,0,65,65,65,65,0,0,
  57,57,57,57,0,0,57,57,57,57,0,0,
  55,55,55,55,0,0,58,58,58,58,0,0,
  58,58,58,58,0,0,63,63,63,63,0,0,
  65,65,65,65,0,0,64,64,64,64,0,0
};

const uint16_t NUM_EVENTI =
  sizeof(parteD2) / sizeof(parteD2[0]);

const uint32_t DURATA_OTTAVO = 462UL;

uint16_t frequenzaDaMIDI(uint8_t nota) {
  if (nota == 0) {
    return 0;
  }

  while (nota < 55) {
    nota += 12;
  }

  if (nota < 41 || nota > 74) {
    return 0;
  }

  return pgm_read_word(
    &frequenzeMIDI[nota - 41]
  );
}

uint16_t calcolaIncremento(uint16_t frequenza) {
  if (frequenza == 0) {
    return 0;
  }

  return (
    ((uint32_t)frequenza * 65536UL)
    + SAMPLE_RATE / 2
  ) / SAMPLE_RATE;
}

uint8_t leggiNota(byte voce, uint16_t posizione) {
  switch (voce) {
    case 0:
      return pgm_read_byte(&parteD2[posizione]);

    case 1:
      return pgm_read_byte(&parteD3[posizione]);

    case 2:
      return pgm_read_byte(&parteD4[posizione]);

    case 3:
      return pgm_read_byte(&parteD5[posizione]);

    case 4:
      return pgm_read_byte(&parteD6[posizione]);
  }

  return 0;
}

void caricaSlot(uint16_t posizione) {
  uint16_t nuovoIncremento[NUM_VOCI];
  uint8_t nuovaNota[NUM_VOCI];

  for (byte voce = 0; voce < NUM_VOCI; voce++) {
    nuovaNota[voce] =
      leggiNota(voce, posizione);

    uint16_t f =
      frequenzaDaMIDI(nuovaNota[voce]);

    nuovoIncremento[voce] =
      calcolaIncremento(f);
  }

  noInterrupts();

  for (byte voce = 0; voce < NUM_VOCI; voce++) {
    if (
      incremento[voce] != nuovoIncremento[voce]
    ) {
      fase[voce] = 0;
    }

    incremento[voce] =
      nuovoIncremento[voce];

    if (nuovaNota[voce] == 0) {
      gateAttivo[voce] = 0;
    }
    else {
      gateAttivo[voce] = 1;
    }
  }

  interrupts();
}

void silenzioTotale() {
  noInterrupts();

  for (byte i = 0; i < NUM_VOCI; i++) {
    gateAttivo[i] = 0;
    incremento[i] = 0;
  }

  PORTD |= BUZZER_MASK;

  interrupts();
}

ISR(TIMER1_COMPA_vect) {
  uint8_t uscita =
    PORTD | BUZZER_MASK;

  for (byte voce = 0; voce < NUM_VOCI; voce++) {
    if (
      incremento[voce] != 0
      &&
      gateAttivo[voce]
    ) {
      fase[voce] += incremento[voce];

      if (
        fase[voce] < dutyVoce[voce]
      ) {
        uscita &=
          ~_BV(voce + 2);
      }
    }
  }

  PORTD =
    (PORTD & ~BUZZER_MASK)
    |
    (uscita & BUZZER_MASK);
}

void inizializzaAudio() {
  DDRD |= BUZZER_MASK;

  PORTD |= BUZZER_MASK;

  noInterrupts();

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  OCR1A =
    (F_CPU / SAMPLE_RATE) - 1;

  TCCR1B |= _BV(WGM12);

  TCCR1B |= _BV(CS10);

  TIMSK1 |= _BV(OCIE1A);

  interrupts();
}

uint16_t slotCorrente = 0;

unsigned long inizioBrano = 0;

bool branoFinito = false;

void setup() {
  inizializzaAudio();

  delay(800);

  slotCorrente = 0;

  caricaSlot(0);

  inizioBrano =
    millis();
}

void loop() {
  if (branoFinito) {
    return;
  }

  unsigned long trascorso =
    millis() - inizioBrano;

  uint16_t nuovoSlot =
    trascorso / DURATA_OTTAVO;

  if (nuovoSlot >= NUM_EVENTI) {
    silenzioTotale();

    branoFinito = true;

    return;
  }

  if (nuovoSlot != slotCorrente) {
    slotCorrente =
      nuovoSlot;

    caricaSlot(
      slotCorrente
    );
  }

  uint16_t posizioneDentroSlot =
    trascorso % DURATA_OTTAVO;

  for (byte voce = 0; voce < NUM_VOCI; voce++) {
    if (
      gateAttivo[voce]
      &&
      posizioneDentroSlot >= gateMs[voce]
    ) {
      gateAttivo[voce] = 0;
    }
  }
}
