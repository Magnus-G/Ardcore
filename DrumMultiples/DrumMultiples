
// the drumPatterns.h file must be in the same folder as this file when compiling.
#include "drumPatterns.h";

const int clkIn = 2;           // the digital (clock) input
const int digPin[10] = {3,4,5,6,7,8,9,10,11,12};  // the digital output pins
const int pinOffset = 5;       // the first DAC pin (from 5-12)
const int trigTime = 10;       // triggers are 10 ms.
volatile int clkState = LOW;

const int noOfMultipliedDrums = 3;
unsigned long interval = 0;
unsigned long previousTime[noOfMultipliedDrums] = {0,0,0};
unsigned long currentTime[noOfMultipliedDrums] = {0,0,0};
unsigned long spaceBetweenFires[noOfMultipliedDrums] = {0,0,0};
unsigned long potReading[noOfMultipliedDrums] = {0,0,0};

int noOfMultiplications[noOfMultipliedDrums] = {0,0,0};

int reset = 0;


int noOfPotSteps = 2;
int potStepSize = 1023 / noOfPotSteps; // e.g. 500 for 2 steps, 333 for 3 steps, etc


// for the on/off switch on clock in
int inputIsHigh = 0;

// counting drumloops. see drums.h for all drum patterns
int drumLoops = 0;

int currentPatternColumn = 0;


//  ==================== start of setup() ======================

void setup() {
  Serial.begin(9600);
  
  // set up the digital (clock) input
  pinMode(clkIn, INPUT);
  
  // set up the digital outputs
  for (int i=0; i<10; i++) {
    pinMode(digPin[i], OUTPUT);
    digitalWrite(digPin[i], LOW);
  }
  
  attachInterrupt(0, isr, RISING);
}

//  ==================== start of loop() =======================
void loop() {

  if (clkState == HIGH) {
    clkState = LOW;
    reset = 1;
    
    //////////////////////////////
    
    for (int x=0; x<noOfMultipliedDrums; x++) {
      currentTime[x] = millis();
      interval = currentTime[x] - previousTime[x];
      potReading[x] = analogRead(x);
      noOfMultiplications[x] = (potReading[x] / potStepSize); // should render e.g. 2 multiplications
      spaceBetweenFires[x] = interval / noOfMultiplications[x]; // e.g. 500ms if interval is 1000ms and noOf is 2
      previousTime[x] = currentTime[x];
    }
    
    

    //////////////////////////////
 
    int drumProgram = (analogRead(3) / (1023/noOfDrumPrograms)); 
    if (drumProgram > 0) { // deal with zero indexing on addressing the array vs the integer declared to set the number.
      drumProgram--;
    }
            
    if (currentPatternColumn >= 16) {
      currentPatternColumn = 0;
    }

    for (int y=1; y<noOfDrumSteps; y++) { // vertical, outputs. start with output 0...
      digitalWrite(pinOffset+(y-1), drums[drumProgram][y-1][currentPatternColumn]);
      delay(1);
      digitalWrite(pinOffset+(y-1), LOW);
    }
    
    currentPatternColumn++; // x dimension

  } // end if clock state high



  for (int i=0; i<noOfMultipliedDrums; i++) {
    if (millis() - currentTime[i] > spaceBetweenFires[i]) {
      currentTime[i] = millis();
      digitalWrite(pinOffset+i, HIGH);
      digitalWrite(pinOffset+i, LOW);
    }
  }

  
  
    
  

//    Serial.println();
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
