#include "psLug.h"


int psLug::setSwitchPin(int sPin) {
  //Register pins attached to reed switches in lugs
  switchPin = sPin;
  pinMode(switchPin,INPUT);
  return sPin;
}

int psLug::setRelayPin(int rPin) {
  //Register and set signal on relays controlling green/amber lights in lugs
  relayPin = rPin;
  pinMode(relayPin,OUTPUT);
  digitalWrite(relayPin,LOW);
  return rPin;
}

boolean psLug::checkSwitch() {
  boolean state;
  if(digitalRead(switchPin) == HIGH) //lug wrench is touched to lug
  {
    delay(50); //debounce
    return true;
  }
  else return false;
}

boolean psLug::getStatus() { 
  return status; 
}

boolean psLug::setStatus(boolean state) {
  //Set LED relay to indicated state
  digitalWrite(relayPin,state);
  return status = state;
}

void psLug::reset() {
  //Return to default state: light green, wrench not present.
  digitalWrite(relayPin,LOW);
  status = false;
}

//Container for psLugs, representing one wheel

psLugGroup::psLugGroup (const int sPins[], const int rPins[], const int mPin)
{
  for (int i=0;i<NumLugs;i++){
    //Initiate all lugs, associating with passed arrays of switch and relay pins
    lugs[i].setSwitchPin(sPins[i]);
    lugs[i].setRelayPin(rPins[i]);
    lugs[i].setStatus(false);
  }
  //This pin will control the magnet at the center of the wheel
  magnetPin = mPin;
  pinMode(magnetPin,OUTPUT);
  //Engage magnet
  this->setMagnet(true);
  changeTime = 0;
  soundQueued = false;
  //phase tracks where we are in the sequence, as documented in psLug.h
  phase = WheelIdle;
}

//Check for progress in our current phase, updating LugGroup and Lugs as needed
//False - in progress
//True - phase complete, phase incremented
int psLugGroup::checkPhase()
{ 
  switch(phase) {

  case WheelIdle:
    return true;

  case WheelPhase1:
    for(int i = 0;i<NumLugs;i++)
      if(lugs[i].getStatus() == false && lugs[i].checkSwitch()) //Lug is green, wrench is touching
      {
        lugs[i].setStatus(true);
        soundQueued = true;
      }
    for(int i = 0;i<NumLugs;i++) //Check if any lugs are still green, return false if so
      if(lugs[i].getStatus() == false)
        return false;
    this->setMagnet(false); //Disengage magnet
    phase = WheelPhase2;    //Increment phase
    changeTime = millis();
    return true;

  case WheelPhase2:
    for(int i = 0;i<NumLugs;i++)
      if(lugs[i].getStatus() == true && lugs[i].checkSwitch()) //Lug is amber, wrench is touching
      {
        lugs[i].setStatus(false);
        soundQueued = true;
        if(!this->getMagnet())
          this->setMagnet(true); //Engage magnet to grab hubcap
      }
    for(int i = 0;i<NumLugs;i++) //Check if any lugs are still amber, return false if so
      if(lugs[i].getStatus() == true)
        return false;
    phase = WheelDone;      //Increment phase
    changeTime = millis();
    return true;

  default:
    return true;
  }
}

void psLugGroup::setMagnet(boolean state) {
  digitalWrite(magnetPin,!state);
  magnetStatus = state;
}

boolean psLugGroup::getMagnet() { 
  return magnetStatus; 
}

boolean psLugGroup::playSound() //If a sound is queued, unset soundQueued and return true
{
  if(soundQueued == true)
  {
    soundQueued = false;
    return true;
  }
  return false;
}

void psLugGroup::reset() {
  for(int i = 0;i<NumLugs;i++)
    lugs[i].reset();
  this->setMagnet(true);
  changeTime = 0;
  soundQueued = false;
  phase = WheelIdle;
}

