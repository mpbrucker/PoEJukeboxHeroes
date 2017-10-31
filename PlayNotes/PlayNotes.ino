
#import "Note.h"

const int SOLENOID_DELAY = 50; // The amount of time to keep the solenoid extended (ms)


void setup() {
  Serial.begin(115200);
  for (int i = 1; i <= 13; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }

}

void loop() {
  while (!Serial.available()) { // Wait for something on serial port
  }
  String strIn = Serial.readString(); // Read in what's on the serial port
  int inLen = 0;
  for (int i = 0; i < strIn.length(); i++) {
    if (strIn[i] == 'x') {
      inLen++; // Record the number of notes being read
    }
  }

  int pins[inLen]; // Set up list of pins
  int times[inLen]; // Set up list of durations

  int idx = 0;
  int curFreq;
  int curDur;
  String curString = "";
  for (int i = 0; i < strIn.length(); i++) { // Iterate through the input string
    char charIn = char(strIn[i]); // Get the current character
    if (charIn == 'x') {
      pins[idx] = curString.toInt(); // If we reached the first delimiting character, update the frequencies
      curString = ""; // Reset the current string
    }
    else if (charIn == 'y') {
      times[idx] = curString.toInt(); // If we reached the second delimiting character, update the durations
      curString = "";
      idx++;
    }
    else {
      curString += charIn; // Add to the current string we're looking at
    }
  }

  int curNote = 0; // Iterate through the list of notes and play each note
  int timeStart = millis();
  int deltaTime = millis() - timeStart;
//  while (curNote != -1) {
//    curNote = playNotes(pins, times, curNote, deltaTime);
//    deltaTime = millis() - timeStart;
//  }
    do {
      bool noteDone = playNote(pins[curNote], times[curNote], timeStart); // Based on the start time of the note, figure out whether we're done playing this note
      if (noteDone) {
        timeStart = millis(); // Reset the start time
        curNote++; // Move to the next note
      }
    } while (curNote < inLen);

}

int playNotes(int pins[], int times[], int startNote, int curTime) {
  int noteReturn = startNote; // The note we're currently looking at
  for (int i = startNote; i < sizeof(pins); i++) { // Iterate through the notes, starting at the current note
    if (curTime > times[i] && curTime < times[i] + SOLENOID_DELAY) { // If this note is currently being played
      digitalWrite(pins[i], LOW);
    }
    else {
      digitalWrite(pins[i], HIGH);
      if (curTime > times[i]) { // If the note's time has passed, move on to the next note
        noteReturn++;
      }
    }
  }
  if (noteReturn < sizeof(pins)) { // Return the current note
    return noteReturn;
  }
  else {
    return -1; // We're done playing all of the notes
  }
}


// Plays the current note. Based on startTime, returns true if the note is finished playing, false otherwise
bool playNote(int pin, int dur, int startTime) {
  int diffTime = millis() - startTime; // The difference between the current time and start time
  if (dur > SOLENOID_DELAY) {
    if (diffTime <= dur) {
      if (diffTime < SOLENOID_DELAY) {
        digitalWrite(pin, LOW);
      } else {
        digitalWrite(pin, HIGH);
      }
      return false;
    } else {
      return true; // We're done playing the note
    }
  }


}








