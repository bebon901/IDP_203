#include <Adafruit_MotorShield.h>
#include "Arduino.h"
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);
#define MAX_RANG (520)//the max measurement value of the module is 520cm(a little bit longer
#include "Wire.h"
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


int frontLeftPin = 4;
int frontRightPin = 1; // Connect sensor to input pin 3
int leftlinesensorPin = 2;
int rightlinesensorPin = 3; // Connect sensor to input pin 3
int scanning = 0;
int sensityPin = A0; 

void setup() {
 Serial.begin(9600); // Init the serial port
 Wire.begin();
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

void StraightLine(int valFrontLeft, int valFrontRight, int valRight, int valLeft){

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

}
void go_to_box(int magnetic)
{
  while(true){
 int valLeft = digitalRead(leftlinesensorPin); // read left input value
 int valRight = digitalRead(rightlinesensorPin); // read right input value
 int valFrontLeft = digitalRead(frontLeftPin); // read left input value
 int valFrontRight = digitalRead(frontRightPin); // read right input value
  //Serial.println("Front: " << valFrontLeft << "  Back: " << valFrontRight << "  Right: " << valRight << "  Left: " << valLeft);
  // Start by continuing along the line

  StraightLine(valFrontLeft, valFrontRight, valRight , valLeft);

  // We now reach the intersection, we either go right or left:

if(!valFrontLeft && !valFrontRight && valRight && valLeft)
      {
        if (magnetic)
        {
          // Turn right and head to the red box
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
          // Now facing the correct direction.
          // Follow this line for a while, until it finishes
          while(true)
          {
            // Following lines...
            valLeft = digitalRead(leftlinesensorPin); // read left input value
            valRight = digitalRead(rightlinesensorPin); // read right input value
            valFrontLeft = digitalRead(frontLeftPin); // read left input value
            valFrontRight = digitalRead(frontRightPin);

            StraightLine(valFrontLeft, valFrontRight, valRight , valLeft);
            
            if(!valFrontLeft && !valFrontRight)
            {
              // Guys... We've just left the line...
              // Make a small turn to the left, point towards the box more
              leftMotor->run(BACKWARD);
              leftMotor->setSpeed(70);
              rightMotor->run(FORWARD);
              rightMotor->setSpeed(70);
              delay(200);
              leftMotor->run(FORWARD);
              leftMotor->setSpeed(255);
              rightMotor->run(FORWARD);
              rightMotor->setSpeed(255);
              while(true)
              {
                // Wait for the front line sensors to detect again
                      valLeft = digitalRead(leftlinesensorPin); // read left input value
                valRight = digitalRead(rightlinesensorPin); // read right input value
                valFrontLeft = digitalRead(frontLeftPin); // read left input value
                valFrontRight = digitalRead(frontRightPin);

                if(valFrontLeft && valFrontRight)
                {
                  Serial.println("break");
                  break;
                }
              }

              // We're now in the box
              leftMotor->run(FORWARD);
              leftMotor->setSpeed(0);
              rightMotor->run(FORWARD);
              rightMotor->setSpeed(0);
              delay(10000);
              // Done!
            }
          }
        }
        if (!magnetic)
        {
          // Turn left and head to the green box
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
            Serial.println("turning");

              if(valFrontLeft && valFrontRight)
              {
                Serial.println("break");
                break;
              }
              
          }
          // Now facing the correct direction.
          // Follow this line for a while, until it finishes
          while(true)
          {
            // Following lines...
            valLeft = digitalRead(leftlinesensorPin); // read left input value
            valRight = digitalRead(rightlinesensorPin); // read right input value
            valFrontLeft = digitalRead(frontLeftPin); // read left input value
            valFrontRight = digitalRead(frontRightPin);

            StraightLine(valFrontLeft, valFrontRight, valRight , valLeft);
            
            if(!valFrontLeft && !valFrontRight)
            {
              // Guys... We've just left the line...
              // Make a small turn to the right, point towards the box more
              leftMotor->run(FORWARD);
              leftMotor->setSpeed(255);
              rightMotor->run(BACKWARD);
              rightMotor->setSpeed(255);
              delay(500);
              leftMotor->run(FORWARD);
              leftMotor->setSpeed(255);
              rightMotor->run(FORWARD);
              rightMotor->setSpeed(255);
              while(true)
              {
                // Wait for the front line sensors to detect again
                valLeft = digitalRead(leftlinesensorPin); // read left input value
                valRight = digitalRead(rightlinesensorPin); // read right input value
                valFrontLeft = digitalRead(frontLeftPin); // read left input value
                valFrontRight = digitalRead(frontRightPin);

                if(valFrontLeft && valFrontRight)
                {
                  Serial.println("break");
                  break;
                }
              }

              // We're now in the box
              leftMotor->run(FORWARD);
              leftMotor->setSpeed(0);
              rightMotor->run(FORWARD);
              rightMotor->setSpeed(0);
              // Done!
            }
          }
        }
          
          // Now facing the correct direction.
        }
  }
   
}

