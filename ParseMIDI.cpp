// To compile: g++ -std=c++11 ParseMIDI.cpp -Lmidifile/lib -lmidifile -L libserial-0.6.0rc2/src/.libs -l serial -lpthread
#include "midifile/include/MidiFile.h"
#include "midifile/include/Options.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <SerialStream.h>
#include <SerialPort.h>
#include <unistd.h>

using namespace LibSerial;

typedef unsigned char uchar;

// user interface variables
Options options;
int     debugQ = 0;             // use with --debug option
int     maxcount = 100000;
double  tempo = 60.0;

// function declarations:
string      convertMidiFileToText (MidiFile& midifile);
void      setTempo              (MidiFile& midifile, int index, double& tempo);
int       getPin                (int key);
string      parseNote        (int pin, int dur);

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {

  // Open serial port and set options
  SerialPort mySerial("/dev/ttyACM0");
  mySerial.Open();
  mySerial.SetBaudRate(SerialPort::BAUD_115200);
  mySerial.SetParity(SerialPort::PARITY_NONE);
  mySerial.SetFlowControl(SerialPort::FLOW_CONTROL_NONE);

  usleep(2000000); // Wait for serial to reset
  cout << "Sleeping" << endl;

  //  mySerial.Write("8x0y8x1000y");

  options.process(argc, argv);
  MidiFile midifile(options.getArg(1));
  string outStr = convertMidiFileToText(midifile);
  cout << outStr << endl;
  mySerial.Write(outStr);
  mySerial.Close();
  return 0;
}

/*

*/
string convertMidiFileToText(MidiFile& midifile) {
  string strOut = "";

  midifile.absoluteTicks();
  midifile.joinTracks();

  vector<double> ontimes(128);
  vector<int> onvelocities(128);
  int i;
  for (i=0; i<128; i++) {
    ontimes[i] = -1.0;
    onvelocities[i] = -1;
  }

  double offtime = 0.0;

  int key = 0;
  int vel = 0;
  int command = 0;

  for (i=0; i<midifile.getNumEvents(0); i++) {
    command = midifile[0][i][0] & 0xf0;
    if (command == 0x90 && midifile[0][i][2] != 0) {
       // store note-on velocity and time
       key = midifile[0][i][1];
       ontimes[key] = midifile[0][i].tick * 60.0 / tempo /
             midifile.getTicksPerQuarterNote();
    } else if (command == 0x90 || command == 0x80) {
       // note off command write to output
       key = midifile[0][i][1]; // Get the current key being pressed
       int noteDur = ontimes[key] * 1000; // Get the current note time in seconds
       int pin = getPin(key); // Get the current pin
       strOut += parseNote(pin, noteDur);
       ontimes[key] = -1.0;
    }

    //check for tempo indication
    if (midifile[0][i][0] == 0xff &&
               midifile[0][i][1] == 0x51) {
       setTempo(midifile, i, tempo);
    }
  }
  cout << strOut << endl;
  return strOut;
}

/*
  Redefines the tempo based on a MIDI command.
*/
void setTempo(MidiFile& midifile, int index, double& tempo) {
   double newtempo = 0.0;
   static int count = 0;
   count++;

   MidiEvent& mididata = midifile[0][index];

   int microseconds = 0;
   microseconds = microseconds | (mididata[3] << 16);
   microseconds = microseconds | (mididata[4] << 8);
   microseconds = microseconds | (mididata[5] << 0);

   newtempo = 60.0 / microseconds * 1000000.0;
   if (count <= 1) {
      tempo = newtempo;
   } else if (tempo != newtempo) {
      cout << "; WARNING: change of tempo from " << tempo
           << " to " << newtempo << " ignored" << endl;
   }
}




int getPin(int key) {
  if (key >= 60) {
    return 8;
  }
  else {
    return 9;
  }
}

string parseNote(int pin, int dur) {
 return std::to_string(pin) + "x" + std::to_string(dur) + "y";
}
