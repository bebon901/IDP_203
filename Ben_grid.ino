#include <Adafruit_MotorShield.h>
#include "Arduino.h"
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);
#define MAX_RANG (510)//the max measurement value of the module is 510cm(a little bit longer
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
DFRobot_VL53L0X sensor;

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *leftMotor = AFMS.getMotor(3);
Adafruit_DCMotor *rightMotor = AFMS.getMotor(4);

int green_ledPin = 13;
int red_ledPin = 12;
int frontLeftPin = 2;
int frontRightPin = 3; // Connect sensor to input pin 3
int leftlinesensorPin = 5;
int rightlinesensorPin = 4; // Connect sensor to input pin 3
int scanning = 0;
int sensityPin = A0; 

void setup() {
 Serial.begin(9600); // Init the serial port
 Wire.begin();
 pinMode(red_ledPin, OUTPUT);
pinMode(green_ledPin, OUTPUT);
 pinMode(leftlinesensorPin, INPUT); // declare LED as output
 pinMode(rightlinesensorPin, INPUT); // declare Micro switch as input
 pinMode(frontRightPin, INPUT); // declare LED as output
 pinMode(frontLeftPin, INPUT); // declare Micro switch as input
 Wire.begin();
 //Set I2C sub-device address
 sensor.begin(0x50);
 //Set to Back-to-back mode and high precision mode
 sensor.setMode(sensor.eContinuous,sensor.eHigh);
 //Laser rangefinder begins to work
 sensor.start();

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

void StraightLine(int valFrontLeft, int valFrontRight, int valRight, int valLeft){

  if (valFrontLeft && valFrontRight && !valRight && !valLeft)
  {
    // On a line, going straight. All good
    //continue straight
      leftMotor->setSpeed(255);
  leftMotor->run(FORWARD);
  rightMotor->setSpeed(255);
  rightMotor->run(FORWARD);

  }
  
  else if (!valFrontLeft  && valFrontRight && !valRight && !valLeft){
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(255);
    rightMotor->run(BACKWARD);
    rightMotor->setSpeed(255);
    //Serial.println("Correcting itself");
  }  
  else if (valFrontLeft && !valFrontRight && !valRight && !valLeft){
    leftMotor->run(BACKWARD);
    leftMotor->setSpeed(255);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(255);
    //Serial.println("Correcting itself");
  }

}

void turn_right()
{
  
  int valLeft = digitalRead(leftlinesensorPin); // read left input value
  int valRight = digitalRead(rightlinesensorPin); // read right input value
  int valFrontLeft = digitalRead(frontLeftPin); // read left input value
  int valFrontRight = digitalRead(frontRightPin); // read right input value
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
    //Serial.println("turning");

      if(valFrontLeft && valFrontRight)
      {
        //Serial.println("break");
            leftMotor->run(FORWARD);
          leftMotor->setSpeed(255);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(255);
          delay(250);
        break;
      }
      
  }
}
void turn_left()
{
  
  int valLeft = digitalRead(leftlinesensorPin); // read left input value
  int valRight = digitalRead(rightlinesensorPin); // read right input value
  int valFrontLeft = digitalRead(frontLeftPin); // read left input value
  int valFrontRight = digitalRead(frontRightPin); // read right input value
  while(true)
  {
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin);

    leftMotor->run(BACKWARD);
    leftMotor->setSpeed(255);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(255);
    //Serial.println("turning");

      if(valFrontLeft && valFrontRight)
      {
        //Serial.println("break");
            leftMotor->run(FORWARD);
          leftMotor->setSpeed(255);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(255);
        break;
      }
      
  }
}

void turn_right_at_intersection()
{
  
  int valLeft = digitalRead(leftlinesensorPin); // read left input value
  int valRight = digitalRead(rightlinesensorPin); // read right input value
  int valFrontLeft = digitalRead(frontLeftPin); // read left input value
  int valFrontRight = digitalRead(frontRightPin); // read right input value
  while(true)
  {
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin);

    leftMotor->run(FORWARD);
    leftMotor->setSpeed(100);
    rightMotor->run(BACKWARD);
    rightMotor->setSpeed(100);
    //Serial.println("turning");

      if(!valRight)
      {
        Serial.println("first_condition");
        delay(200);
        break;
      }
  }
  Serial.println("Got here");
  while(true)
  {
      valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin);
    if(valRight)
    {
      Serial.println("Stopping!");
      break;
    }
  }

  while(true)
  {
      valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin);
    if(valFrontLeft && valFrontRight)
    {
      Serial.println("Stopping!");
            leftMotor->run(FORWARD);
          leftMotor->setSpeed(100);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(100);
          delay(100);
      break;
    }
  }
}

void turn_180()
{

}

void turn_left_at_intersection()
{
  
  int valLeft = digitalRead(leftlinesensorPin); // read left input value
  int valRight = digitalRead(rightlinesensorPin); // read right input value
  int valFrontLeft = digitalRead(frontLeftPin); // read left input value
  int valFrontRight = digitalRead(frontRightPin); // read right input value
  while(true)
  {
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin);

    leftMotor->run(BACKWARD);
    leftMotor->setSpeed(100);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(100);
    //Serial.println("turning");

      if(!valLeft)
      {
        Serial.println("break");
        break;
      }
  }
  while(true)
  {
    if(valFrontLeft && valFrontRight)
    {
      Serial.println("break");
            leftMotor->run(FORWARD);
          leftMotor->setSpeed(100);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(100);
      break;
    }
  }
}

