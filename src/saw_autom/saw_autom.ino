/*
Saw automation

Created on 28.02.2021

Initially written by Santeri Porrasmaa
*/

byte toggleAutomPin=2; 
byte sawRelay=4; 
byte splitRelay=5; 
byte sawSensor=6; 
byte splitSensor=7; 
bool runLoop=false;

void setup() {
    pinMode(sawRelay, OUTPUT);
    pinMode(splitRelay, OUTPUT);
    pinMode(sawSensor, INPUT);
    pinMode(splitSensor, INPUT);
    attachInterrupt(digitalPinToInterrupt(toggleAutomPin), toggleAutom, RISING);
    // Relays are active low
    digitalWrite(sawRelay, 1);
    digitalWrite(splitRelay, 1);
}

void toggleAutom() {
    /*
    ISR TO TOGGLE ON OR OFF THE AUTOMATION LOOP
    */
    if (runLoop) {
        runLoop=false;
    }
    else {
        runLoop=true;
    }
}

void loop() {
    if (runLoop) {
        delay(1000); // Wait 1s
        digitalWrite(sawRelay, 0); // Turn on saw
        while (!(digitalRead(sawSensor))) {} // Loop until log has been saw blade is at extreme position 
        delay(200); // Wait 0.2s to ensure log has been sawed
        digitalWrite(sawRelay, 1); // Turn off saw
        delay(1000); // Ensure wood is in through
        digitalWrite(splitRelay, 0); // Turn on splitting
        delay(1000); 
        digitalWrite(splitRelay, 1); // Turn off splitting
        while(!(digitalRead(splitSensor))) {} // Loop until log is split
    }
} 
