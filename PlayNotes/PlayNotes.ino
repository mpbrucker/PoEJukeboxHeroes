
#import "Note.h"

const int SOLENOID_DELAY = 10; // The amount of time to keep the solenoid extended (ms)


void setup() {
  Serial.begin(115200);
  for (int i = 1; i <= 13; i++) {
    pinMode(i,OUTPUT);
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

  int freqs[inLen];
  int durs[inLen];

  int idx = 0;
  int curFreq;
  int curDur;
  String curString = "";
  for (int i = 0; i < strIn.length(); i++) { // Iterate through the input string
    char charIn = char(strIn[i]); // Get the current character
    if (charIn == 'x') { 
      freqs[idx] = curString.toInt(); // If we reached the first delimiting character, update the frequencies
      curString = ""; // Reset the current string
    }
    else if (charIn == 'y') {
      durs[idx] = curString.toInt(); // If we reached the second delimiting character, update the durations
      curString = "";
      idx++;
    }
    else {
      curString += charIn; // Add to the current string we're looking at
    }
  }
//
//  for (int i = 0; i < inLen; i++) {
//    Serial.println(String(freqs[i]) + " " + String(durs[i]));
//  }

}


void playNote(int freq, int dur) {
  int curTime = millis()
  int pin = map(freq, 0, 1000, 1, 13); // Map from frequency to pin
  digitalWrite(pin, HIGH);
  delay(SOLENOID_DELAY);
  digitalWrite(pin, LOW);
  while (millis() - curTime < dur) {
    __asm__("nop\n\t");
  }

  
}








