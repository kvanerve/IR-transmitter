/*
 * IR-transmitter for Particle (2nd and 3th generation (Mesh)
 * Version 1.0 December, 2018
 * By Koen van Erve
 * For details, see https://github.com/kvanerve/IR-transmitter
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

#include "application.h"
#include "IR-transmitter.h"

void IRsend::sendNEC(unsigned long data, int nbits) {
    enableIROut(38);
    mark(NEC_HDR_MARK);
    space(NEC_HDR_SPACE);
    for (int i = 0; i < nbits; i++) {
        if (data & TOPBIT) {
            mark(NEC_BIT_MARK);
            space(NEC_ONE_SPACE);
        } else {
            mark(NEC_BIT_MARK);
            space(NEC_ZERO_SPACE);
        }
        data <<= 1;
    }
    mark(NEC_BIT_MARK);
    space(0);
}

void IRsend::sendSony(unsigned long data, int nbits) {
    enableIROut(40);
    mark(SONY_HDR_MARK);
    space(SONY_HDR_SPACE);
    data = data << (32 - nbits);
    for (int i = 0; i < nbits; i++) {
        if (data & TOPBIT) {
            mark(SONY_ONE_MARK);
            space(SONY_HDR_SPACE);
        } else {
            mark(SONY_ZERO_MARK);
            space(SONY_HDR_SPACE);
        }
        data <<= 1;
    }
}

void IRsend::sendRaw(unsigned int buf[], int len, int hz) {
    enableIROut(hz);
    for (int i = 0; i < len; i++) {
        if (i & 1) {
            space(buf[i]);
        } else {
            mark(buf[i]);
        }
    }
    space(0); // Just to be sure
}

// Note: first bit must be a one (start bit)
void IRsend::sendRC5(unsigned long data, int nbits) {
    enableIROut(36);
    data = data << (32 - nbits);
    mark(RC5_T1); // First start bit
    space(RC5_T1); // Second start bit
    mark(RC5_T1); // Second start bit
    for (int i = 0; i < nbits; i++) {
        if (data & TOPBIT) {
            space(RC5_T1); // 1 is space, then mark
            mark(RC5_T1);
        } else {
            mark(RC5_T1);
            space(RC5_T1);
        }
        data <<= 1;
    }
    space(0); // Turn off at end
}

// Caller needs to take care of flipping the toggle bit
void IRsend::sendRC6(unsigned long data, int nbits) {
    enableIROut(36);
    data = data << (32 - nbits);
    mark(RC6_HDR_MARK);
    space(RC6_HDR_SPACE);
    mark(RC6_T1); // start bit
    space(RC6_T1);
    int t;
    for (int i = 0; i < nbits; i++) {
        if (i == 3) {
            // double-wide trailer bit
            t = 2 * RC6_T1;
        } else {
            t = RC6_T1;
        }
        if (data & TOPBIT) {
            mark(t);
            space(t);
        } else {
            space(t);
            mark(t);
        }
        data <<= 1;
    }
    space(0); // Turn off at end
}
void IRsend::sendPanasonic(unsigned int address, unsigned long data) {
    enableIROut(35);
    mark(PANASONIC_HDR_MARK);
    space(PANASONIC_HDR_SPACE);

    for(int i=0;i<16;i++)
    {
        mark(PANASONIC_BIT_MARK);
        if (address & 0x8000) {
            space(PANASONIC_ONE_SPACE);
        } else {
            space(PANASONIC_ZERO_SPACE);
        }
        address <<= 1;
    }
    for (int i=0; i < 32; i++) {
        mark(PANASONIC_BIT_MARK);
        if (data & TOPBIT) {
            space(PANASONIC_ONE_SPACE);
        } else {
            space(PANASONIC_ZERO_SPACE);
        }
        data <<= 1;
    }
    mark(PANASONIC_BIT_MARK);
    space(0);
}
void IRsend::sendJVC(unsigned long data, int nbits, int repeat) {
    enableIROut(38);
    data = data << (32 - nbits);
    if (!repeat){
        mark(JVC_HDR_MARK);
        space(JVC_HDR_SPACE);
    }
    for (int i = 0; i < nbits; i++) {
        if (data & TOPBIT) {
            mark(JVC_BIT_MARK);
            space(JVC_ONE_SPACE);
        } else {
            mark(JVC_BIT_MARK);
            space(JVC_ZERO_SPACE);
        }
        data <<= 1;
    }
    mark(JVC_BIT_MARK);
    space(0);
}
void IRsend::mark(int time) {
    if (time > 0) {
        int cycles = time/28;
        for (int j = 0; j < cycles; j++) {
            digitalWriteFast(TIMER_PWM_PIN, HIGH);
            delayMicroseconds(12);
            digitalWriteFast(TIMER_PWM_PIN, LOW);
            delayMicroseconds(12);
        }
    }
}

/* Leave pin off for time (given in microseconds) */
void IRsend::space(int time) {
    if (time > 0) {
        delayMicroseconds(time);
    }
}

void IRsend::enableIROut(int khz) {
    pinMode(TIMER_PWM_PIN, OUTPUT);
    digitalWrite(TIMER_PWM_PIN, LOW);
}


void IRsend::sendSharp(unsigned long data, int nbits) {
    unsigned long invertdata = data ^ SHARP_TOGGLE_MASK;
    enableIROut(38);
    for (int i = 0; i < nbits; i++) {
        if (data & 0x4000) {
            mark(SHARP_BIT_MARK);
            space(SHARP_ONE_SPACE);
        } else {
            mark(SHARP_BIT_MARK);
            space(SHARP_ZERO_SPACE);
        }
        data <<= 1;
    }
    
    mark(SHARP_BIT_MARK);
    space(SHARP_ZERO_SPACE);
    delay(46);
    for (int i = 0; i < nbits; i++) {
        if (invertdata & 0x4000) {
            mark(SHARP_BIT_MARK);
            space(SHARP_ONE_SPACE);
        } else {
            mark(SHARP_BIT_MARK);
            space(SHARP_ZERO_SPACE);
        }
        invertdata <<= 1;
    }
    mark(SHARP_BIT_MARK);
    space(SHARP_ZERO_SPACE);
    delay(46);
}

void IRsend::sendDISH(unsigned long data, int nbits) {
    enableIROut(56);
    mark(DISH_HDR_MARK);
    space(DISH_HDR_SPACE);
    for (int i = 0; i < nbits; i++) {
        if (data & DISH_TOP_BIT) {
            mark(DISH_BIT_MARK);
            space(DISH_ONE_SPACE);
        } else {
            mark(DISH_BIT_MARK);
            space(DISH_ZERO_SPACE);
        }
        data <<= 1;
    }
}
