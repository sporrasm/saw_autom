/*
Saw automation

Created on 28.02.2021

Initially written by Santeri Porrasmaa
*/
// New pin configuration
#define NEW_PINS
// uncomment below for active low relays
//#define RELAY_ACT_LOW
// uncomment below for serial monitor debug prints
//#define DEBUG

#ifdef RELAY_ACT_LOW
byte onState=0;
byte offState=1;
#else
byte onState=1;
byte offState=0;
#endif

#ifndef NEW_PINS
// Pins:
byte toggleAutomPin=2; 
byte blinker=3;
byte sawRelay=4; 
byte splitRelay=5; 
byte sawSensor=6; 
byte splitSensor=7; 
#else
byte toggleAutomPin=5;
byte blinker=13;
byte sawRelay=A1;
byte splitRelay=A0;
byte sawSensor=A2;
byte splitSensor=A3;
#endif

//  boolean flag for toggling automation based on user input
bool runLoop=false;


// Time (in milliseconds) to read button as pressed
unsigned long debounceTime = 3000;
// Time (in milliseconds) to deactivate saw if sensor doesn't give signal
unsigned long sawMaxTime = 5000;
// Variables to measure the time passed (in milliseconds)
// NOTE: millis() call will overflow variable after approx. 50 days
unsigned long lastBounce = 0;
unsigned long looptime = 0;
unsigned long sawBegin = 0;
unsigned long timePassedSaw = 0;
// Counter for checking whether or not we toggle automation on/off
unsigned int toggleAutomCounter = 0;
// Autom toggle threshold
unsigned int toggleAutomThresh = 100;
// Delay to wait between counter increments
unsigned int counterIncDelay = int(debounceTime / toggleAutomThresh);

void setup() {
    #ifdef DEBUG
    Serial.begin(9600);
    char buf[30];
    sprintf(buf, "Time to wait between incr: %d ms", counterIncDelay);
    Serial.println(buf);
    #endif
    pinMode(sawRelay, OUTPUT);
    pinMode(splitRelay, OUTPUT);
    pinMode(blinker, OUTPUT);
    pinMode(sawSensor, INPUT);
    pinMode(splitSensor, INPUT);
    pinMode(toggleAutomPin, INPUT);
    // Relays are active low, turn off initially
    digitalWrite(sawRelay, offState);
    digitalWrite(splitRelay, offState);
    digitalWrite(blinker, offState);
}


void toggleAutomState() {
    /* 
    Helper function to toggle automation state between on and off

    1. Reset counter
    2. If the toggle button is pressed, increment counter every debounceTime/toggleAutomThresh milliseconds
    3. After button is released, check if counter is over toggleAutomThresh
    4. If it is, toggle automation state

    */
    toggleAutomCounter = 0; // Reset counter
    #ifdef DEBUG
    char buf[30];
    sprintf(buf, "Counter value at begin is %d", toggleAutomCounter);
    Serial.println(buf);
    #endif
    while ((digitalRead(toggleAutomPin))) { // Loop until button released 
        delay(counterIncDelay);
        toggleAutomCounter++;
    }
    #ifdef DEBUG
    sprintf(buf, "Counter value at end is %d", toggleAutomCounter);
    Serial.println(buf);
    #endif
    if (toggleAutomCounter >= toggleAutomThresh) { // Perform action if button was pressed for debounceTime or more
        if (runLoop) { // Turn off automation & blinker
            runLoop=false;
            digitalWrite(blinker, offState);
        }
        else { // Turn on automation and blinker
            runLoop=true;
            digitalWrite(blinker, onState);
        }
    }
}

void automationLoop() {
    /*
    Helper function to run desired automation loop

    1. Wait 1s
    2. Turn on saw
    3. Wait sawMaxTime or until we read the sawSensor, which ever happens first -> turn off saw
    4. Wait 0.2 s
    5. Turn off saw, if it wasn't already
    6. Wait 0.6 s for wood to fall into through
    7. Turn on piston for log splitter
    8. Wait 0.6
    9. Turn off piston and wait until we receive reading from the splitSensor (piston returned to home position)
    10. Repeat
    */
    delay(1000); // Wait 1s
    digitalWrite(sawRelay, onState); // Turn on saw
    sawBegin = millis(); // Get current time in milliseconds
    // Wait until sensor gives signal OR until approx 5s has passed
    while (!(digitalRead(sawSensor))) { 
        timePassedSaw = millis();
        if (timePassedSaw - sawBegin >= sawMaxTime) {
            digitalWrite(sawRelay, offState); // Turn off saw
            break;
        }
    }
    delay(200); // Wait 0.2s to ensure log has been sawn
    digitalWrite(sawRelay, offState); // Turn off saw (if it wasn't already)
    delay(600); // Ensure wood is in through
    digitalWrite(splitRelay, onState); // Turn on splitting
    delay(600); 
    digitalWrite(splitRelay, offState); // Turn off splitting
    while(!(digitalRead(splitSensor))) {} // Wait until log is split
}

void loop() {
    if ((digitalRead(toggleAutomPin))) {
        toggleAutomState();
        #ifdef DEBUG
        if (runLoop) {
            Serial.println("Automation on");
        }
        else {
            Serial.println("Automation off");
        }
        #endif
    }
    else {
        if (runLoop) {
            automationLoop();
        }
    }
} 
