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
  while (true) {
    try {
      mySerial.Open();
      mySerial.SetBaudRate(SerialPort::BAUD_115200);
      mySerial.SetParity(SerialPort::PARITY_NONE);
      mySerial.SetFlowControl(SerialPort::FLOW_CONTROL_NONE);
      break;
    } catch (SerialPort::OpenFailed exc) {
      cout << "Port not found" << endl;
      usleep(1000000);
    }
  }

  usleep(5000000); // Wait for serial to reset
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
       int onTime = (midifile[0][i].tick * 60.0 / tempo / midifile.getTicksPerQuarterNote()) * 1000;

       int pin = getPin(key); // Get the current pin
       cout << key << "\t" << onTime << endl;
       if (pin != -1)
       {
         strOut += parseNote(pin, onTime);
       }
      //  cout << midifile[0][1].tick << endl;
      //  ontimes[key] = midifile[0][i].tick * 60.0 / tempo /
      //        midifile.getTicksPerQuarterNote();
    } else if (command == 0x90 || command == 0x80) {
       // note off command write to output
      //  key = midifile[0][i][1]; // Get the current key being pressed
      //  int noteDur = ontimes[key] * 1000; // Get the current note time in seconds
      //  cout << key << "\t" << ontimes[key] << endl;
      //  int pin = getPin(key); // Get the current pin
      //  strOut += parseNote(pin, noteDur);
      //  ontimes[key] = -1.0;
    }

    //check for tempo indication
    if (midifile[0][i][0] == 0xff &&
               midifile[0][i][1] == 0x51) {
       setTempo(midifile, i, tempo);
    }
  }
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
  int octave = key/12;
  if (octave >= 5) { // For now, just look at notes within the melody
    int standNote = key - (octave*12);
    cout << standNote << endl;
    switch(standNote) {
      case 0:
        return 1;
        break;
      case 2:
        return 2;
        break;
      case 4:
        return 3;
        break;
      case 5:
        return 4;
        break;
      case 7:
        return 5;
        break;
      case 9:
        return 6;
        break;
      case 11:
        return 7;
        break;
      case 12:
        return 8;
        break;
      case 14:
        return 9;
        break;
      case 16:
        return 10;
        break;
      case 17:
        return 11;
        break;
      case 19:
        return 12;
        break;
      case 21:
        return 13;
        break;
      default:
        return -1;
    }
  }
  return -1;
}

string parseNote(int pin, int dur) {
  string returnStr = std::to_string(pin) + "x" + std::to_string(dur) + "y";
  return returnStr;
}