/*
void box_to_stem(int magnetic)
{
  while(true){
 int valLeft = digitalRead(leftlinesensorPin); // read left input value
 int valRight = digitalRead(rightlinesensorPin); // read right input value
 int valFrontLeft = digitalRead(frontLeftPin); // read left input value
 int valFrontRight = digitalRead(frontRightPin); // read right input value
  //Serial.println("Front: " << valFrontLeft << "  Back: " << valFrontRight << "  Right: " << valRight << "  Left: " << valLeft);
  // Start by continuing along the line

  StraightLine(valFrontLeft, valFrontRight, valRight , valLeft);

  // We now reach the intersection, we either go right or left:

if(!valFrontLeft && !valFrontRight && valRight && valLeft)
      {
        if (magnetic)
        {
          // Turn right and head to the red box
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
          // Now facing the correct direction.
          // Follow this line for a while, until it finishes
          while(true)
          {
            // Following lines...
            valLeft = digitalRead(leftlinesensorPin); // read left input value
            valRight = digitalRead(rightlinesensorPin); // read right input value
            valFrontLeft = digitalRead(frontLeftPin); // read left input value
            valFrontRight = digitalRead(frontRightPin);

            StraightLine(valFrontLeft, valFrontRight, valRight , valLeft);
            
            if(!valFrontLeft && !valFrontRight)
            {
              // Guys... We've just left the line...
              // Make a small turn to the left, point towards the box more
              leftMotor->run(BACKWARD);
              leftMotor->setSpeed(70);
              rightMotor->run(FORWARD);
              rightMotor->setSpeed(70);
              delay(200);
              leftMotor->run(FORWARD);
              leftMotor->setSpeed(255);
              rightMotor->run(FORWARD);
              rightMotor->setSpeed(255);
              while(true)
              {
                // Wait for the front line sensors to detect again
                      valLeft = digitalRead(leftlinesensorPin); // read left input value
                valRight = digitalRead(rightlinesensorPin); // read right input value
                valFrontLeft = digitalRead(frontLeftPin); // read left input value
                valFrontRight = digitalRead(frontRightPin);

                if(valFrontLeft && valFrontRight)
                {
                  Serial.println("break");
                  break;
                }
              }

              // We're now in the box
              leftMotor->run(FORWARD);
              leftMotor->setSpeed(0);
              rightMotor->run(FORWARD);
              rightMotor->setSpeed(0);
              delay(10000);
              // Done!
            }
          }
        }
        
   
}
*/

