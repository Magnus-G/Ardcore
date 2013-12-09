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
//int MAX_CHORD_LENGTH = 5;

const int pattern1Length = 7;
int pattern1Place = 0;
int pattern1[pattern1Length] = {0,1,2,3,4,5,6};

const int noOfPatterns = 2;
int pattern[noOfPatterns][8] = {
  {0,1,2,3,4,5,6,7},
  {0,1,0,2,0,3,0,4}
};

const int noOfScales = 25;
int scaleUtility = 1023 / noOfScales;
int scales[noOfScales][7] = {
// Major. 12 scales
{4,6,8,9,11,1,3},    // c major
{6,8,10,11,1,3,5},   // d major
{5,7,9,10,12,2,4},   // C#/Db
{7,9,11,12,2,4,6},   // D#/Eb
{8,10,12,1,3,5,7},   // E Major
{9,11,1,2,4,6,8},    // F Major
{10,12,2,3,5,7,9},   // F#/Gb
{11,1,3,4,6,8,10},   // G Major
{12,2,4,5,7,9,11},   // G#/Ab
{1,3,5,6,8,10,12},   // A Major
{2,4,6,7,9,11,1},    // A#/Bb
{3,5,7,8,10,12,2},   // B Major

// Minor. 12 scales. 
{1,3,4,6,9,11,1},    // A Minor
{2,4,5,7,9,10,12},   // A#/Bb
{3,5,6,8,10,11,1},   // B Minor
{5,6,8,10,12,1,3},   // C Minor
{5,7,8,10,12,1,3},   // C#/Db
{6,8,9,11,1,2,4},    // D Minor
{7,9,10,12,2,3,5},   // D#/Eb
{8,10,11,1,3,4,6},   // E Minor
{9,11,12,2,4,5,7},   // F Minor
{10,12,1,3,5,6,8},   // F#/Gb
{11,1,2,4,6,7,9},    // G Minor
{12,2,3,5,7,8,1},     // G#/Ab

// Pentatonic. 12 scales.
{4,6,8,11,1,4,6}
};



//{,,,,,,},
//{,,,,,,},
//{,,,,,,},
//{,,,,,,},
//{,,,,,,},
//{,,,,,,},
//{,,,,,,},
//{,,,,,,},
//{,,,,,,}

















//{6,8,9,11,1,2,4}, // d minor

//int chords[12][5] = {{1,4,8,0,0}, {1,5,8,0,0}, {1,8,8,0,0}, {1,4,7,0,0}, {1,5,8,10,0}, {1,5,8,12,0}, {1,4,8,11,0}, {1,5,10,0,0},{1,5,8,11,0}, {1,5,8,15,0},{1,5,10,0,0}, {1,4,8,10,0}};
//int pattern[4] = {0,1,2,3};

//String chordName[12] = {"Major", };
//int currentChord[5];

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
 
  digitalWrite(digPin[0], clkState);
  digitalWrite(digPin[1], clkState);  

  if (clkState == HIGH) {
    clkState = LOW;
 
   if (pattern == 0) { // According to pattern
       currentPosition = pattern1[pattern1Place];
       pattern1Place++;
       
       if (pattern1Place >= (pattern1Length)) {
        pattern1Place = 0; 
       }    
    }

    previousPosition = currentPosition;

    int note = scales[scale][currentPosition] * 3;
    Serial.println(note);
    dacOutput(note);
    
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

