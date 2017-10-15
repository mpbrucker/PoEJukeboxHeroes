
#import "Note.h"


void setup() {
  Serial.begin(9600);
  Note note1(440, 1000);
  Serial.println(note1.getFreq());
  Serial.println(note1.getDur());
}

void loop() {

}

// Takes a stream of note inputs and breaks it up into a series of notes and times
void streamNotes() {
  char startChar = char(Serial.read());
  if (startChar == 's') { // Ensure we're getting a well-formed series of bytes
    int streamLen = Serial.available()/8; // Since each note/time combo should be 8 bytes, 
    Note notes[streamLen]; 
    int idx = 0;
    int curFreq;
    int curDur;
    String curString = "";
    while (Serial.available()) {
      char charIn = char(Serial.read());
      if (charIn == 'x') {
        curFreq = curString.toInt();
        curString = "";
      }
      else if (charIn == 'y') {
        curDur = curString.toInt();
        curString = "";
        notes[idx] = Note(curFreq, curDur);
        idx++;
      }
      else {
        curString += charIn;
      }
    }
    return 
  }
  Serial.flush();
}

