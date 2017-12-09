#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

const int SOLENOID_DELAY = 20; // The amount of time to keep the solenoid extended (ms)
bool isPlaying = false;

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);


void setup() {
  AFMS.begin();
  Serial.begin(115200);
  for (int i = 2; i <= 15; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  myMotor->setSpeed(50);
  myMotor->run(FORWARD);

}

void loop() {
  while (!Serial.available()) { // Wait for something on serial port
  }

  String strIn = Serial.readString();  //Read in what's on the serial port
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
      cur_note = curString.toInt()+1;
      if (cur_note > 12) { // If we're using a pin higher than the 12th pin, move to the analog pins.
        cur_note++;
      }
      pins[idx] =  // If we reached the first delimiting character, update the frequencies
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
  int prevDelta = 0;
  while (curNote != -1) {
    
    int noteOut = playNotes(pins, times, curNote, inLen, deltaTime, prevDelta);
    curNote = noteOut;
    prevDelta = deltaTime; 
    deltaTime = millis() - timeStart;
  }


}

int playNotes(int pins[], int times[], int startNote, int numNotes, int curTime, int prevTime) {
  int noteReturn = startNote; // The note we're currently looking at
  for (int i = 0; i < numNotes; i++) { // Iterate through the notes, starting at the current note
    if (curTime > times[i] && curTime < times[i] + SOLENOID_DELAY) { // If this note is currently being played
//      Serial.println("playing " + String(i) + " pin " + String(pins[i]) + " time " + String(times[i]));
      digitalWrite(pins[i], HIGH);
    }
    else if (curTime > times[i] && prevTime < times[i] + SOLENOID_DELAY){ // If the note has passed, push the solenoid back out
      digitalWrite(pins[i], LOW);
//      Serial.println("not playing" + String(i));
      noteReturn++;
    }
  }
//  Serial.println(noteReturn);
  if (noteReturn < numNotes) { // Return the current note
    return noteReturn;
  } else {
//    Serial.println("done");
    return -1; // We're done playing all of the notes
  }
}






