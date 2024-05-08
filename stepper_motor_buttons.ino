#include <Stepper.h>

const int stepsPerRevolution = 2038;
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

// Pins for the buttons
const int buttonPin0 = 0; // Button connected to digital pin 0
const int buttonPin1 = 1; // Button connected to digital pin 1

int buttonState0 = 0;
int buttonState1 = 0;

void setup() {
  pinMode(buttonPin0, INPUT);
  pinMode(buttonPin1, INPUT);
}

void loop() {
  buttonState0 = digitalRead(buttonPin0);
  buttonState1 = digitalRead(buttonPin1);

  // If button 0 is pressed, rotate the stepper clockwise
  if (buttonState0 == HIGH && buttonState1 == LOW) {
    myStepper.setSpeed(10);
    myStepper.step(stepsPerRevolution);
    delay(1000);
  }
  // If button 1 is pressed, rotate the stepper counterclockwise
  else if (buttonState1 == HIGH && buttonState0 == LOW) {
    myStepper.setSpeed(10);
    myStepper.step(-stepsPerRevolution);
    delay(1000);
  }
  // If both buttons are pressed simultaneously, do nothing
  else {
    // Do nothing
  }
}