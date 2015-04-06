// Arduino program to control a timer based on a series of triggers
// @author Kristen Arnold

// Analog pins for reading the lug nut sensors
const int lugs[] = {2, 3, 4, 5, 6};
const int relayGroup[] = {23, 25, 27, 29, 31, 33};

void setup()
{
    
  Serial.begin(9600);
  for (int i=0; i < 6; i++) {
    pinMode(relayGroup[i], OUTPUT); 
  }
  
  for (int i=0; i <= 5; i++) {
    pinMode(lugs[i], INPUT);
  }
}

void loop()
{ 
  int* vals = readSensors();
  
  Serial.print("lug1: ");
  Serial.print(vals[0]);
  Serial.print(" lug2:");
  Serial.print(vals[1]);
  Serial.print(" lug3:");
  Serial.print(vals[2]);
  Serial.print(" lug4:");
  Serial.print(vals[3]);
  Serial.print(" lug5:");
  Serial.println(vals[4]);
}

static int* readSensors()
{
  int values[] = {};
  
  for (int i = 0; i < 5; i++) {
    values[i] = readSensor(lugs[i]);
    if (values[i]) {
      digitalWrite(relayGroup[i], HIGH);
    }
  }
  
  return values;
}
  
static boolean readSensor(int pin)
{

  int val = digitalRead(pin);
//  Serial.print("pin: ");
//  Serial.print(pin);
//  Serial.print(" = ");
//  Serial.println(val);
  if (val == HIGH) {
    return true;
  }

  return false; 
}
