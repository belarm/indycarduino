// This is a simple test sketch to test the timer on the raspberry pi.
// Connect a switch to pin 8 with a pull up resistor.  Once loaded the raspberry
// pi will listen for the serial commands to start and stop the timer.

boolean state = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(8, INPUT);
  Serial.begin(9600);
}

void loop() {
  
  // put your main code here, to run repeatedly:
  if (digitalRead(8) == HIGH && state == false) {
    state = true;
    Serial.write("start\n");
  }
  
  if (digitalRead(8) == LOW && state == true) {
    Serial.write("stop\n");
    state = false;
  }
  
  Serial.write(".\n");
  delay(500);
}
