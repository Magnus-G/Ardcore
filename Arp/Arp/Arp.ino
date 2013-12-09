#include <math.h>
//#include "drumPatterns.h";

//  constants related to the Arduino Nano pin use
const int clkIn = 2;           // the digital (clock) input
const int digPin[2] = {3, 4};  // the digital output pins
const int pinOffset = 5;       // the first DAC pin (from 5-12)
const int trigTime = 25;       // 25 ms trigger timing

//  variables for interrupt handling of the clock input
volatile int clkState = LOW;

//  variables used to control the current DIO output states
int digState[2] = {LOW, LOW};        // start with both set low
unsigned long digMilli[2] = {0, 0};  // a place to store millis()

int currentPosition = -1;
int previousPosition = -1;
int previousNon0Position = 0;
int currentDirection = 1;

int scale;

const int noOfPatterns = 1; 
const int patternLength = 8;
int pattern[noOfPatterns][patternLength] = {{0,1,2,3,4,5,6,7}};
int patternType;
int patternTypeUtility = 1023 / noOfPatterns;
int patternPlace = 0;

const int noOfScales = 25;
const int scaleLength = 8;
int scaleUtility = 1023 / noOfScales;
int scales[noOfScales][scaleLength] = {
// Major. 12 scales
{1,3,4,6,8,9,11,0},    // c major
{6,8,10,11,1,3,5,0},   // d major
{5,7,9,10,12,2,4,0},   // C#/Db
{7,9,11,12,2,4,6,0},   // D#/Eb
{8,10,12,1,3,5,7,0},   // E Major
{9,11,1,2,4,6,8,0},    // F Major
{10,12,2,3,5,7,9,0},   // F#/Gb
{11,1,3,4,6,8,10,0},   // G Major
{12,2,4,5,7,9,11,0},   // G#/Ab
{1,3,5,6,8,10,12,0},   // A Major
{2,4,6,7,9,11,1,0},    // A#/Bb
{3,5,7,8,10,12,2,0},   // B Major

// Minor. 12 scales. 
{1,3,4,6,9,11,1,0},    // A Minor
{2,4,5,7,9,10,12,0},   // A#/Bb
{3,5,6,8,10,11,1,0},   // B Minor
{5,6,8,10,12,1,3,0},   // C Minor
{5,7,8,10,12,1,3,0},   // C#/Db
{6,8,9,11,1,2,4,0},    // D Minor
{7,9,10,12,2,3,5,0},   // D#/Eb
{8,10,11,1,3,4,6,0},   // E Minor
{9,11,12,2,4,5,7,0},   // F Minor
{10,12,1,3,5,6,8,0},   // F#/Gb
{11,1,2,4,6,7,9,0},    // G Minor
{12,2,3,5,7,8,1,0},     // G#/Ab

// Pentatonic. 12 scales.
{4,6,8,11,1,4,6}
};

////////////////////////////////////////////////////////

void setup() 
{
  Serial.begin(9600);

  pinMode(clkIn, INPUT);
  
  for (int i=0; i<2; i++) {
    pinMode(digPin[i], OUTPUT);
    digitalWrite(digPin[i], LOW);
  }
  
  for (int i=0; i<8; i++) {
    pinMode(pinOffset+i, OUTPUT);
    digitalWrite(pinOffset+i, LOW);
  }
  attachInterrupt(0, isr, RISING);
}









void loop() {

  scale = (analogRead(2) / scaleUtility) -1;
  if (scale == -1) {scale = 0;}  
  patternType = (analogRead(3) / patternTypeUtility) -1;
  if (patternType == -1) {patternType = 0;}    
 
  digitalWrite(digPin[0], clkState);
  digitalWrite(digPin[1], clkState);  

  if (clkState == HIGH) {
    clkState = LOW;
 
    currentPosition = pattern[0][patternPlace];
    patternPlace++;
   
    if (patternPlace >= (patternLength)) {
      patternPlace = 0; 
    }    

    previousPosition = currentPosition;
    if (scales[scale][currentPosition] == 0) { // if we are in the last position
      
      int note = scales[scale][0] * 3 + 3;
      dacOutput(note);      
    }
    else {
      int note = scales[scale][currentPosition] * 3;
      dacOutput(note);
    }


    
  }
}



































void isr()
{
  clkState = HIGH;
}

void writeStep(boolean on){
    if(on){
        digitalWrite(digPin[0], HIGH);
        digitalWrite(digPin[1], LOW);
    }else{
        digitalWrite(digPin[0], LOW);
        digitalWrite(digPin[1], HIGH);
    }  
}

//  dacOutput(long) - deal with the DAC output
//  ------------------------------------------
void dacOutput(long v)
{
  // feed this routine a value between 0 and 255 and teh DAC
  // output will send it out.
  int tmpVal = v;
  for (int i=0; i<8; i++) {
    digitalWrite(pinOffset + i, tmpVal & 1);
    tmpVal = tmpVal >> 1;
  }
}

