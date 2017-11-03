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
void      checkOptions          (Options& opts, int argc, char** argv);
void      usage                 (const char* command);
int       getPin                (int key);
void      sendNoteSerial        (int pin, int dur);

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {

  SerialPort mySerial("/dev/ttyACM0");
  mySerial.Open();
  mySerial.SetBaudRate(SerialPort::BAUD_115200);
  mySerial.SetParity(SerialPort::PARITY_NONE);
  mySerial.SetFlowControl(SerialPort::FLOW_CONTROL_NONE);
  usleep(2000000);
  cout << "Sleeping" << endl;

  //  mySerial.Write("8x0y8x1000y");


  checkOptions(options, argc, argv);
  MidiFile midifile(options.getArg(1));
  string outStr = convertMidiFileToText(midifile);
  cout << outStr << endl;
  mySerial.Write(outStr);
  // usleep(2000000);
  cout << "Sleeping again" << endl;
  mySerial.Close();
  return 0;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// convertMidiFileToText --
//

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
       vel = midifile[0][i][2];
       ontimes[key] = midifile[0][i].tick * 60.0 / tempo /
             midifile.getTicksPerQuarterNote();
       onvelocities[key] = vel;
    } else if (command == 0x90 || command == 0x80) {
       // note off command write to output
       key = midifile[0][i][1];
       if (key >= 60) { // Extract out the melody, then output to serial
         int noteDur = ontimes[key] * 1000;
         int pin = getPin(key);
         strOut += std::to_string(pin) + "x" + std::to_string(noteDur) + "y";
        //  cout << strOut << endl;
         sendNoteSerial(pin, noteDur);
       }
       else {
         int noteDur = ontimes[key] * 1000;
         int pin = 9;
         strOut += std::to_string(pin) + "x" + std::to_string(noteDur) + "y";
         sendNoteSerial(pin, noteDur);
       }

       onvelocities[key] = -1;
       ontimes[key] = -1.0;
    }

    // check for tempo indication
    if (midifile[0][i][0] == 0xff &&
               midifile[0][i][1] == 0x51) {
       setTempo(midifile, i, tempo);
    }
  }
  cout << strOut << endl;
  return strOut;
}


//////////////////////////////
//
// setTempo -- set the current tempo
//

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



//////////////////////////////
//
// checkOptions --
//

void checkOptions(Options& opts, int argc, char* argv[]) {
   opts.define("author=b",  "author of program");
   opts.define("version=b", "compilation info");
   opts.define("h|help=b",  "short description");

   opts.define("debug=b",  "debug mode to find errors in input file");
   opts.define("max=i:100000", "maximum number of notes expected in input");

   opts.process(argc, argv);

   // handle basic options:
   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, 22 Jan 2002" << endl;
      exit(0);
   } else if (opts.getBoolean("version")) {
      cout << argv[0] << ", version: 22 Jan 2002" << endl;
      cout << "compiled: " << __DATE__ << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand().data());
      exit(0);
   }



   debugQ = opts.getBoolean("debug");
   maxcount = opts.getInteger("max");

   if (opts.getArgCount() != 1) {
      usage(opts.getCommand().data());
      exit(1);
   }

}

//////////////////////////////
//
// usage --
//

void usage(const char* command) {
   cout << "Usage: " << command << " midifile" << endl;
}

int getPin(int key) {
  return 8;
}

void sendNoteSerial(int pin, int dur) {
 cout << pin << "\t" << dur << endl;
}
