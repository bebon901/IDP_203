#include <Adafruit_MotorShield.h>
#include "Arduino.h"
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);
#define MAX_RANG (520)//the max measurement value of the module is 520cm(a little bit longer
#include <Wire.h>
#include <VL53L0X.h>
VL53L0X sensor;
#define ADC_SOLUTION (1023.0)//ADC accuracy of Arduino UNO is 10bit
#define HIGH_ACCURACY
#define LONG_RANGE
// select the input pin

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *leftMotor = AFMS.getMotor(4);
Adafruit_DCMotor *rightMotor = AFMS.getMotor(3);

int frontLeftPin = 2;
int frontRightPin = 3; // Connect sensor to input pin 3
int leftlinesensorPin = 5;
int rightlinesensorPin = 4; // Connect sensor to input pin 3
int scanning = 0;
int sensityPin = A0; 
int step = 0;
void setup() 
{
 Serial.begin(9600); // Init the serial port
 Wire.begin();
 pinMode(leftlinesensorPin, INPUT); // declare LED as output
 pinMode(rightlinesensorPin, INPUT); // declare Micro switch as input
 pinMode(frontRightPin, INPUT); // declare LED as output
 pinMode(frontLeftPin, INPUT); // declare Micro switch as input

 if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
  // if (!AFMS.begin(1500)) {  // OR with a different frequency, say 1KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  Serial.println("Motor Shield found.");

  // Set the speed to start, from 0 (off) to 150 (max speed)

  // turn on motor
  leftMotor->run(RELEASE);
  rightMotor->run(RELEASE);

  
  /* lidar 
  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }

#if defined LONG_RANGE
  // lower the return signal rate limit (default is 0.25 MCPS)
  sensor.setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
#endif

#if defined HIGH_SPEED
  // reduce timing budget to 20 ms (default is about 33 ms)
  sensor.setMeasurementTimingBudget(15000);

#elif defined HIGH_ACCURACY
  // increase timing budget to 150 ms
  sensor.setMeasurementTimingBudget(150000);
#endif
*/
}

// float tof_distance(){
//   float distance = (sensor.readRangeSingleMillimeters());
//   return distance;
// }

// void bigrectangle()
// {
//   int curr_dir[4] = {0,0,0,0};
//   if (step == 0)
//   {
//     StraightLine 
//   }

// }

void StraightLine(){

  int read_dir[4];
  read(read_dir);
  if (read_dir[0] && read_dir[1] && !read_dir[2] && !read_dir[3])
  {
    // On a line, going straight. All good
    //continue straight
  leftMotor->setSpeed(150);
  leftMotor->run(FORWARD);
  rightMotor->setSpeed(140);
  rightMotor->run(FORWARD);
  //Serial.println("straight");

  }
  
  else if (!read_dir[0]  && read_dir[1] && !read_dir[2] && !read_dir[3])
  {
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(150);
    rightMotor->run(BACKWARD);
    rightMotor->setSpeed(150);
    //Serial.println("Correcting itself");
  }  
  else if (read_dir[0] && !read_dir[1] && !read_dir[2] && !read_dir[3])
  {
    leftMotor->run(BACKWARD);
    leftMotor->setSpeed(150);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(150);
    //Serial.println("Correcting itself");
  }


}

void read(int read_array[])
{
    //
    read_array[2] = digitalRead(leftlinesensorPin); // read left input value
    read_array[3] = digitalRead(rightlinesensorPin); // read right input value
    read_array[0] = digitalRead(frontLeftPin); // read left input value
    read_array[1] = digitalRead(frontRightPin);
    
   //the sequence is read_array = {valFrontLeft, valFrontRight, valLeft, valRight};  


    
}


void loop()
{ 
  small_rectangle();
}


  
void small_rectangle()
{

  int nRight = 0;
  int curr_dir[4] = {0,0,0,0};
  read(curr_dir);
  
  while(true)
  { 
    read(curr_dir);    
    if(curr_dir[3])
    {
      nRight += 1 ;
      Serial.println(nRight);

        if(nRight % 4 == 1)
        {
          Serial.println("in the 1st case");
            while(!(curr_dir[1] && curr_dir[0]))
            {
              
                leftMotor->run(FORWARD);
                leftMotor->setSpeed(200);
                rightMotor->run(BACKWARD);
                rightMotor->setSpeed(200);
                read(curr_dir);

            }
            float time_start1 = millis();
            while(millis() - time_start1 < 2000)
            {
              StraightLine();
            }


        }
          
          

        if(nRight % 4 == 2)
        {
          Serial.println("in the 2ndt case");
          while(!(curr_dir[1] && curr_dir[0]))
          {
              leftMotor->run(FORWARD);
              leftMotor->setSpeed(200);
              rightMotor->run(BACKWARD);
              rightMotor->setSpeed(200);
              read(curr_dir);
          }
          float time_start2 = millis();
          while(millis() - time_start2 < 2000)
          {
            StraightLine();
          }
          

        }
        

        if (nRight % 4 == 3)
        { 
          
          Serial.println("in the 3rdt case");
            while(!(curr_dir[1] && curr_dir[0]))
            {
                leftMotor->run(FORWARD);
                leftMotor->setSpeed(200);
                rightMotor->run(BACKWARD);
                rightMotor->setSpeed(200);
                read(curr_dir);
                
            }
           
            float time_start3 = millis();
            while(millis() - time_start3 < 2000)
            {
                StraightLine();
              
            }
        }
          
        

        

        if (nRight % 4 == 0)
        {
          
          Serial.println("in the 4th case");
          while(!(curr_dir[2] && curr_dir[3]))
          {
              leftMotor->run(FORWARD);
              leftMotor->setSpeed(200);
              rightMotor->run(BACKWARD);
              rightMotor->setSpeed(200);
              read(curr_dir);

          }
          
          float time_start4 = millis();
          while(millis() - time_start4 < 1000)
          {
            leftMotor->run(FORWARD);
            leftMotor->setSpeed(200);
            rightMotor->run(FORWARD);
            rightMotor->setSpeed(180);
          }
          

        }
    }
    else
    {
      StraightLine();
      Serial.println("Going in a straight line");
    }


  }

}

