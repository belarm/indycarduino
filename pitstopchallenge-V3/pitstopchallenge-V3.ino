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
 *  Gas Tank Pin: 53
 *
 *  Start Button:
 *      Pin 51 attached to switch, other pin should be to 5vdc    
 *
 *  Reset Button:
 *      The reset button should be attached to the "reset" pin and the other end to ground
 *
 * NOTES:
 *
 * 1. When relays are in a state of LOW they will be green or engaged.  
 *    If the relays move to state HIGH then they will be amber or disengaged
 */
 
int switchGroupOne[] = {2, 3, 4, 5, 6};
int switchGroupTwo[] = {8, 9, 10, 11, 12};

int relayGroupOne[] = {23, 25, 27, 29, 31, 33};
int relayGroupTwo[] = {22, 24, 26, 28, 30, 32};

int gasTankPin = 53;
int startPin = A0;

boolean ledState[] = { false, false, false, false, false };
boolean stepSequence[] = { false, false };
int startState = 0;

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
    Serial.println("Initializing");
    
    // Set all lugs to green
    for (int i=0; i <= sizeof(relayGroupOne); i++) {
      digitalWrite(relayGroupOne[i], LOW);
      digitalWrite(relayGroupTwo[i], LOW); 
    }
    
    // Send stop command just in case timer is still running
    Serial.write("stop\n");
  
    firstRun = false;  
  }
  
  startState = digitalRead(startPin);
  
  if (startState == HIGH) {
    Serial.println("Start button has been pushed");
    beginSequence(); 
  }
  
  // Send dumb character to console so timer doesnt wait for a command
  //Serial.write(".\n");
}

void beginSequence() {
  stepSequence[0] = isLugsComplete();
 
  // If any steps in the sequence are not complete keep looping thru this.
  for (int i = 0; i < sizeof(stepSequence); i++) {
    if (!stepSequence[i]) {
      beginSequence(); 
      break; 
    }
  }
  
//  stepsState[1] = digitalRead(gasTankPin);
  
  
  // loop thru state array.  if everything in the sequence is complete then return and stop timer
  // else keep running this function
  
}

boolean isLugsComplete() { 
  static boolean firstPassDone = false;
 
  // First pass, turn all to amber
  if (!firstPassDone) {
    //Serial.println("Beginning first pass of lug nuts");
    
    // Watch lugs and change color when they are engaged
    for (int i=0; i < sizeof(switchGroupOne); i++) {
      int mode = digitalRead(switchGroupOne[i]);
    
      if (mode == HIGH) {

        digitalWrite(relayGroupOne[i], HIGH);
        ledState[i] = true;
//        Serial.print("switch ");
//        Serial.print(i);
//        Serial.println(" is on");
//      
//        if (stepsState[i] == true) {
//          digitalWrite(relayGroupOne[i], LOW);
//          ledState[i] = false;
//        } else {
//          digitalWrite(relayGroupOne[i], HIGH);
//          ledState[i] = true;
//        }
//      
//        delay(300);
      }
    }
  
    for (int i=0; i < sizeof(ledState); i++) {
        Serial.print("switch ");
        Serial.print(i);
        Serial.print(" is ");
      if (ledState[i]) {
        Serial.println("on");
        firstPassDone = true;
      } else {
        Serial.println("off");
        firstPassDone = false;
        break;
      }
    }
    
    if (firstPassDone) {
      Serial.println("First Pass Complete");
      
      // Turn off electromagnet to disengage wheel
      digitalWrite(relayGroupOne[5], HIGH);
    } 
  }
  
  // Second Pass, turn all back to green
}









