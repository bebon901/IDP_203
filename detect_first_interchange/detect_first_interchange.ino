#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *leftMotor = AFMS.getMotor(1);
Adafruit_DCMotor *rightMotor = AFMS.getMotor(2);
int frontsensorpin = 1;
int backsensorpin = 4; // Connect sensor to input pin 3
int leftlinesensorPin = 2;
int rightlinesensorPin = 3; // Connect sensor to input pin 3
void setup() {
 Serial.begin(9600); // Init the serial port
 pinMode(leftlinesensorPin, INPUT); // declare LED as output
 pinMode(rightlinesensorPin, INPUT); // declare Micro switch as input
 pinMode(backsensorpin, INPUT); // declare LED as output
 pinMode(frontsensorpin, INPUT); // declare Micro switch as input

 if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
  // if (!AFMS.begin(1000)) {  // OR with a different frequency, say 1KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  Serial.println("Motor Shield found.");

  // Set the speed to start, from 0 (off) to 255 (max speed)
  leftMotor->setSpeed(150);
  leftMotor->run(FORWARD);
  rightMotor->setSpeed(150);
  rightMotor->run(FORWARD);

  // turn on motor
  leftMotor->run(RELEASE);
  rightMotor->run(RELEASE);
}
void loop(){
 int valLeft = digitalRead(leftlinesensorPin); // read left input value
 int valRight = digitalRead(rightlinesensorPin); // read right input value
 int valFront = digitalRead(frontsensorpin); // read left input value
 int valBack = digitalRead(backsensorpin); // read right input value

  if (valFront && valBack && !valRight && !valLeft)
  {
    // On a line, going straight. All good
    //continue straight
  }
  
  elseif (!valFront && valLeft && valRight && valRight)
  {
    // We have reached the front!. Stop
    Serial.println("Reached Destination");
    rightMotor->setSpeed(0);
    leftMotor->setSpeed(0);
  }

 delay(100);
}
