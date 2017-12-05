// Note.h
// Defines a single musical note

#ifndef Note_h
#define Note_h

#include "Arduino.h"

class Note
{
  public:
    Note(int frequency, int duration);
    Note();

    int getFreq();
    int getDur();

  private:
    int _freq;
    int _dur;
};

#endif
