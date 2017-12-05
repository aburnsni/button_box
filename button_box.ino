/**
 * Copyright (c) 2017 Andrew Burns
 * Licensed under The MIT License (MIT)
 * License: ./LICENSE
 * Filenames: tippeny_guitar.ino, midi_chords.h, midi_notes.h
 * Board: Arduino Uno R3
 * Shield: Adafruit MPR121 touch shield
 */

#include <Wire.h>
#include "Adafruit_MPR121.h"
#include <MIDI.h>
#include "midi_notes.h"
#include "midi_chords.h"

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

// Change these only!!!
const bool DEBUG = 1;


int strum = 5;  // delay between each note of strum

// All I Want For Christmas is You
//char style = 'c';   // Valid options = "n" for notes or "c" for chords
//int* song[] = {CHORD_G, CHORD_Em, CHORD_C, CHORD_D, CHORD_Cm6_Eb, CHORD_B7, CHORD_E7, CHORD_Am9, CHORD_Am7b5_D, CHORD_D7, CHORD_D7add13, CHORD_Em7};
//int midiChannel[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
//int instruments[] = {26, 999, 999, 999, 999, 999, 999, 999, 999, 999 /*Drums*/, 999, 999, 999, 999, 999, 999};

// Jingle Bell Rock
char style = 'c';   // Valid options = "n" for notes or "c" for chords
// int* song[] = {CHORD_Ab, CHORD_Bbm7, CHORD_Eb7, CHORD_Eb7s5, CHORD_Db, CHORD_Ddim, CHORD_Fm7, CHORD_Bb7, CHORD_F7s5, CHORD_F7, CHORD_Dbm6, BELLS};
// int midiChannel[] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 10};
int instruments[] = {999, 49, 999, 999, 999, 999, 999, 999, 999, 999 /*Drums*/, 999, 999, 999, 999, 999, 999};
/* Reordered for landscape mpr121.  3,2,1,0  7,6,5,4  11,10,9,8 */
int* song[] = {CHORD_Eb7s5, CHORD_Eb7, CHORD_Bbm7, CHORD_Ab, CHORD_Bb7, CHORD_Fm7, CHORD_Ddim, CHORD_Db, BELLS, CHORD_Dbm6, CHORD_F7, CHORD_F7s5};
int midiChannel[] = {2, 2, 2, 2, 2, 2, 2, 2, 10, 2, 2, 2};


// Simple Chords
//char style = 'c';
//int* song[] = {CHORD_C, CHORD_Am, CHORD_F, CHORD_G, CHORD_Dm, CHORD_Em, CHORD_F7, CHORD_G7, DRUM_BASS, DRUM_SIDE, CYM_1, CYM_2};  // 0's in song prevent that key from sounding
//int midiChannel[] = {1, 1, 1, 1, 1, 1, 1, 1, 10, 10, 10, 10};  // MIDI channel 10 for drums
//int instruments[] = {26, 999, 999, 999, 999, 999, 999, 999, 999, 999 /*Drums*/, 999, 999, 999, 999, 999, 999};  // instruments for each channel.  Setting to 999 prevents change

// Diamonds are a Girls Best Friend
//char style = 'c';
//int* song[] = {CHORD_E, CHORD_B, CHORD_B7, CHORD_Gbm, CHORD_Gb, CHORD_E7, CHORD_A, CHORD_Dbm, CHORD_Gbm, CHORD_Db, CHORD_Am, 0}; 
//int midiChannel[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
//int instruments[] = {26, 999, 999, 999, 999, 999, 999, 999, 999, 999 /*Drums*/, 999, 999, 999, 999, 999, 999};

// C Major scale
//char style = 'n';
//int* song[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5};
//int midiChannel[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; // midi channel for each button
//int instruments[] = {102, 999, 999, 999, 999, 999, 999, 999, 999, 999 /*Drums*/, 999, 999, 999, 999, 999, 999};

// // Button box
// char style = 'c';
// int* song[] = {CHORD_C, CHORD_C, CHORD_C, CHORD_C, CHORD_F, CHORD_F, CHORD_F, CHORD_F, CHORD_G, CHORD_G, CHORD_G, CHORD_G};
// int midiChannel[] = {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4};
// int instruments[] = {26, 102, 35, 75, 999, 999, 999, 999, 999, 999 /*Drums*/, 999, 999, 999, 999, 999, 999};


MIDI_CREATE_DEFAULT_INSTANCE();
void setup() {
  while (!Serial);        // needed to keep leonardo/micro from starting too fast!

  MIDI.begin();
  if (DEBUG) {
    Serial.begin(115200);  // needed for hairless midi
  }
  if (!cap.begin(0x5A)) {
    // Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  delay(1000);
  MIDIsoftreset();  // Midi Reset
  delay(200);
  for (uint8_t i = 0; i < 16; i++) {  // Set instruments for all 16 MIDI channels
    if (instruments[i] < 128) {
      MIDI.sendProgramChange(instruments[i], i+1);
    }
  }
}

void loop() {
  // Get the currently touched pads
  currtouched = cap.touched();

  for (uint8_t i = 0; i < 12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      if (song[i]) {
        if (style == 'n') {
          MIDI.sendNoteOn(song[i], 100, midiChannel[i]);
        } else if (style == 'c' && song[i]) {
          playChord(song[i], midiChannel[i]);
        }
      }
    }
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      if (song[i]) {
        if (style == 'n') {
          MIDI.sendNoteOff(song[i], 100, midiChannel[i]);
        } else if (style == 'c' && song[i]) {
          stopChord(song[i], midiChannel[i]);
        }
      }
    }
  }

  // reset our state
  lasttouched = currtouched;

  // put a delay so it isn't overwhelming
  delay(20);
}

void playChord(int i[], int channel) {
  for (uint8_t note = 0; note < 6; note++) {
    if (i[note]) {
      MIDI.sendNoteOn((i[note]), 100, channel);
      delay(strum);
    }
  }
}
void stopChord(int i[], int channel) {
  for (uint8_t note = 0; note < 6; note++) {
    if (i[note]) {
      MIDI.sendNoteOff((i[note]), 100, channel);
      delay(strum);
    }
  }
}

void MIDIsoftreset()  // switch off ALL notes on channel 1 to 16
{
  for (int channel = 0; channel < 16; channel++)
  {
    MIDI.sendControlChange(123, 0, channel);
  }
}

