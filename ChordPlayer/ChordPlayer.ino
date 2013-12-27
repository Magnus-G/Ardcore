
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

int currentchordValue = 0;
int previousNote = 98;


const int noOfPatterns = 2; // plus one here if a pattern is added to "pattern"

const int patternLength = 16;
int patternValue;
int patternType;
int patternTypeUtility = 1023 / noOfPatterns;
int patternPlace = 0;
int pattern[noOfPatterns][patternLength] = {
  
  
  // 1,  2,  3,  4,  5,  6,  7,  8,   1,  2,  3,  4,  5,  6,  7,  8
    {0,  1,  2,  3,  4,  5,  6,  7,   8,  9,  10, 11, 12, 13, 14, 14},
    {0,  5,  0,  1,  3,  5,  0,  5,   10, 9,  5,  0,  1,  3,  10, 9},    
};


int c     = 1;
int ciss  = 2;
int d     = 3;
int diss  = 4;
int e     = 5;
int f     = 6;
int fiss  = 7;
int g     = 8;
int giss  = 9;
int a     = 10;
int aiss  = 11;
int b     = 12;

int c2     = 13;
int ciss2  = 14;
int d2     = 15;
int diss2  = 16;
int e2     = 17;
int f2     = 18;
int fiss2  = 19;
int g2     = 20;
int giss2  = 21;
int a2     = 22;
int aiss2  = 23;
int b2     = 24;

int c3     = 25;
int ciss3  = 26;
int d3     = 27;
int diss3  = 28;
int e3     = 29;
int f3     = 30;
int fiss3  = 31;
int g3     = 32;
int giss3  = 33;
int a3     = 34;
int aiss3  = 35;
int b3     = 36;

int c4     = 37;
int ciss4  = 38;
int d4     = 39;
int diss4  = 40;
int e4     = 41;
int f4     = 42;
int fiss4  = 43;
int g4     = 44;
int giss4  = 45;
int a4     = 46;
int aiss4  = 47;
int b4     = 48;

int chord;

const int noOfchords = 8;
const int chordLength = 15;
int chordUtility = 1023 / noOfchords;
int chords[noOfchords][chordLength] = {
  
  // chords. 99 means "skip this"  

  
  // 0,    1,    2,    3,     4,     5,     6,     7,     8,     9,     10,    11,    12,    13,     14    
    {c,    e,    g,    99,    99,    c2,    e2,    g2,    99,    99,    c3,    e3,    g3,    99,     99},     // C  
    {a,    c2,   e2,   99,    99,    a2,    c3,    e3,    99,    99,    a3,    c4,    e4,    99,     99},      // Am
    {g,    b,    d2,   99,    99,    g2,    b2,    d3,    99,    99,    g3,    b3,    d4,    99,     99},      // Am
    {e,    g,    b,    99,    99,    e2,    g2,    b2,    99,    99,    e3,    g3,    b3,    99,     99},      // Am
    {d,    fiss, a,    99,    99,    d2,    fiss2, a2,    99,    99,    d3,    fiss3, a3,    99,     99},      // Am
    {b,    d2,   fiss2,99,    99,    b2,    d3,    fiss3, 99,    99,    b3,    d4,    fiss4, 99,     99},      // Am
    {a,    ciss2,e2,   99,    99,    a2,    ciss3, e3,    99,    99,    a3,    ciss4, e4,    99,     99},      // Am
    {fiss, a,    ciss2,99,    99,    fiss2, a2,    ciss3, 99,    99,    fiss3,    a3,    ciss4,    99,     99}      // Am    
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

//  chord = (analogRead(2) / chordUtility) -1; if (chord == -1) {chord = 0;}  
  chord = (analogRead(2) >> 7); // Divide 1023 by 2. then the result by 2... iterate seven times. Chord is between 0 and 7
  
       Serial.print("chord: "); Serial.println(chord); 

  patternType = (analogRead(3) / patternTypeUtility) -1; if (patternType == -1) {patternType = 0;}   


  

  if (clkState == HIGH) {
    clkState = LOW;
    
    digitalWrite(digPin[0], HIGH);
    digitalWrite(digPin[0], LOW);
   
    
    patternValue = pattern[patternType][patternPlace];    
    
    while (chords[chord][patternValue] == 99) {
      patternPlace++;
      
      if (patternPlace >= patternLength) {
        patternPlace = 0;
      }
      
      patternValue = pattern[patternType][patternPlace];
    }
    
    int note = (((chords[chord][patternValue]) * 4) + (12 * 4));
    dacOutput(note);
    
    if (previousNote != note) {
      digitalWrite(digPin[1], HIGH);   
      digitalWrite(digPin[1], LOW);
    }
    
    previousNote = note;

    patternPlace++;
    
    if (patternPlace >= patternLength) {
      patternPlace = 0;
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

