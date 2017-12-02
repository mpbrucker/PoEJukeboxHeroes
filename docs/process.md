---
layout: default
title: Our Process
description: A sprint-by-sprint look at how we got to where we are.
category: process
---

## Sprint 1
Our primary focus during this sprint was to get the mechanical specifics of launching marbles working. We knew that consistently launching marbles would prove to be a fairly difficult task, so we decided to focus our efforts on that. In addition, we wanted to integrate the electrical and basic software components to have two working marble launchers by the end of the sprint.


### Mechanical

![The solenoid circuit design.](media/mech1.jpg)


Over the course of this sprint, we went through quite a few iterations of our marble launcher designs. Early into the sprint, we chose to use solenoids to actuate the marble launching, as they provide the most precise timing for launching marbles. From that point, we prototyped, CADed, and 3D printed about 6 different iterations of chute designs, eventually ending up with one that worked fairly consistently. In addition, we built a basic frame that contained the marbles in the marble hopper, held the xylophone, and attached to the marble chutes. The primary issue we encountered mechanical-wise in this sprint was actuation; the solenoids had consistency issues with launching balls, and they also heated up very quickly due to always drawing current.

### Electrical

![The solenoid circuit design.](media/elec1.jpg)

The electrical design during this sprint was very straightforward. Since our main task was to get ball launching working, the main electrical concern was how to actuate the solenoids. Fortunately, we were able to figure out a design that uses an N-channel MOSFET to actuate the solenoids, and using an external power supply, it was a fairly simple process to actuate them using the Arduino.

### Software

This sprint, we were able to get the basic note playing working via an Arduino. We began by writing code to process messages from the Arduino's serial port, then using that message to actuate the solenoids at given times. We got a basic version working, but with some issues: we weren't able to get more than one note to play simultaneously, and every note still had to be manually input through the Arduino's serial monitor. We also wrote some preliminary code in C++ to process MIDI files and send them to the Arduino through serial.
