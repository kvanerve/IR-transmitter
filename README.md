# IR-transmitter

IR-transmitter for Particle (2nd and 3th generation (Mesh)

Version 1.0 December, 2018
By Koen van Erve
For details, see https://github.com/kvanerve/IR-transmitter
Based on the IRRemote library: https://github.com/babean/IRRemote
Edited to remove the SparkIntervalTimer.h dependency
Only transmitter is left (on A0 pin, see #define TIMER_PWM_PIN)

Old header:

* IRremote
* Version 0.11 August, 2009
* Copyright 2009 Ken Shirriff
* For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
*
* Modified by Paul Stoffregen <paul@pjrc.com> to support other boards and timers
* Modified  by Mitra Ardron <mitra@mitra.biz>
* Added Sanyo and Mitsubishi controllers
* Modified Sony to spot the repeat codes that some Sony's send
*
* Interrupt code based on NECIRrcv by Joe Knapp
* http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
* Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
*
* JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
