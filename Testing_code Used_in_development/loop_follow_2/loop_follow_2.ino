#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *leftMotor = AFMS.getMotor(1);
Adafruit_DCMotor *rightMotor = AFMS.getMotor(2);
int frontLeftPin = 4;
int frontRightPin = 1; // Connect sensor to input pin 3
int leftlinesensorPin = 2;
int rightlinesensorPin = 3; // Connect sensor to input pin 3
void setup() {
 Serial.begin(9600); // Init the serial port
 pinMode(leftlinesensorPin, INPUT); // declare LED as output
 pinMode(rightlinesensorPin, INPUT); // declare Micro switch as input
 pinMode(frontRightPin, INPUT); // declare LED as output
 pinMode(frontLeftPin, INPUT); // declare Micro switch as input

 if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
  // if (!AFMS.begin(1000)) {  // OR with a different frequency, say 1KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  Serial.println("Motor Shield found.");

  // Set the speed to start, from 0 (off) to 255 (max speed)

  // turn on motor
  leftMotor->run(RELEASE);
  rightMotor->run(RELEASE);
}
void loop(){
 int valLeft = digitalRead(leftlinesensorPin); // read left input value
 int valRight = digitalRead(rightlinesensorPin); // read right input value
 int valFrontLeft = digitalRead(frontLeftPin); // read left input value
 int valFrontRight = digitalRead(frontRightPin); // read right input value
  //Serial.println("Front: " << valFrontLeft << "  Back: " << valFrontRight << "  Right: " << valRight << "  Left: " << valLeft);
  int stop = 0;
  int turning = 0;

  if (valFrontLeft && valFrontRight && !stop)
  {
    // On a line, going straight. All good
    //continue straight
      leftMotor->setSpeed(255);
  leftMotor->run(FORWARD);
  rightMotor->setSpeed(255);
  rightMotor->run(FORWARD);
  }
  if (valRight) // ignoring left sensor!!!
{
        leftMotor->run(FORWARD);
      leftMotor->setSpeed(120);
      rightMotor->run(BACKWARD);
      rightMotor->setSpeed(100);
      Serial.println("Started rotating");

      while (valRight){
        delay(100);
        valRight = digitalRead(rightlinesensorPin); // read right input value
        Serial.println(valRight);
        Serial.println("RIGHT NOW ACTIVE");
      }
      Serial.println("RIGHT NOW INACTIVE");
      while(!valRight){
        delay(100);
      
        valRight = digitalRead(rightlinesensorPin); // read right input value
        Serial.println(valRight);
        Serial.println("RIGHT NOW INACTIVE");
      }
      
      Serial.println("RIGHT ACTIVE");
          leftMotor->setSpeed(150);
    leftMotor->run(FORWARD);
    rightMotor->setSpeed(150);
    rightMotor->run(FORWARD);


}
  

  else if (!valFrontLeft&& valFrontRight){
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(100);
    rightMotor->run(BACKWARD);
    rightMotor->setSpeed(100);
  }  
  else if (valFrontLeft&& !valFrontRight){
    leftMotor->run(BACKWARD);
    leftMotor->setSpeed(100);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(100);
  }



 delay(100);
}