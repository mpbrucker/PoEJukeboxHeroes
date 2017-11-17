ParseMIDI.o: ParseMIDI.cpp
	g++ -lwiringPi -std=c++11 ParseMIDI.cpp -Lmidifile/lib -lmidifile -L libserial-0.6.0rc2/src/.libs -l serial -lpthread
