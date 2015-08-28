#include "psLug.h"


const int rPins1[] = {23, 25, 27, 29, 31};
const int rPins2[] = {22, 24, 26, 28, 30};
const int sPins1[] = {2, 3, 4, 5, 6};
const int sPins2[] = {8, 9, 10, 11, 12};
const int magnet1 = 33;
const int magnet2 = 32;

const int lugTrigger = 52;
const int gasTrigger = 53;
const int gasPin = A1;
const int startPin = A0;

const int triggerDelay = 100;
const int WheelDelay = 500;

unsigned long lugSoundTime = 0;
unsigned long gasSoundTime = 0;
unsigned long runTime = 0;
boolean lugPlaying = false;
boolean gasPlaying = false;
boolean gasComplete = false;
boolean running = false;
unsigned long timeOut = 60000 * 2;


psLugGroup wheel1(sPins1,rPins1,magnet1);
psLugGroup wheel2(sPins2,rPins2,magnet2);


void setup() {
  //Start button
  pinMode(startPin,INPUT);
  //Gas cap arduino sets this pin to indicate tank has been 'filled'
  pinMode(gasPin,INPUT);
  //Play lug and gas glug sounds
  pinMode(lugTrigger,OUTPUT);
  pinMode(gasTrigger,OUTPUT);
  digitalWrite(gasTrigger,LOW);
  digitalWrite(lugTrigger,LOW);
  pinMode(7,INPUT); //WHY?! [Still no idea]
  Serial.begin(9600);
}

void loop() {
  if(!running && digitalRead(startPin) == HIGH)
  {
    Serial.println("start");
    //Set wheels to Phase1: magnet engaged, all lugs 'on'
    wheel1.phase = WheelPhase1;
    wheel2.phase = WheelPhase1;
    running = true;
    runTime = millis();
  }
  if(running)
  {
    if ((wheel1.phase != WheelDone) && (millis() - wheel1.changeTime > WheelDelay))
      wheel1.checkPhase();
    if ((wheel2.phase != WheelDone) && (millis() - wheel2.changeTime > WheelDelay))
      wheel2.checkPhase();
    if(!gasComplete)
      checkGas();
    if(wheel1.playSound() || wheel2.playSound()) //playSound() indicates if a sound is queued, and clears it if so.
      playLugSound();
    if(wheel1.phase == WheelDone && wheel2.phase == WheelDone && gasComplete) //all done!
    {
//      wheel1.phase = WheelIdle;
//      wheel2.phase = WheelIdle;
      wheel1.reset();
      wheel2.reset();
      gasComplete = false;
      Serial.println("stop");
      running = false;
      runTime = 0;
    }
    if(millis() - runTime > timeOut)
    {
      //timeout exceeded, reset
      Serial.println("stop");
      //sleep 1s to allow Pi to display time and reset timer.
      delay(1000);
      //Reset all variables and state.
      stopSounds();
      wheel1.reset();
      wheel2.reset();
      gasComplete = false;
      runTime = 0;
      running = false;
    }
  //  stopSounds();
  }
}

void stopSounds()
{
  /*if(lugPlaying == true && millis() - lugSoundTime > triggerDelay)
  {
    digitalWrite(lugTrigger,LOW);
    lugPlaying = false;
    lugSoundTime = 0;
  }*/
  
  if(gasPlaying == true && millis() - gasSoundTime > triggerDelay)
  {
    digitalWrite(gasTrigger,LOW);
    gasPlaying = false;
    gasSoundTime = 0;
  }
}

boolean playLugSound()
{
    digitalWrite(lugTrigger,LOW);
    delay(20);
    digitalWrite(lugTrigger,HIGH);
    return true;
}

boolean playGasSound()
{
  if(!gasPlaying)
  {
    gasPlaying = true;
    gasSoundTime = millis();
    digitalWrite(gasTrigger,HIGH);
    return true;
  }
  return false;
}

void checkGas()
{
  //Gas cap arduino sets this pin HIGH when the filling sequence is complete
  //Can this be modified so that the line is set either when the can contacts the switch,
  //or perhaps ~1s later?
  int val = digitalRead(gasPin);
  if(val == HIGH)
  {
    playGasSound();
    gasComplete = true;
  }
}

