#include "IR-transmitter.h"

// IR SEND (PIN A0)
IRsend irsend;

char gDeviceInfo[120];  //adjust size as required

// SETUP
void setup() {
    
    Particle.variable("deviceInfo", gDeviceInfo);
    
    snprintf(gDeviceInfo, sizeof(gDeviceInfo)
        ,"App: %s, Date: %s, Time: %s, Sysver: %s"
        ,__FILE__
        ,__DATE__
        ,__TIME__
        ,(const char*)System.version()  // cast required for String
    );
    
    
    // MAKE THIS A PARTICLE FUNCTION
    Particle.function("sendIR", sendIR);
    
    Serial.begin(9600);
}

// LOOP
void loop() {

}




// SEND IR FUNCTION
int sendIR(String args){
    int code = (int) strtol(args, NULL, 0);
    irsend.sendRC6(code, 20);
    return 1;
}
