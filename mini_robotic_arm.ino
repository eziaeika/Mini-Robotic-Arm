#include <TimedAction.h>
#include <Servo.h>

// Initializes the LED pins
#define playLED 11   // play mode indicator
#define recordLED 10 // record mode indicator

// Functions declaration
void manual();
void record();

// Initializes a TimedAction class for manual() every 50ms
TimedAction manualThread = TimedAction(50,manual);

// Initiallizes the TimeAction for record() every 60ms
TimedAction recordThread = TimedAction(60,record);

// Initializes number of position for storage array
short sizeCounter = 0;

// Initializes the servo pins
#define baseServoPin 3
#define shoulderServoPin 5
#define elbowServoPin 6
#define gripperServoPin 9

// Initializes all of the needed servo motors
Servo baseServo;
Servo shoulderServo;
Servo elbowServo;
Servo gripperServo;

// Initializes all potentionmeter pins
#define basePot A0
#define shoulderPot A1
#define elbowPot A2
#define gripperPot A3

int basePotVal;
int shoulderPotVal;
int elbowPotVal;
int gripperPotVal;

// Initializes buttons and their value holders
#define playButton 8
#define recordButton 7
boolean playButtonState = false;
boolean recordButtonState = false;

// Initializes the size of storage array
const short arraySize = 310;

// Initializes array for storing the 4 potentiometer values
byte baseServoPosStorge[arraySize];
byte shoulderServoPosStorge[arraySize];
byte elbowServoPosStorge[arraySize];
byte gripperServoPosStorge[arraySize];

void setup() {

  // Declared the pin attachments of all servo motors
  baseServo.attach(baseServoPin);
  shoulderServo.attach(shoulderServoPin);
  elbowServo.attach(elbowServoPin);
  gripperServo.attach(gripperServoPin);

  // Declared output mode for LEDs
  pinMode(playLED, OUTPUT);
  pinMode(recordLED, OUTPUT);

  // Declared input mode for the buttons
  pinMode(playButton, INPUT);
  pinMode(recordButton, INPUT);

  // Debug statement
  Serial.begin(9600);
}// End of setup()

void loop() {

  // Start manual control of the arm when record is not on
  if (playButtonState == false) {

    // Manual mode start at the correct time
    manualThread.check();

    // Check the status of play mode on the play button
    if (digitalRead(playButton) == HIGH) {
      playButtonState = true;
      recordButtonState = false;

      // Indicate play mode is on
      digitalWrite(playLED, HIGH);

      // Stop the manual thread
      manualThread.disable();

      // Stop the thread from running for the record function when play mode is on
      recordThread.disable();

      // Indicate the recording mode is off
      digitalWrite(recordLED, LOW);
    }

    // Check record mode and button is using pull-down resistor
    if (digitalRead(recordButton) == HIGH) {

      // Block record mode when the storage array is full
      if (sizeCounter < arraySize) {
        recordButtonState = true;

        // Indicate record mode is on
       digitalWrite(recordLED, HIGH);
      }
    }
  }

  // Run the recording func when the record button is pressed
  if (recordButtonState) {
    recordThread.check();
  }

  // Run the playing func when the play button is pressed
  if (playButtonState) {

    for (short j = 0; j <= sizeCounter; j++) {
      baseServo.write(baseServoPosStorge[j]);
      shoulderServo.write(shoulderServoPosStorge[j]);
      elbowServo.write(elbowServoPosStorge[j]);
      gripperServo.write(gripperServoPosStorge[j]);

      delay(60);
    }
  }
}// End of loop()

//=========================================================================
// Function will store pot values into an array. one pot per array
void record() {

  // Break out the record thread when array is used up
  if (sizeCounter >= arraySize) {

    // Stop the thread from running for the record function
    recordThread.disable();

    // Indicate record mode is off
    digitalWrite(recordLED, LOW);
    return;
  }

  // Record the position of potentionmeters
  baseServoPosStorge[sizeCounter]     = basePotVal;
  shoulderServoPosStorge[sizeCounter] = shoulderPotVal;
  elbowServoPosStorge[sizeCounter]    = elbowPotVal;
  gripperServoPosStorge[sizeCounter]  = gripperPotVal;

  // Increment the array position to next avaliable space within the array
  sizeCounter = sizeCounter + 1;
}// End of record()

//============================================================================
// Function will directly control the arm
void manual() {

  // Read the base potentiometer value between 0 and 1023
  // Map value of the base potentiometer from 0-650 to the value of the base servo 0-180
  // Map for 1 to 1 ratio in term of rotation degree
  basePotVal = map(analogRead(basePot), 0, 600, 5, 180);
  baseServo.write(basePotVal); // Set servo position according to scaled value

  shoulderPotVal = map(analogRead(shoulderPot), 0, 600, 5, 180);
  shoulderServo.write(shoulderPotVal);

  elbowPotVal = map(analogRead(elbowPot), 0, 600, 5, 180);
  elbowServo.write(elbowPotVal);

  gripperPotVal = map(analogRead(gripperPot), 0, 600, 5, 180);
  gripperServo.write(gripperPotVal);
}
