
// Clock and drum pattern sketches for the Ardcore Expander
// Feed a clock in "Clock in" for external beat. Set A1 to more than 0 for internal beat.
// The clock in toggles between D0 and D1 to open and close.
// Programs are set with A0. 1, 2, 3... etc from CCW to CW
// 1. Cycle Program 
// 2. Halves Program
// 3. Divider Program
// 4. Drum Program      Use A3 to set different drum patterns.
// A2 controls probablility of output for triggers. Set A2 full CCW for no randomization and turn it CW for increased "drop out" of trig outs. 
// There is a 1 second signal as the program is changed.
// Written by: Magnus Gyllensward

// the drumPatterns.h file must be in the same folder as this file when compiling.
#include "drumPatterns.h";

const int clkIn = 2;           // the digital (clock) input
const int digPin[2] = {3,4};  // the digital output pins
const int pinOffset = 5;       // the first DAC pin (from 5-12)
const int trigTime = 10;       // triggers are 10 ms.

//  variables for interrupt handling of the clock input
volatile int clkState = LOW;

// randomization/probability
int randValue = 1023;

// for the on/off switch on clock in
int inputIsHigh = 0;
int digState[2] = {LOW, LOW};  // start with both set low

// counting drumloops. see drums.h for all drum patterns
int drumLoops = 0;
int drumDivision[6] = {1, 2, 4, 8, 16, 32};    


//  ==================== start of setup() ======================

void setup() {
  Serial.begin(9600);
  
  // set up the digital (clock) input
  pinMode(clkIn, INPUT);
  
  // set up the digital outputs
  for (int i=0; i<2; i++) {
    pinMode(digPin[i], OUTPUT);
    digitalWrite(digPin[i], LOW);
  }
  
  // set up the 8-bit DAC output pins
  for (int i=0; i<8; i++) {
    pinMode(pinOffset+i, OUTPUT);
    digitalWrite(pinOffset+i, LOW);
  }
  
  attachInterrupt(0, isr, RISING);
}

//  ==================== start of loop() =======================
void loop() {
            
      // go through drum matrix
      for (int x=1; x<17; x++) { // temporal. start with first beat point...
        
        // set melodyPatternType
        int melodyPatternType = (analogRead(0) / (1023/noOfMelodyPatterns)); if (melodyPatternType > 0) {  melodyPatternType--; } // deal with zero indexing on addressing the array vs the integer declared to set the number.
        
        // write 0 or 1 on output to trigger melody pattern, set independently of the current drumpattern.
        digitalWrite(pinOffset+(5), melodyPattern[melodyPatternType][x-1]);
        delay(1);
        digitalWrite(pinOffset+(5), LOW);  
      
        for (int y=1; y<noOfDrumSteps; y++) { // vertical, outputs. start with output 0...
          randValue = random(0, 1023);
          if (randValue > analogRead(2)) {     
            
            // set drumProgram
            int drumProgram = (analogRead(3) / (1023/noOfDrumPrograms)); if (drumProgram > 0) {  drumProgram--; } // deal with zero indexing on addressing the array vs the integer declared to set the number.
            
            // the hits
            digitalWrite(pinOffset+(y-1), drums[drumProgram][y-1][x-1]);
            delay(1);
            digitalWrite(pinOffset+(y-1), LOW);  
          }          
        }
        delay(1023 - analogRead(1)); // delay after each beat == bpm
      }
      
  // extra hits, outside of the drum program
  drumLoops++;
  
  if (drumLoops % 2 == 0) {
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);
  }
  
  if (drumLoops % 3 == 0) {
    digitalWrite(4, HIGH);
    digitalWrite(3, LOW);
  }

  if (drumLoops % 6 == 0) {
    digitalWrite(pinOffset+(7), HIGH);
    delay(1);
    digitalWrite(pinOffset+(7), LOW);
  }      
} // end loop

//  =================== convenience routines ===================
//  isr() - quickly handle interrupts from the clock input
//  ------------------------------------------------------
void isr()
{
  // Note: you don't want to spend a lot of time here, because
  // it interrupts the activity of the rest of your program.
  // In most cases, you just want to set a variable and get
  // out.
  clkState = HIGH;
}

//  dacOutput(byte) - deal with the DAC output
//  -----------------------------------------
void dacOutput(byte v)
{
  PORTB = (PORTB & B11100000) | (v >> 3);
	PORTD = (PORTD & B00011111) | ((v & B00000111) << 5);
}

//  ===================== end of program =======================
