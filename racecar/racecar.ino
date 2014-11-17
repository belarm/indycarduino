// Arduino program to control a timer based on a series of triggers
// @author Kristen Arnold


#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <StopWatch.h> // Stop watch library

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

StopWatch sw(StopWatch::SECONDS);

const int SENSOR_THRESHOLD = 100;

boolean reset = false;
boolean cycleComplete = true;
boolean phaseOne = false;

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
  
  //boolean startTimer = false;

  // Begin sequence if all lugs are returned to green status after power on
  if (reset) {
    
    // Begin sequence if jack is triggered
    if (analogRead(jack) >= 686 || !cycleComplete) {
      //Serial.println("Jack has been raised");
      cycleComplete = false;
      
      // Watch for all lugs to go red
      if (phaseOne == false) {
        //startTimer = true;
        Serial.println("Phase one initiated");
        sw.start();
            
        // When the lugs have all gone red trigger that this step is complete
        if (!vals[0] && !vals[1] && !vals[2] && !vals[3] && !vals[4]) {
          Serial.println("All lugs have turned red");
          phaseOne = true;  
        }
      } else {
        Serial.println("Phase two initiated");
        
        // After cycling thru red, now all lugs must cycle back to green
        
        //TODO add check against gas line as well
        if (vals[0] && vals[1] && vals[2] && vals[3] && vals[4]) {
          Serial.println("All lugs have turned green");
          phaseOne = false;
          cycleComplete = true;
          sw.stop();
        }
      }
      
      drawScreen(String(sw.elapsed()) + "sec");
        
    } else {
      // Jack is lowered and sequence should be complete
      Serial.println("Jack has been lowered");
      
      // Pause for 30 seconds and then reset the delay
      delay(30000);
      drawScreen("Ready!");
    }
    
  } else {
    Serial.println("Initializing system");
    
    if (vals[0] && vals[1] && vals[2] && vals[3] && vals[4]) {
      Serial.println("All systems go");
      reset = true;
    } else {
      Serial.println("Please reset lugs");
      drawScreen("reset");
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
  if (val > SENSOR_THRESHOLD) {
    return true;
  }

  return false; 
}
