/*
Saw automation

Created on 28.02.2021

Initially written by Santeri Porrasmaa
*/

// Pins:
byte toggleAutomPin=2; 
byte blinker=3;
byte sawRelay=4; 
byte splitRelay=5; 
byte sawSensor=6; 
byte splitSensor=7; 
//  boolean flag for toggling automation based on user input
bool runLoop=false;

// Time (in milliseconds) to read button as pressed
unsigned long debounceTime = 1000;
// Time (in milliseconds) to deactivate saw if sensor doesn't give signal
unsigned long sawMaxTime = 5000;
// Variables to measure the time passed (in milliseconds)
// NOTE: millis() call will overflow variable after approx. 50 days
unsigned long lastBounce = 0;
unsigned long looptime = 0;
unsigned long sawBegin = 0;
unsigned long timePassedSaw = 0;


void setup() {
    pinMode(sawRelay, OUTPUT);
    pinMode(splitRelay, OUTPUT);
    pinMode(blinker, OUTPUT);
    pinMode(sawSensor, INPUT);
    pinMode(splitSensor, INPUT);
    pinMode(toggleAutomPin, INPUT);
    // Relays are active low, turn off initially
    digitalWrite(sawRelay, 1);
    digitalWrite(splitRelay, 1);
    digitalWrite(blinker, 1);
}

void loop() {
    if (!(digitalRead(toggleAutomPin))) {
        lastBounce = millis(); // Initial press registered
        while (!(digitalRead(toggleAutomPin))) { // Loop until button released 
            looptime = millis(); 
        }
        if (looptime - lastBounce >= debounceTime) { // Perform action if button was pressed for 1s or more
            if (runLoop) { // Turn off automation & blinker
                runLoop=false;
                digitalWrite(blinker, 1);
            }
            else { // Turn on automation and blinker
                runLoop=true;
                digitalWrite(blinker, 0);
            }
        }
    }
    else {
        if (runLoop) {
        delay(1000); // Wait 1s
        digitalWrite(sawRelay, 0); // Turn on saw
        sawBegin = millis(); // Get current time in milliseconds
        // Wait until sensor gives signal OR until approx 5s has passed
        while (!(digitalRead(sawSensor))) { 
            timePassedSaw = millis();
            if (timePassedSaw - sawBegin >= sawMaxTime) {
                digitalWrite(sawRelay, 1); // Turn off saw
                break;
            }
        }
        delay(200); // Wait 0.2s to ensure log has been sawn
        digitalWrite(sawRelay, 1); // Turn off saw (if it wasn't already)
        delay(1000); // Ensure wood is in through
        digitalWrite(splitRelay, 0); // Turn on splitting
        delay(1000); 
        digitalWrite(splitRelay, 1); // Turn off splitting
        while(!(digitalRead(splitSensor))) {} // Wait until log is split
        }
    }
} 
