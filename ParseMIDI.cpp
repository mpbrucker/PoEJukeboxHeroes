// To compile: g++ -lwiringPi -std=c++11 ParseMIDI.cpp -Lmidifile/lib -lmidifile -L libserial-0.6.0rc2/src/.libs -l serial -lpthread
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
#include <wiringPi.h>

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
  wiringPiSetup();
  pinMode(7, INPUT);
  pinMode(0, INPUT);
  pinMode(2, INPUT);
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
  cout << "Serial connection established." << endl;
  while (true) {
  cout << "Waiting for song selection." << endl;
  string songSelect = "";
  // Wait for button input
  while (songSelect == "") {
    bool button1Status = digitalRead(7);
    bool button2Status = digitalRead(0);
    bool button3Status = digitalRead(2);
    if (button1Status) {
      songSelect = "/home/pi/Documents/PoEJukeboxHeroes/twinkle_twinkle.mid";
    }
    else if (button2Status) {
      songSelect = "/home/pi/Documents/PoEJukeboxHeroes/mary_had_a_little_lamb_pno.mid";
    }
    else if (button3Status) {
      songSelect = "/home/pi/Documents/PoEJukeboxHeroes/three_blind_mice_pno.mid";
    }
  }

  MidiFile midifile(songSelect);
  string outStr = convertMidiFileToText(midifile);
  cout << songSelect << endl;
  cout << outStr << endl;

  // Open serial port and set options

  usleep(3000000); // Wait for serial to reset
  cout << "Sleeping" << endl;

  //mySerial.Write("1x0y1x600y5x1200y5x1800y6x2400y6x3000y5x3600y4x4800y4x5400y3x6000y3x6600y2x7200y2x7800y1x8400y5x9600y5x10200y4x10800y4x11400y3x12000y3x12600y2x13200y5x14400y5x15000y4x15600y4x16200y3x16800y3x17400y2x18000y1x19200y1x19800y5x20400y5x21000y6x21600y6x22200y5x22800y4x24000y4x24600y3x25200y3x25800y2x26400y2x27000y1x27600y");

  //options.process(argc, argv);
  mySerial.Write(outStr);
  usleep(30000000);
  }
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
