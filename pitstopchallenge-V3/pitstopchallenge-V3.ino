/*************************
 * Pitstop Challenge
 *
 * PINOUTS:
 *  Relay Groups:
 *      Relay Group #1: Pins 23, 25, 27, 29, 31, 33
 *      Relay Group #2: Pins 22, 24, 26, 28, 30, 32
 *
 *  Magnetic Switch Group:
 *      Switch Group #1: 2, 3, 4, 5, 6
 *      Switch Group #2: 8, 9, 10, 11, 12
 *
 *  Gas Tank Pin: A1
 *      Because we are reading the pin with analogRead it is 
 *      represented as 1 instead of A1 in code below
 *
 *  Start Button:
 *      Pin A0 attached to switch, other pin should be to 5vdc    
 *
 *  Reset Button:
 *      The reset button should be attached to the "reset" pin and the other end to ground
 *
 * NOTES:
 *
 * 1. When relays are in a state of LOW they will be green or engaged.  
 *    If the relays move to state HIGH then they will be amber or disengaged
 *
 * 2. When relay is low electromagnet will be engaged
 */
 
const int switchGroupOne[] = {2, 3, 4, 5, 6};
const int switchGroupTwo[] = {8, 9, 10, 11, 12};

const int relayGroupOne[] = {23, 25, 27, 29, 31, 33};
const int relayGroupTwo[] = {22, 24, 26, 28, 30, 32};

const int gasTankPin = 1; // Analog 1
const int startPin = A0;

boolean ledState[] = { false, false, false, false, false };
int startState = 0;

boolean firstRun = true;

void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  
  for (int i=0; i < 6; i++) {
    pinMode(relayGroupOne[i], OUTPUT); 
    pinMode(relayGroupTwo[i], OUTPUT);
  }
  
  for (int i=0; i <= 5; i++) {
    pinMode(switchGroupOne[i], INPUT);
    pinMode(switchGroupTwo[i], INPUT);
  }
  
  pinMode(gasTankPin, INPUT);
  pinMode(startPin, INPUT);
  
  //pinMode(solidStatePin, OUTPUT);  // set pin as an output 
  //pinMode(audioPin, OUTPUT);  
}

void loop() {
  
  if (firstRun) {
    
    // Set all lugs to green and magnet to on
    for (int i=0; i < 6; i++) {
      digitalWrite(relayGroupOne[i], LOW);
      digitalWrite(relayGroupTwo[i], LOW); 
    }
    
    // Send stop command just in case timer is still running
    Serial.write("stop\n");
  
    firstRun = false;  
  }
  
  startState = digitalRead(startPin);
  
  if (startState == HIGH) {
    Serial.write("start\n");
    beginSequence(); 
  }
  
  // Send dumb character to console so timer doesnt wait for a command
  //Serial.write(".\n");
}

void beginSequence() {
  static boolean stepSequence[] = { false, false };
  // Send dumb character to console so timer doesnt wait for a command
  //Serial.write(".\n");
  
  if (stepSequence[0] == false) {
    // Test lug sequence
    stepSequence[0] = isLugsComplete();
  }
  
  // Test gas tank sequence
  if (stepSequence[1] == false) {
    int gas = analogRead(gasTankPin);
    
    if (gas != LOW) {
      stepSequence[1] = true;
    } else {
      stepSequence[1] = false;
    }
  }
 
  // If any steps in the sequence are not complete keep looping thru this.
  for (int i = 0; i < 2; i++) {

    if (!stepSequence[i]) {
      beginSequence(); 
      break; 
    } 
  }
  
  Serial.write("BLARGstop\n");
  softwareReset();
  
}

boolean isLugsComplete() { 
  static boolean firstPassDone = false;
  static boolean secondPassDone = false;
 
  // First pass, turn all to amber
  if (!firstPassDone && !secondPassDone) {
    
    // Watch lugs and change color when they are engaged
    for (int i=0; i < 5; i++) {
//        Serial.print("check switch ");
//        Serial.println(i);
      int mode = digitalRead(switchGroupOne[i]);
    
      if (mode == HIGH) {
//        Serial.print("turn on switch ");
//        Serial.println(i);
        
        digitalWrite(relayGroupOne[i], HIGH);
        ledState[i] = true;
        
      }
    }
  
    // After looping thru lugs, inspect the state of each one and decide if we
    // should continue to phase two
    for (int i=0; i < 5; i++) {
      if (ledState[i]) {
        firstPassDone = true;
      } else {
        //Serial.println("lugs partially changed");
        firstPassDone = false;
        break;
      }
    }
    
    if (firstPassDone) {
      //Serial.println("First Pass Complete");

      // Turn off electromagnet to disengage wheel
      digitalWrite(relayGroupOne[5], HIGH);
      delay(300);
    } 
  }
    
  // Second Pass, turn all back to green
  
  if (firstPassDone && !secondPassDone) {

    //Serial.println("Begin Second pass");
    
    // Watch lugs and change color when they are engaged
    for (int i=0; i < 5; i++) {
      int mode = digitalRead(switchGroupOne[i]);
    
      if (mode == HIGH) {
//        Serial.print("turn off switch ");
//        Serial.println(i);
        
        // Turn on electromagnet to engage wheel
        digitalWrite(relayGroupOne[5], LOW);
    
        digitalWrite(relayGroupOne[i], LOW);
        ledState[i] = false;
        
      }
    }
    
    // After looping thru lugs, inspect the state of each one and decide if we
    // should continue to phase two
    for (int i=0; i < 5; i++) {
      if (!ledState[i]) {
        secondPassDone = true;
      } else {
        secondPassDone = false;
        //Serial.println("some lugs still on");
        break;
      }
    }
    
    if (secondPassDone) {
      Serial.println("Second Pass Complete");
      return true;
    } 
  }
  
  return false;
}

/**
 * Restarts program from beginning but 
 * does not reset the peripherals and registers
 */
void softwareReset()
{
  asm volatile ("  jmp 0");  
} 








