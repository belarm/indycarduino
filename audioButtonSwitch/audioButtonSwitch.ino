/*************************
 * This sketch takes 5 inputs and lights up subsiquent outputs with a mosfet shield
 */

const int inputPins[] = {2, 3, 4, 5, 6};
const int outputPins[] = {8, 9, 10, 11, 12};

/**
 * @var float delayTime
 * An array of time in seconds to delay each trigger when a HIGH 
 * signal is sent
 */
const float delayTime[] = {
    8, 
    4, 
    4, 
    6, 
    8
};

unsigned long timer[] = { 0, 0, 0, 0, 0};

void setup()
{
    Serial.begin(9600);

    for (int i = 0; i < 5; i++) {
        pinMode(inputPins[i], INPUT);
        pinMode(outputPins[i], OUTPUT);
    }
}

void loop()
{

    for (int i = 0; i < 5; i++) {
        if (digitalRead(inputPins[i]) == HIGH) {
            timer[i] = millis();
            digitalWrite(outputPins[i], HIGH);
        }

        if (millis() - timer[i] >= delayTime[i] * 1000) {
            digitalWrite(outputPins[i], LOW);

        }
    }

}