void after_drop_return(int magnetic)
{
  while(true){
 int valLeft = digitalRead(leftlinesensorPin); // read left input value
 int valRight = digitalRead(rightlinesensorPin); // read right input value
 int valFrontLeft = digitalRead(frontLeftPin); // read left input value
 int valFrontRight = digitalRead(frontRightPin); // read right input value

 if (magnetic)
  {
          // Turn right and head to the red box
          while(true)
          {
            valLeft = digitalRead(leftlinesensorPin); // read left input value
            valRight = digitalRead(rightlinesensorPin); // read right input value
            valFrontLeft = digitalRead(frontLeftPin); // read left input value
            valFrontRight = digitalRead(frontRightPin);

            leftMotor->run(BACKWARD);
            leftMotor->setSpeed(150);
            rightMotor->run(BACKWARD);
            rightMotor->setSpeed(140);

            if(valLeft or valRight){
              break;
            }
          }

          while(true)
          {
            valLeft = digitalRead(leftlinesensorPin); // read left input value
            valRight = digitalRead(rightlinesensorPin); // read right input value
            valFrontLeft = digitalRead(frontLeftPin); // read left input value
            valFrontRight = digitalRead(frontRightPin);

            while(valLeft && !valRight)
            {
              leftMotor->run(FORWARD);
              leftMotor->setSpeed(70);
              rightMotor->run(BACKWARD);
              rightMotor->setSpeed(70);

              valLeft = digitalRead(leftlinesensorPin); // read left input value
              valRight = digitalRead(rightlinesensorPin); // read right input value
              valFrontLeft = digitalRead(frontLeftPin); // read left input value
              valFrontRight = digitalRead(frontRightPin);
            }

            while(!valLeft && valRight)
            {
              leftMotor->run(BACKWARD);
              leftMotor->setSpeed(70);
              rightMotor->run(FORWARD);
              rightMotor->setSpeed(70);

              valLeft = digitalRead(leftlinesensorPin); // read left input value
              valRight = digitalRead(rightlinesensorPin); // read right input value
              valFrontLeft = digitalRead(frontLeftPin); // read left input value
              valFrontRight = digitalRead(frontRightPin);
            }

            if(valLeft && valRight)
            {
              Serial.println('now align');
              leftMotor->run(BACKWARD);
              leftMotor->setSpeed(0);
              rightMotor->run(BACKWARD);
              rightMotor->setSpeed(0);
              break;
            }
          }

          float time_reverse_0 = millis();
          while((millis() - time_reverse_0) < 500)
          {
          leftMotor->run(BACKWARD);
          leftMotor->setSpeed(255);
          rightMotor->run(BACKWARD);
          rightMotor->setSpeed(255);

          valLeft = digitalRead(leftlinesensorPin); // read left input value
          valRight = digitalRead(rightlinesensorPin); // read right input value
          valFrontLeft = digitalRead(frontLeftPin); // read left input value
          valFrontRight = digitalRead(frontRightPin);
          }  

          float time_reverse_1 = millis();
          while(((millis() - time_start_reverse_1) < 500) || !(valLeft) or !(valRight))
          {
          leftMotor->run(BACKWARD);
          leftMotor->setSpeed(255);
          rightMotor->run(BACKWARD);
          rightMotor->setSpeed(255);

          valLeft = digitalRead(leftlinesensorPin); // read left input value
          valRight = digitalRead(rightlinesensorPin); // read right input value
          valFrontLeft = digitalRead(frontLeftPin); // read left input value
          valFrontRight = digitalRead(frontRightPin);
          }  

          if(valLeft or valRight)
          {
            float time_reverse_2 = millis();
            while(((millis() - time_start_reverse_2) < 500))
            {
              leftMotor->run(BACKWARD);
              leftMotor->setSpeed(255);
              rightMotor->run(BACKWARD);
              rightMotor->setSpeed(100);
            }
          }
  }

  if (!magnetic)
  {
          // Turn right and head to the red box
          while(true)
          {
            valLeft = digitalRead(leftlinesensorPin); // read left input value
            valRight = digitalRead(rightlinesensorPin); // read right input value
            valFrontLeft = digitalRead(frontLeftPin); // read left input value
            valFrontRight = digitalRead(frontRightPin);

            leftMotor->run(BACKWARD);
            leftMotor->setSpeed(150);
            rightMotor->run(BACKWARD);
            rightMotor->setSpeed(140);

            if(valLeft or valRight){
              break;
            }
          }

          while(true)
          {
            valLeft = digitalRead(leftlinesensorPin); // read left input value
            valRight = digitalRead(rightlinesensorPin); // read right input value
            valFrontLeft = digitalRead(frontLeftPin); // read left input value
            valFrontRight = digitalRead(frontRightPin);

            while(valLeft && !valRight)
            {
              leftMotor->run(FORWARD);
              leftMotor->setSpeed(70);
              rightMotor->run(BACKWARD);
              rightMotor->setSpeed(70);

              valLeft = digitalRead(leftlinesensorPin); // read left input value
              valRight = digitalRead(rightlinesensorPin); // read right input value
              valFrontLeft = digitalRead(frontLeftPin); // read left input value
              valFrontRight = digitalRead(frontRightPin);
            }

            while(!valLeft && valRight)
            {
              leftMotor->run(BACKWARD);
              leftMotor->setSpeed(70);
              rightMotor->run(FORWARD);
              rightMotor->setSpeed(70);

              valLeft = digitalRead(leftlinesensorPin); // read left input value
              valRight = digitalRead(rightlinesensorPin); // read right input value
              valFrontLeft = digitalRead(frontLeftPin); // read left input value
              valFrontRight = digitalRead(frontRightPin);
            }

            if(valLeft && valRight)
            {
              Serial.println('now align');
              leftMotor->run(BACKWARD);
              leftMotor->setSpeed(0);
              rightMotor->run(BACKWARD);
              rightMotor->setSpeed(0);
              break;
            }
          }

          float time_reverse_4 = millis();
          while((millis() - time_reverse_4) < 500)
          {
          leftMotor->run(BACKWARD);
          leftMotor->setSpeed(255);
          rightMotor->run(BACKWARD);
          rightMotor->setSpeed(255);

          valLeft = digitalRead(leftlinesensorPin); // read left input value
          valRight = digitalRead(rightlinesensorPin); // read right input value
          valFrontLeft = digitalRead(frontLeftPin); // read left input value
          valFrontRight = digitalRead(frontRightPin);
          }  

          float time_reverse_5 = millis();
          while(((millis() - time_start_reverse_5) < 500) || !(valLeft) or !(valRight))
          {
          leftMotor->run(BACKWARD);
          leftMotor->setSpeed(255);
          rightMotor->run(BACKWARD);
          rightMotor->setSpeed(255);

          valLeft = digitalRead(leftlinesensorPin); // read left input value
          valRight = digitalRead(rightlinesensorPin); // read right input value
          valFrontLeft = digitalRead(frontLeftPin); // read left input value
          valFrontRight = digitalRead(frontRightPin);
          }  

          if(valLeft or valRight)
          {
            float time_reverse_6 = millis();
            while(((millis() - time_start_reverse_6) < 500))
            {
              leftMotor->run(BACKWARD);
              leftMotor->setSpeed(100);
              rightMotor->run(BACKWARD);
              rightMotor->setSpeed(255);
            }
          }
  }
}


void loop()
{
  
 int valLeft = digitalRead(leftlinesensorPin); // read left input value
 int valRight = digitalRead(rightlinesensorPin); // read right input value
 int valFrontLeft = digitalRead(frontLeftPin); // read left input value
 int valFrontRight = digitalRead(frontRightPin); // read right input value
  StraightLine(valFrontLeft, valFrontRight, valRight , valLeft);
  after_drop_return(1);
  delay(10000);
}
