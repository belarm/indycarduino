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
 *      This pin should attach to the arduino flora circuit
 *
 *  Start Button:
 *      Pin A0 attached to switch, other pin should be to 5vdc    
 *
 *  Reset Button:
 *      The reset button should be attached to the "reset" pin and the other end to ground
 *
 *  Triggers:
 *      Lug trigger: 52
 *        This pin sends a HIGH signal when any lugs are triggered
 *      Gas Tank trigger: 53
 *        This pin sends a HIGH signal when the gas tank sequence is complete
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

const int gasTankPin = A1;
const int startPin = A0;

const int lugTrigger = 52;
const int gasTrigger = 53;

/**
 * @var triggerDelay
 *  Set the interval in milliseconds to keep the trigger pins HIGH.
 */
const int triggerDelay = 100;

/**
 * @var wheelDelay
 * the time to pause after all lugs have been disengaged
 */
const int wheelDelay = 500;

/**
 * @var restartTimeout
 *   Change the last number to increment by X minutes.
 *   NOTE: that times over 2 minutes may eat up memory and cause the
 *   board to reset on its own
 */
const float restartTimeout = 60000 * 2;

/**
 * @var useTwoWheels
 * Set true to support a system with two wheels with lugs
 * Set false to support only one set of wheels
 */
const boolean useTwoWheels = true;

/* Do not change anything below this line */

boolean ledState[] = { false, false, false, false, false, false, false, false, false, false };

boolean stepSequence[] = { false, false };
int startState = 0;
int i = 0;

boolean firstRun = true;

void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  
  for (int i=0; i <= 6; i++) {
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
    //Serial.println("Initializing");
    
    // Set all lugs to green
    for (int i=0; i < sizeof(relayGroupOne); i++) {
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

  // Timeout if no one completes the sequence
  if (millis() > restartTimeout) {
    softwareReset();
  }  
  
}

void beginSequence() {

  // Timeout if no one completes the sequence
  if (millis() > restartTimeout) {
    softwareReset();
  }
  
  delay(50);
  
  if (!stepSequence[0]) {
    stepSequence[0] = isLugsComplete();
  }

  if (!stepSequence[1]) {
    stepSequence[1] = isGasComplete();
  }
 
  // If any steps in the sequence are not complete keep looping thru this.
  for (i = 0; i < 2; i++) {
    
    if (!stepSequence[i]) {
      beginSequence(); 
      break;
    }
  }
    
  softwareReset();
}

boolean isLugsComplete() { 
  static boolean OneFirstPassDone = false;
  static boolean OneSecondPassDone = false;
  static boolean TwoFirstPassDone = false;
  static boolean TwoSecondPassDone = false;
 
  // First Wheel, First pass, turn all to amber
  if (!OneFirstPassDone && !OneSecondPassDone) {
    OneFirstPassDone = isFirstPass(relayGroupOne, switchGroupOne, 0);
  }
    
  // First Wheel, Second Pass, turn all back to green
  if (OneFirstPassDone && !OneSecondPassDone) {
    OneSecondPassDone = isSecondPass(relayGroupOne, switchGroupOne, 0);
  }
  
  if (useTwoWheels) {
    // Second Wheel, First Pass, turn all to amber
    if (!TwoFirstPassDone && !TwoSecondPassDone) {
      TwoFirstPassDone = isFirstPass(relayGroupTwo, switchGroupTwo, 5);
    }
    
    // Second Wheel, Second Pass, turn all back to green
    if (TwoFirstPassDone && !TwoSecondPassDone) {
      TwoSecondPassDone = isSecondPass(relayGroupTwo, switchGroupTwo, 5);
    }
  } else {
    TwoFirstPassDone = true;
    TwoSecondPassDone = true;
  }
  
  return OneFirstPassDone && OneSecondPassDone && TwoFirstPassDone && TwoSecondPassDone;
}

/**
 * First pass will turn lights from green to amber
 */
boolean isFirstPass(const int relayGroup[], const int switchGroup[], int offset) {
  
  // Watch lugs and change color when they are engaged
  for (int i=0; i < 5; i++) {
    // int mode = digitalRead(switchGroup[i]);
  
    // if (mode == HIGH) {
    if (digitalRead(switchGroup[i]) == HIGH) {
      
      digitalWrite(relayGroup[i], HIGH);

      trigger(lugTrigger);
    
      ledState[i+offset] = true;
      
    }
  }

  // After looping thru lugs, inspect the state of each one and decide if we
  // should continue to phase two
  for (int i=offset; i < offset+5; i++) {
    if (!ledState[i]) {
      return false;
    }
  }

  // Turn off electromagnet to disengage wheel
  digitalWrite(relayGroup[5], HIGH);
  delay(wheelDelay);

  return true;
}

/**
 * Second pass will turn lights from amber back to green
 */
boolean isSecondPass(const int relayGroup[], int const switchGroup[], int offset) {
  // Watch lugs and change color when they are engaged
  for (int i=0; i < 5; i++) {
    // int mode = digitalRead(switchGroup[i]);
  
    // if (mode == HIGH) {
    if (digitalRead(switchGroup[i]) == HIGH) {
      
      // Turn on electromagnet to engage wheel
      digitalWrite(relayGroup[5], LOW);
  
      digitalWrite(relayGroup[i], LOW);

      trigger(lugTrigger);

      ledState[i+offset] = false;
      
    }
  }
  
  // After looping thru lugs, inspect the state of each one and decide if we
  // should continue to phase two
  for (int i=offset; i < offset+5; i++) {
    if (ledState[i]) {
      return false;
    }
  }
  
  return true; 
}

/**
 * Listen for a signal from the arduino flora to tell us
 * that the gas tank function has completed
 */
boolean isGasComplete() {
  static boolean isComplete = false;
    
  if (!isComplete) {
    int val = digitalRead(gasTankPin);
    
    if (val == HIGH) {
      trigger(gasTrigger);

      isComplete = true;
    }   
  }
  
  return isComplete;
}

/**
 * Restarts program from beginning but 
 * does not reset the peripherals and registers
 */
void softwareReset()
{
  delay(100);
  Serial.write("stop\n");
  delay(100);
  
  asm volatile ("  jmp 0");  
  
  delay(100);
} 

void trigger(const int pin) {
  digitalWrite(pin, HIGH);
  delay(triggerDelay);
  digitalWrite(pin, LOW);
}






