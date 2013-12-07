
const int clkIn = 2;           // the digital (clock) input
const int pin[4] = {5, 6, 7, 8};  // the digital output pins
const int pinOffset = 5;       // the first DAC pin (from 5-12)
const int queueSize = 3;       // the size of the timing queue
const int trigTime = 25;       // default of 25 ms trigger time

//  variables for interrupt handling of the clock input
volatile int clkState = LOW;

int drumLoops = 0;

//  variables used to control the current DIO output states
int state[4] = {LOW, LOW, LOW, LOW};        // start with both set low
unsigned long milli[4] = {0,0,0,0};   // the number of milliseconds since last firing
long times[4] = {10000,10000,10000,10000};

unsigned long lastMilli = 0;         // the last time millis() was called
unsigned long digQueue[queueSize];   // the timing for the last six clock hits
int currentQueue = 0;
int queueReady = 0;

//  ==================== start of setup() ======================

void setup() {
    Serial.begin(9600);
  // set up the digital (clock) input
  pinMode(clkIn, INPUT);
  
  // set up the digital outputs
  for (int i=0; i<2; i++) {
    pinMode(pin[i], OUTPUT);
    digitalWrite(pin[i], LOW);
  }
  
  // set up the 8-bit DAC output pins
  for (int i=0; i<8; i++) {
    pinMode(pinOffset+i, OUTPUT);
    digitalWrite(pinOffset+i, LOW);
  }
  
  // Note: Interrupt 0 is for pin 2 (clkIn)
  attachInterrupt(0, isr, RISING);
}

//  ==================== start of loop() =======================

void loop()
{

  // check for clock input
  if (clkState) {
    
    drumLoops++;

    if (lastMilli != 0) {
      digQueue[currentQueue] = millis() - lastMilli;
      currentQueue++;
      
      if (currentQueue >= queueSize) {
        currentQueue = 0;
        queueReady = 1;
      }
      
      if (queueReady) {
        // on each clock in there will be at least one trig out
        for (int i=0; i<5; i++) {
          state[i] = HIGH;
          milli[i] = millis();
          digitalWrite(pin[i], state[i]);
        }
        
        
        
        
        long tempInterval = 0;
        for (int i=0; i<queueSize; i++) {
          tempInterval += digQueue[i];
        }
        tempInterval = tempInterval / queueSize;
        
        int a4 = analogRead(4);
        if (a4 == 515) {
          a4 =0;
        }
        
        int a5 = analogRead(5);
        if (a5 == 520) {
          a5 =0;
        }
        
        int a0WithCv = analogRead(0) + a4;
        if (a0WithCv > 1023) {
          a0WithCv = 1023;
        }
        
        int a1WithCv = analogRead(1) + a5;
        if (a1WithCv > 1023) {
          a1WithCv = 1023;
        }
        
        times[0] = (tempInterval / ((a0WithCv >> 6) + 1));
        times[1] = (tempInterval / ((a1WithCv >> 6) + 1));
        times[2] = (tempInterval / ((analogRead(2) >> 6) + 1));
        times[3] = (tempInterval / ((analogRead(3) >> 6) + 1));        
        
        // lower value if higher A0
        
//        Serial.println("---------------------");
//        Serial.print("times[0] ");  Serial.println(times[0]);
//        Serial.print("a4 ");  Serial.println(analogRead(4));        
//        Serial.print("a5 ");  Serial.println(analogRead(5));        
        
      }
    }
    
    lastMilli = millis();
    clkState = 0;
  }
  
  // the extra hits
  if (queueReady) {
    for (int i=0; i<5; i++) {
      if (millis() - milli[i] > times[i]) { // time passed since the first trig out get to the time for "times"
        milli[i] = millis();
        if (state[i] != HIGH) {
          state[i] = HIGH;
          digitalWrite(pin[i], HIGH);
        }
      }
    }
    
    if (drumLoops % 2 == 0) {
      digitalWrite(pinOffset+(4), HIGH);
      delay(1);
      digitalWrite(pinOffset+(4), LOW);
    }
    
    if (drumLoops % 4 == 0) {
      digitalWrite(pinOffset+(5), HIGH);
      delay(1);
      digitalWrite(pinOffset+(5), LOW);
    }
    
    if (drumLoops % 6 == 0) {
      digitalWrite(pinOffset+(6), HIGH);
      delay(1);
      digitalWrite(pinOffset+(6), LOW);
    }
    
    if (drumLoops % 8 == 0) {
      digitalWrite(pinOffset+(7), HIGH);
      delay(1);
      digitalWrite(pinOffset+(7), LOW);
    }

    // turn off triggers
    for (int i=0; i<5; i++) {
      if ((state[i] == HIGH) && (millis() - milli[i] > trigTime)) {
        state[i] = LOW;
        digitalWrite(pin[i], state[i]);
      }
    }
  }
}

//  =================== convenience routines ===================

//  isr() - quickly handle interrupts from the clock input
//  ------------------------------------------------------
void isr()
{
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
