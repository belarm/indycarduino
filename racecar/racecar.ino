// Arduino program to control a timer based on a series of triggers
// Written by Kristen Arnold


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

// Analog pins for reading the available sensors
int lugOne   = 8;
int lugTwo   = 9;
int lugThree = 10;
int lugFour  = 11;
int lugFive  = 12;
int jack     = 13;
int fuel     = 14;

int lugs[] = {8, 9, 10, 11, 12};

const int SENSOR_THRESHOLD = 100;

unsigned long timerStart;

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
  int val;
  boolean reset = false;
  
  if (vals[0] && vals[1] && vals[2] && vals[3] && vals[4]) {
    reset = true;
  } else {
    drawScreen("reset");
  }

  if (reset) {
  
    unsigned long timerStart = millis();
  
    for (int i = 0; i< 100000; i++) {
    
      unsigned long timeReading = getTime();
  
      drawScreen(String(timeReading) + "sec");
  
      if (val < 1023) {
        //drawScreen("Off");
      } else {
        drawScreen("On");
        timerStart = millis();
      }
      delay(100);
    } 
  }
}

static void drawScreen(String text)
{
  matrix.fillScreen(0);
  matrix.setTextColor(matrix.ColorHSV(255, 255, 255, true));
  matrix.setCursor(1, 4);
  matrix.print(text);
  matrix.swapBuffers(false);

}

unsigned long getTime()
{
    unsigned long time = millis();
    
    unsigned long elapsed = (time - timerStart) / 1000;
    //Serial.print("time: ");
    //Serial.println(time);
    //Serial.print("timeStart: ");
    //Serial.println(timerStart);
    //Serial.print("elapsed");
    //Serial.println(elapsed);
    //Serial.println("------------");
    
    return elapsed;
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
  //Serial.println(val);
  if (val > SENSOR_THRESHOLD) {
    return true;
  }

  return false; 
}
