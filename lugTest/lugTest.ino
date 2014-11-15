// Arduino program to control a timer based on a series of triggers
// @author Kristen Arnold


#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

// Similar to F(), but for PROGMEM string pointers rather than literals
#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr

#define CLK 11  // MUST be on PORTB! (Use pin 11 on Mega)
#define LAT A3
#define OE  9
#define A   A0
#define B   A1
#define C   A2
// Last parameter = 'true' enables double-buffering, for flicker-free,
// buttery smooth animation.  Note that NOTHING WILL SHOW ON THE DISPLAY
// until the first call to swapBuffers().  This is normal.
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, true);

// Analog pins for reading the lug nut sensors
int lugs[] = {8, 9, 10, 11, 12};

int jack = 14;

const int SENSOR_THRESHOLD = 100;


void setup()
{
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setTextSize(1);
    
  Serial.begin(9600);
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
  }
  
  return values;
}
  
static boolean readSensor(int pin)
{

  int val = analogRead(pin);
//  Serial.print("pin: ");
//  Serial.print(pin);
//  Serial.print(" = ");
//  Serial.println(val);
  if (val > SENSOR_THRESHOLD) {
    return true;
  }

  return false; 
}
