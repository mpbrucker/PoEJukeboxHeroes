/* 
 *  Note.cpp
 *  A class that defines a single musical note; used for keeping track of notes in a musical segment.
 */
#include "Arduino.h"
#include "Note.h"

 Note::Note() {
  _freq = 0;
  _dur = 0;
 }

 Note::Note(int frequency, int duration) {
  _freq = frequency;
  _dur = duration;
 }

 int Note::getFreq() {
  return _freq;
 }

int Note::getDur() {
  return _dur;
}

