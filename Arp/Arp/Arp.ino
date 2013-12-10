//#include <math.h>
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


const int noOfPatterns = 3; 
const int patternLength = 9;
int patternType;
int patternTypeUtility = 1023 / noOfPatterns;
int patternPlace = 0;
int pattern[noOfPatterns][patternLength] = {
  {0,1,2,3,4,5,6,7,8},
  {8,7,6,5,4,3,2,1,0},
  {8,7,6,5,4,3,2,1,0}
};




const int noOfScales = 25;
const int scaleLength = 9;
int scaleUtility = 1023 / noOfScales;
int scales[noOfScales][scaleLength] = {
  
  // scales. 99 means "end here"  
    
  // Major. 12 scales
  {1,3,4,6,8,9,11,13,99},    // c major
  {1,3,5,6,8,10,11,13,99},   // d major
  {2,4,5,7,9,10,12,14,99},   // C#/Db
  {2,4,6,7,9,11,12,14,99},   // D#/Eb
  {1,3,5,7,8,10,12,13,99},   // E Major
  {1,2,4,6,8,9,11,13,99},    // F Major
  {2,3,5,7,9,10,12,14,99},   // F#/Gb
  {1,3,4,6,8,10,11,13,99},   // G Major
  {2,4,5,7,9,11,12,14,99},   // G#/Ab
  {1,3,5,6,8,10,12,13,99},   // A Major
  {1,2,4,6,7,9,11,13,99},    // A#/Bb
  {2,3,5,7,8,10,12,14,99},   // B Major
  
  // Minor. 12 scales. 
  {1,3,4,6,8,9,11,13,99},    // A Minor
  {2,4,5,7,9,10,12,14,99},   // A#/Bb
  {1,3,5,6,8,10,11,13,99},   // B Minor
  {1,3,5,6,8,10,12,13,99},   // C Minor
  {1,3,5,7,8,10,12,13,99},   // C#/Db
  {1,2,4,6,8,9,11,13,99},    // D Minor
  {2,3,5,7,9,10,12,14,99},   // D#/Eb
  {1,3,4,6,8,10,11,13,99},   // E Minor
  {2,4,5,7,9,11,12,14,99},   // F Minor
  {1,3,5,6,8,10,12,13,99},   // F#/Gb
  {1,2,4,6,7,9,11,13,99},    // G Minor
  {1,2,3,5,7,8,12,13,99},     // G#/Ab
  
  // Pentatonic. 12 scales.
  {1,4,6,8,11,13,99,99,99}
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

 Serial.print("analogRead(3): "); Serial.println(analogRead(3));  
 Serial.print("patternType: "); Serial.println(patternType); 
 
  digitalWrite(digPin[0], clkState);
  digitalWrite(digPin[1], clkState);  

  if (clkState == HIGH) {
    clkState = LOW;
    currentPosition = pattern[patternType][patternPlace];          
    patternPlace++;                         
    
    if (scales[scale][currentPosition] == 99) { // if we are in the last position
      patternPlace = 0;
      currentPosition = 0;
      int note = (((scales[scale][currentPosition]) * 4) + (12 * 4));
      dacOutput(note);
      patternPlace++;    
    }
    
    else {
      int note = (((scales[scale][currentPosition]) * 4) + (12 * 4));
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

