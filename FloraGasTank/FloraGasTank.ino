#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#define PIN 12
#define SWITCHPIN 6
#define OUTPIN 9

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  Serial.begin(9600);
  Serial.write("Initializing");
  pinMode(SWITCHPIN, INPUT);
  pinMode(OUTPIN, OUTPUT);
}

void loop() {
  
  uint32_t yellow = strip.Color(255, 100, 0);
  uint32_t green = strip.Color(0, 255, 0);
  
  digitalWrite(OUTPIN, LOW);
  
  int val = digitalRead(SWITCHPIN);
  
  if (val == HIGH) {
    
    // Begin light sequence, if sensor stops at any point reset the sequence
    
    for (int i=0; i < strip.numPixels(); i=i+1) {
      strip.setPixelColor(i, yellow);
      strip.show();
    
      delay(312);
      
      val = digitalRead(SWITCHPIN);
      
      if (val != HIGH) {
        return;
      }
    }

    setLights(green, 0);
    
    digitalWrite(OUTPIN, HIGH);
   
    delay(2000);  
  } else {
    // Shut off lights     
    setLights(0, 0);
  }
}

void setLights(uint32_t color, int wait) {
  for (int i=0; i < strip.numPixels(); i=i+1) {
    strip.setPixelColor(i, color);
    strip.show();
    
    delay(wait);
  }
     

}
