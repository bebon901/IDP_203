#include <Adafruit_MotorShield.h>
#include "Arduino.h"
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);
#define MAX_RANG (520)//the max measurement value of the module is 520cm(a little bit longer
#include "Wire.h"
#include "DFRobot_VL53L0X.h"
#define ADC_SOLUTION (1023.0)//ADC accuracy of Arduino UNO is 10bit
// select the input pin
float dist_t, sensity_t;

float distance_0 = 100;
float distance_1 = 100;
float distance_2 = 100;
bool detected = false;
float lidar = 100;


// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *leftMotor = AFMS.getMotor(1);
Adafruit_DCMotor *rightMotor = AFMS.getMotor(2);
DFRobot_VL53L0X sensor;

int frontLeftPin = 4;
int frontRightPin = 1; // Connect sensor to input pin 3
int leftlinesensorPin = 2;
int rightlinesensorPin = 3; // Connect sensor to input pin 3
int scanning = 0;
int sensityPin = A0; 

void setup() {
 Serial.begin(9600); // Init the serial port
 Wire.begin();
 sensor.begin(0x50);
 sensor.setMode(sensor.eContinuous,sensor.eHigh);
 sensor.start();
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
void loop()
{
 int valLeft = digitalRead(leftlinesensorPin); // read left input value
 int valRight = digitalRead(rightlinesensorPin); // read right input value
 int valFrontLeft = digitalRead(frontLeftPin); // read left input value
 int valFrontRight = digitalRead(frontRightPin); // read right input value
  //Serial.println("Front: " << valFrontLeft << "  Back: " << valFrontRight << "  Right: " << valRight << "  Left: " << valLeft);
  int stop = 0;
  int turning = 0;
  int intersection = 0;
  
  // straiaht line code

  if (valFrontLeft && valFrontRight && !valRight && !valLeft)
  {
    // On a line, going straight. All good
    //continue straight
      leftMotor->setSpeed(255);
  leftMotor->run(FORWARD);
  rightMotor->setSpeed(255);
  rightMotor->run(FORWARD);
  Serial.println("straight");

  }
  
  else if (!valFrontLeft  && valFrontRight && !valRight && !valLeft){
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(255);
    rightMotor->run(BACKWARD);
    rightMotor->setSpeed(255);
    Serial.println("Correcting itself");
  }  
  else if (valFrontLeft && !valFrontRight && !valRight && !valLeft){
    leftMotor->run(BACKWARD);
    leftMotor->setSpeed(255);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(255);
    Serial.println("Correcting itself");
  }

// turning 

if(!valFrontLeft && !valFrontRight && valRight)
{
  while(true)
  {

    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin);

    leftMotor->run(FORWARD);
    leftMotor->setSpeed(255);
    rightMotor->run(BACKWARD);
    rightMotor->setSpeed(255);
    Serial.println("turning");

      if(valFrontLeft && valFrontRight)
      {
        Serial.println("break");
        break;
      }
      
  } 
   
}


  sensity_t = analogRead(sensityPin);
  dist_t = sensity_t * MAX_RANG / ADC_SOLUTION;
  Serial.println(dist_t, 0); 

  // distance_0 = distance_1;
  // distance_1 = distance_2;
  // distance_2 = dist_t;
  // float average = ( distance_0 + distance_1 + distance_2 )/3;

  // if (average < 50)
  // {
  //   detected = true;
  //   while (true)
  //   {

  //     if (!valFrontLeft && !valFrontRight && valRight && valLeft)
  //     {
  //       break;
  //     }

  //     rightMotor->run(BACKWARD);
  //     rightMotor->setSpeed(100);
  //     leftMotor->run(FORWARD);
  //     leftMotor->setSpeed(100);

  //   }

  // }
  // //
  // lidar = sensor.getDistance()/10 // we want in cm
  // if (detected && lidar < 50){


  // }
  delay(100);
}
