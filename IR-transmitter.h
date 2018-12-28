/*
 * IR-transmitter for Particle (2nd and 3th generation (Mesh)
 * Version 1.0 December, 2018
 * By Koen vab Erve
 * For details, see github
 *
 * Based on the IRRemote library: https://github.com/babean/IRRemote
 * Edited to remove the SparkIntervalTimer.h dependency
 * Only transmitter is left (on A0 pin, see #define TIMER_PWM_PIN)
 * 
 * Old header:
 * 
 * * IRremote
 * * Version 0.11 August, 2009
 * * Copyright 2009 Ken Shirriff
 * * For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
 * *
 * * Modified by Paul Stoffregen <paul@pjrc.com> to support other boards and timers
 * * Modified  by Mitra Ardron <mitra@mitra.biz>
 * * Added Sanyo and Mitsubishi controllers
 * * Modified Sony to spot the repeat codes that some Sony's send
 * *
 * * Interrupt code based on NECIRrcv by Joe Knapp
 * * http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
 * * Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
 * *
 * * JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
 *
 */

#include "Particle.h"


#define TIMER_PWM_PIN        A0

#define TOPBIT 0x80000000

// Pulse parms are *50-100 for the Mark and *50+100 for the space
// First MARK is the one after the long gap
// pulse parameters in usec
#define NEC_HDR_MARK	9000
#define NEC_HDR_SPACE	4500
#define NEC_BIT_MARK	560
#define NEC_ONE_SPACE	1600
#define NEC_ZERO_SPACE	560
#define NEC_RPT_SPACE	2250

#define SONY_HDR_MARK	2400
#define SONY_HDR_SPACE	600
#define SONY_ONE_MARK	1200
#define SONY_ZERO_MARK	600
#define SONY_RPT_LENGTH 45000
#define SONY_DOUBLE_SPACE_USECS  500  // usually ssee 713 - not using ticks as get number wrapround

// SA 8650B
#define SANYO_HDR_MARK	3500  // seen range 3500
#define SANYO_HDR_SPACE	950 //  seen 950
#define SANYO_ONE_MARK	2400 // seen 2400
#define SANYO_ZERO_MARK 700 //  seen 700
#define SANYO_DOUBLE_SPACE_USECS  800  // usually ssee 713 - not using ticks as get number wrapround
#define SANYO_RPT_LENGTH 45000

// #define MITSUBISHI_HDR_MARK	250  // seen range 3500
#define MITSUBISHI_HDR_SPACE	350 //  7*50+100
#define MITSUBISHI_ONE_MARK	1950 // 41*50-100
#define MITSUBISHI_ZERO_MARK  750 // 17*50-100

#define RC5_T1		889
#define RC5_RPT_LENGTH	46000

#define RC6_HDR_MARK	2666
#define RC6_HDR_SPACE	889
#define RC6_T1		444
#define RC6_RPT_LENGTH	46000

#define SHARP_BIT_MARK 245
#define SHARP_ONE_SPACE 1805
#define SHARP_ZERO_SPACE 795
#define SHARP_GAP 600000
#define SHARP_TOGGLE_MASK 0x3FF
#define SHARP_RPT_SPACE 3000

#define DISH_HDR_MARK 400
#define DISH_HDR_SPACE 6100
#define DISH_BIT_MARK 400
#define DISH_ONE_SPACE 1700
#define DISH_ZERO_SPACE 2800
#define DISH_RPT_SPACE 6200
#define DISH_TOP_BIT 0x8000

#define PANASONIC_HDR_MARK 3502
#define PANASONIC_HDR_SPACE 1750
#define PANASONIC_BIT_MARK 502
#define PANASONIC_ONE_SPACE 1244
#define PANASONIC_ZERO_SPACE 400

#define JVC_HDR_MARK 8000
#define JVC_HDR_SPACE 4000
#define JVC_BIT_MARK 600
#define JVC_ONE_SPACE 1600
#define JVC_ZERO_SPACE 550
#define JVC_RPT_LENGTH 60000


// IR SEND CLASS
class IRsend {
    public:
    IRsend() {}
    void sendNEC(unsigned long data, int nbits);
    void sendSony(unsigned long data, int nbits);
    void sendRaw(unsigned int buf[], int len, int hz);
    void sendRC5(unsigned long data, int nbits);
    void sendRC6(unsigned long data, int nbits);
    void sendDISH(unsigned long data, int nbits);
    void sendSharp(unsigned long data, int nbits);
    void sendPanasonic(unsigned int address, unsigned long data);
    void sendJVC(unsigned long data, int nbits, int repeat);
    // private:
    void enableIROut(int khz);
    void mark(int usec);
    void space(int usec);
};
