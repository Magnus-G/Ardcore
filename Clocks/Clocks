
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

int cycleCounter = 0;
int dividerCounter = 1;

int halvesCounter = 1;
int halves[8] = {1, 2, 4, 8, 16, 32, 64, 128};

int indicator[4] = {0,0,0,0}; // for the "change program signal"

// program controlled with A0 e.g. program 1 = from 0 to 300.
int program1 = 250;
int program2 = 500;
int program3 = 750;
int program4 = 9999;

// randomization/probability
int randValue = 1023;

// for the on/off switch on clock in
int inputIsHigh = 0;
int digState[2] = {LOW, LOW};  // start with both set low

// counting drumloops. see drums.h for all drum patterns
int drumLoops = 0;
int drumDivision[6] = {1, 2, 4, 8, 16, 32};    

// Functions
void programChangeSignal(int x) {    
  if (indicator[x-1] == 0) { // if the the new program is not already active, let's signal that this is a new program...
    for (int i=0; i<8; i++) { 
      pinMode(pinOffset+i, OUTPUT); digitalWrite(pinOffset+i, HIGH); // turn on all triggers for a while to signal program change
    }
    delay(1000); // duration of signal
    for (int i=0; i<8; i++) { 
      pinMode(pinOffset+i, OUTPUT); digitalWrite(pinOffset+i, LOW); // turn off
    }
  }
  
  for (int y=0; y<4; y++) {
    indicator[y] = 0; // reset all indicators 
  }
  indicator[x-1] = 1; // set the current program indicator to 1 i.e. ON
}


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

  if ((clkState == HIGH && (analogRead(1) < 10)) || (11 < analogRead(1))) { // if A1 is fully CCW and there's a input beat on clock in OR if A1 is above CCW

    if (11 < analogRead(1) && indicator[3] != 1) { // if A1 is above CCW AND you are not running the drum program (that clocks itself internally)
      delay(1023 - analogRead(1)); // run without external clock. i.e. add a delay from A1 and keep firing triggers.
    }
    
    // send a trig to "clock in" to get D0 and D1 to switch back and forth with open gates
    if (clkState == HIGH) {
      if (inputIsHigh == 0) {
        digState[0] = HIGH;
        digitalWrite(digPin[0], HIGH);
        digitalWrite(digPin[1], LOW);
        ++inputIsHigh;
      }
      
      else {
        digState[0] = LOW;
        digitalWrite(digPin[0], LOW);
        digitalWrite(digPin[1], HIGH);
        --inputIsHigh;
      }
    }
    
    clkState = LOW;  // reset for the next clock
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //   Cycle (program 1)
    //
        
    if (analogRead(0) < program1) {
     
     programChangeSignal(1);
        
      for(int i=0; i<8; i++) {
        if (cycleCounter==i) {
          Serial.print("Step through: ");  Serial.println(i);  
          randValue = random(0, 1023);

          if (randValue > analogRead(2)) {
            digitalWrite(pinOffset+i, HIGH);
            delay(1); // to let the LED's show
            digitalWrite(pinOffset+i, LOW);
          }
        }
      }
      
      if (cycleCounter<7) { 
        cycleCounter++;
      }
      
      else {
        cycleCounter=0;
      }
      
    } // Program 1
       
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //   Halves (Program 2)  
    //
    
     if (analogRead(0) > (program1+1) && analogRead(0) < program2) {  

      programChangeSignal(2);
        
      for (int x=0; x<8; x++) {
        if ((halvesCounter) % halves[x] == 0) { 
        randValue = random(0, 1023);
        
          if (randValue > analogRead(2)) {                   
            digitalWrite(pinOffset+x, HIGH);
            delay(1);
            digitalWrite(pinOffset+x, LOW);
          }
        } 
      } 
              
      if (halvesCounter<128) { 
        halvesCounter++;
      }
      
      else {
        halvesCounter=1;
      }
    } // Program 2
     
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //   Divider (Program 3)   
    
    
    if (analogRead(0) > (program2+1) && analogRead(0) < program3) {  // knob setting to tune in program

      programChangeSignal(3);
    
      for (int i=0; i<8; i++) {
        if (dividerCounter % (i+1) == 0) { 
          randValue = random(0, 1023);
          if (randValue > analogRead(2)) {     
            digitalWrite(pinOffset+(i), HIGH);
            delay(1); // to let the LED's show
            digitalWrite(pinOffset+(i), LOW);
          }
        }
      }  
    
      dividerCounter++;
      
    } // program 3
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //   Drums (Program 4)   
    
    
    if (analogRead(0) > (program3+1) && analogRead(0) < program4) {  // knob setting to tune in program

      programChangeSignal(4);
      
      int drumProgram = (analogRead(3) / (1023/noOfDrumPrograms)); 
      if (drumProgram > 0) { // deal with zero indexing on addressing the array vs the integer declared to set the number.
        drumProgram--;
      }
            Serial.println(drumProgram);
            
      // go through drum matrix
      for (int x=1; x<17; x++) { // temporal. start with first beat point...
        for (int y=1; y<noOfDrumSteps; y++) { // vertical, outputs. start with output 0...
          randValue = random(0, 1023);
          if (randValue > analogRead(2)) {     
            digitalWrite(pinOffset+(y-1), drums[drumProgram][y-1][x-1]);
            digitalWrite(pinOffset+(6), HIGH); // send one trigger per beat to output 6.
          }          
        }
  
        // decide if beat is going to be double speed, double-double etc...  
        int drumDivisionNo = ((analogRead(4) / 100)/2); //e.g. 1023/100 -> 10. 10/2 -> 5. Fifth place in the drumDivision array == 32 
        delay((1023 - analogRead(1))/drumDivision[drumDivisionNo]); // delay after each beat == bpm
 
        for (int y=1; y<9; y++) { 
          digitalWrite(pinOffset+(y-1), LOW); // turn off triggers after each "drumLoops"
        }
      }
      // every nth time the drumLoop loops, send out a trigger on no 7.
      drumLoops++;
      if (drumLoops % 4 == 0) {
        digitalWrite(pinOffset+(7), HIGH);
        delay(2);
        digitalWrite(pinOffset+(7), LOW);
      }
    } // program 4
  } // end if clock state high
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