void scan_using_dist()
{
  float dist_t, sensity_t;
  // read the value from the sensor:
  sensity_t = analogRead(sensityPin);
  // turn the ledPin on
  dist_t = sensity_t * MAX_RANG / ADC_SOLUTION;//
  Serial.println(dist_t);
  if (dist_t < 40)
  {
    digitalWrite(red_ledPin, HIGH);
    digitalWrite(green_ledPin, LOW);
  }
  else
  {
    digitalWrite(red_ledPin, LOW);
    digitalWrite(green_ledPin, HIGH);
  }
}
void traverse_grid()
{
  int position = 2;
    int valLeft = digitalRead(leftlinesensorPin); // read left input value
    int valRight = digitalRead(rightlinesensorPin); // read right input value
    int valFrontLeft = digitalRead(frontLeftPin); // read left input value
    int valFrontRight = digitalRead(frontRightPin); // read right input value
    // This assumes we start from the middle intersection, facing to the left.
    while(true)
    {
      while(true)
      {
        valLeft = digitalRead(leftlinesensorPin); // read left input value
        valRight = digitalRead(rightlinesensorPin); // read right input value
        valFrontLeft = digitalRead(frontLeftPin); // read left input value
        valFrontRight = digitalRead(frontRightPin); // read right input value
        int x = check_for_block();
        // Reached first right
        if(valRight)
        {
          Serial.println("Saw a right!!");
          Serial.println(position);
          position += 1;

          if (position == 2)
          {
            turn_right();
            for (int i=0; i<10; i++)
            {
              valLeft = digitalRead(leftlinesensorPin); // read left input value
              valRight = digitalRead(rightlinesensorPin); // read right input value
              valFrontLeft = digitalRead(frontLeftPin); // read left input value
              valFrontRight = digitalRead(frontRightPin); // read right input value
              StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
              delay(20);
            }
          }
          
          else if (position == 3)
          {
            Serial.println("HERE");
            turn_right_at_intersection();
            for (int i=0; i<100; i++)
            {
              
            Serial.println("Straight_liene");
              valLeft = digitalRead(leftlinesensorPin); // read left input value
              valRight = digitalRead(rightlinesensorPin); // read right input value
              valFrontLeft = digitalRead(frontLeftPin); // read left input value
              valFrontRight = digitalRead(frontRightPin); // read right input value
              StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
              delay(20);
            }
          }
          else if (position == 7)
          {
            turn_right();
            for (int i=0; i<10; i++)
            {
              valLeft = digitalRead(leftlinesensorPin); // read left input value
              valRight = digitalRead(rightlinesensorPin); // read right input value
              valFrontLeft = digitalRead(frontLeftPin); // read left input value
              valFrontRight = digitalRead(frontRightPin); // read right input value
              StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
              delay(100);
            }
          }
          else if (position == 8)
          {
            turn_right_at_intersection();
            for (int i=0; i<10; i++)
            {
              valLeft = digitalRead(leftlinesensorPin); // read left input value
              valRight = digitalRead(rightlinesensorPin); // read right input value
              valFrontLeft = digitalRead(frontLeftPin); // read left input value
              valFrontRight = digitalRead(frontRightPin); // read right input value
              StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
              delay(100);
            }
          }
          else
          {
          delay(500);
          }
          break;
        }
        StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
      }
      if (position == 10)
      {
        break;
      }
    }
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(0);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(0);
    delay(1000);
}

void drive_till_intersection()
{    int valLeft = digitalRead(leftlinesensorPin); // read left input value
    int valRight = digitalRead(rightlinesensorPin); // read right input value
    int valFrontLeft = digitalRead(frontLeftPin); // read left input value
    int valFrontRight = digitalRead(frontRightPin); // read right input value

  while(true)
  {
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin); // read right input value
    StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
    int x = check_for_block();
    if (valLeft or valRight)
    {
      break;
    }
  }
}


void pass_over_intersection()
{   
  int valLeft = digitalRead(leftlinesensorPin); // read left input value
  int valRight = digitalRead(rightlinesensorPin); // read right input value
  int valFrontLeft = digitalRead(frontLeftPin); // read left input value
  int valFrontRight = digitalRead(frontRightPin); // read right input value

  while(true)
  {
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin); // read right input value
    StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
    int x = check_for_block();
    if (!valLeft && !valRight)
    {
      break;
    }
  }
}
int check_for_block()
{
  int x = sensor.getDistance();
  if (x < 50)
  {
    digitalWrite(red_ledPin, HIGH);
    digitalWrite(green_ledPin, LOW);
    return 1;
  }
  else
  {
    digitalWrite(red_ledPin, LOW);
    digitalWrite(green_ledPin, HIGH);
    return 0;
  }
}
void get_home(int position)
{
  if (position != 0)
  {
    if (position == 1)
    {
      // Drive forwards
      drive_till_intersection();
      pass_over_intersection();
    }
    if (position == 2)
    {
      // Drive forwards
      drive_till_intersection();
      pass_over_intersection();
    }
    if (position == 3)
    {
      // Drive forwards, turn left
      drive_till_intersection();
      turn_left_at_intersection();
    }
    if (position == 4)
    {
      // Drive forwards, turn left
      drive_till_intersection();
      turn_left();
    }
    if (position == 5)
    {
      // Drive forwards
      drive_till_intersection();
      pass_over_intersection();
    }
    if (position == 6)
    {
      // Drive forwards
      drive_till_intersection();
      pass_over_intersection();
    }
    if (position == 7)
    {
      // Drive forwards
      drive_till_intersection();
      pass_over_intersection();
    }
    if (position == 8)
    {
      // Drive forwards, turn left
      drive_till_intersection();
      turn_left();
    }
    if (position == 9)
    {
      // Drive forwd, turn left
      drive_till_intersection();
      turn_left_at_intersection();
    }
    position = position - 1;
    get_home(position);
  }
}
void loop()
{
traverse_grid();
}
