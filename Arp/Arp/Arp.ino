#include <math.h>

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


int root, chord, pattern, octave;
int MAX_CHORD_LENGTH = 5;

//Chords are represented as 4 note sequences. 1-12, where A=1,A#=2,B=3,C etc  
int chords[12][5] = {{1,4,8,0,0}, {1,5,8,0,0}, {1,8,8,0,0}, {1,4,7,0,0}, {1,5,8,10,0}, {1,5,8,12,0}, {1,4,8,11,0}, {1,5,10,0,0},{1,5,8,11,0}, {1,5,8,15,0},{1,5,10,0,0}, {1,4,8,10,0}};
String chordName[12] = {"Major", };
int currentChord[5];

//  ==================== start of setup() ======================

//  This setup routine should be used in any ArdCore sketch that
//  you choose to write; it sets up the pin usage, and sets up
//  initial state. Failure to properly set up the pin usage may
//  lead to damaging the Arduino hardware, or at the very least
//  cause your program to be unstable.

void setup() 
{

  // if you need to send data back to your computer, you need
  // to open the serial device. Otherwise, comment this line out.
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
  
  // set up an interrupt handler for the clock in. If you
  // aren't going to use clock input, you should probably
  // comment out this call.
  // Note: Interrupt 0 is for pin 2 (clkIn)
  attachInterrupt(0, isr, RISING);
   
}


void loop() 
{
  
  
    //TODO: Would a v/oct input be better?
    root=analogRead(0) / 86; //12 value = A,A#,B,C#, etc
    chord=analogRead(1) / 86; // 12 values see chords array above
    pattern=analogRead(2) /147;  // UP, DOWN, UP_DOWN, ALTERNATE 1-ALL, ALTERNATE 1-2, ALTERNATE 1-3, RANDOM 
    
    //TODO: This octave impl is kinda getto since it can cause values to "roll over"
    int pin3 = analogRead(3);
    octave= ( pin3 / 205 ); // scale down to 5 values, leave room to detect reset
    
    //Serial.println("loop.... ");

 
    digitalWrite(digPin[0], clkState);
    digitalWrite(digPin[1], clkState);  
    //TODO: Should I detect the user changing the values and "reset" state?
    //      A reset input may be handy as well.. Maybe ocatve is not as useful as a reset?
  
    if(pin3 > 1010){
        Serial.println("Got Reset");
        currentPosition=0;
    }
  
    // check to see if the clock as been set
    if (clkState == HIGH) {
      clkState = LOW;
 
      //Serial.print("Root Note:");  
      //Serial.println(root + 1);  
      //Serial.print("Chord:"); 
      //Serial.print(chords[chord][0]);  
      //Serial.print(","); 
      //Serial.print(chords[chord][1]); 
      //Serial.print(",");    
      //Serial.print(chords[chord][2]);  
      //Serial.print(","); 
      //Serial.print(chords[chord][3]);  
      //Serial.print(","); 
      //Serial.print(chords[chord][4]);
      //Serial.println(); 
           
      //Serial.print("Octave:"); 
      //Serial.println(octave); 
   
     if (pattern == 0) {
         //Serial.println("Up Pattern");  
          currentPosition++;
          if(chords[chord][currentPosition] == 0){
           currentPosition = 0;
          }      
      }else if(pattern == 1){
          //Serial.println("Down Pattern");  
          currentPosition--;
          if(currentPosition < 0){
            //This only happens in "DOWN" mode so move to end!
            currentPosition = MAX_CHORD_LENGTH -1;
            while(chords[chord][currentPosition] == 0){
              currentPosition = currentPosition -1;
            }
          }          
      }else if(pattern == 2){
          //Serial.println("Up/Down Pattern");  
          if(currentDirection == 1){
            currentPosition++;
            if(currentPosition >= MAX_CHORD_LENGTH || chords[chord][currentPosition] == 0){
              currentPosition = MAX_CHORD_LENGTH -1;              
              while(chords[chord][currentPosition] == 0){
                currentPosition = currentPosition -1;
              }
              currentPosition = currentPosition -1;
              currentDirection = 2;
            }            
          }else{
            currentPosition--;
            if(currentPosition < 0){
              currentPosition = 1;
              currentDirection = 1;
            }
          }  
      }else if(pattern == 3){
          //Serial.println("Alternate 1-ALL");  
          if(previousPosition != 0){
            Serial.println(previousPosition);
            currentPosition = 0;
          }else{
            currentPosition=previousNon0Position+1;
            if(currentPosition >= MAX_CHORD_LENGTH || chords[chord][currentPosition] == 0){
              currentPosition = 1;
            } 
            previousNon0Position=currentPosition;
          }
      }else if(pattern == 4){
          //Serial.println("Alternate 1-2");         
          if(previousPosition != 0){
            Serial.println(previousPosition);
            currentPosition = 0;
          }else{
            currentPosition = 1;
          }
      }else if(pattern == 5){
          //Serial.println("Alternate 1-3");         
          if(previousPosition != 0){
            Serial.println(previousPosition);
            currentPosition = 0;
          }else{
            currentPosition = 2;
          }
      }else if(pattern == 6){
        //   Serial.println("Random");         
          while(currentPosition == previousPosition || chords[chord][currentPosition] == 0){
            currentPosition = random(0,5);
          }
      }
      
      //Serial.print("Playing note: ");
      //Serial.println(chords[chord][currentPosition]);
      //Serial.print("Value: ");
      
      //Reset previous...
      previousPosition = currentPosition;

      int note = (((chords[chord][currentPosition] + root) * 4) + (12 * 4 * octave));
      Serial.println(note);
      dacOutput(note);
      
    }
  
}


//  =================== convenience routines ===================

//  These routines are some things you will need to use for
//  various functions of the hardware. Explanations are provided
//  to help you know when to use them.

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

//  ===================== end of program =======================
