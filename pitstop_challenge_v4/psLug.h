#ifndef pslug_h
#define pslug_h
#include <Arduino.h>

static const int NumLugs = 5;
//static const int NumWheels = 2;

static const int WheelIdle = 0;    //Not running
static const int WheelPhase1 = 1;  //Lugs green, magnet on
static const int WheelPhase2 = 2;  //Lugs amber, magnet disengages until another lug is set green
static const int WheelDone = 3;    //Sequence complete; tire changed



class psLug {
public:
  int switchPin;
  int relayPin;
  boolean checkSwitch();
  boolean getStatus();
  boolean setStatus(boolean state);
  int setSwitchPin(int sPin);
  int setRelayPin(int rPin);
  void reset();
private:
  boolean status; //false: green, true: amber
};

class psLugGroup {
public:
  psLugGroup(const int sPins[], const int rPins[], const int mPin);

  psLug lugs[NumLugs];
  int phase;
  unsigned long changeTime;

  int checkPhase();
  void setMagnet(boolean state);
  boolean getMagnet();
  boolean playSound();
  void reset();
private:
  boolean magnetStatus;
  int magnetPin;
  boolean soundQueued;
};

#endif

