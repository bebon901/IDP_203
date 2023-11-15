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
int detected = false;
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

  // Set the speed to start, from 0 (off) to 100 (max speed)

  // turn on motor
  leftMotor->run(RELEASE);
  rightMotor->run(RELEASE);
}

void StraightLine(){

  int read_dir[4];
  read(read_dir);
  if (read_dir[0] && read_dir[1] && !read_dir[2] && !read_dir[3])
  {
    // On a line, going straight. All good
    //continue straight
  leftMotor->setSpeed(100);
  leftMotor->run(FORWARD);
  rightMotor->setSpeed(100);
  rightMotor->run(FORWARD);
  Serial.println("straight");

  }
  
  else if (!read_dir[0]  && read_dir[1] && !read_dir[2] && !read_dir[3])
  {
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(100);
    rightMotor->run(BACKWARD);
    rightMotor->setSpeed(100);
    Serial.println("Correcting itself");
  }  
  else if (read_dir[0] && !read_dir[1] && !read_dir[2] && !read_dir[3])
  {
    leftMotor->run(BACKWARD);
    leftMotor->setSpeed(100);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(100);
    Serial.println("Correcting itself");
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

void function(int array_start[4], int array_stop[4], int direction)
{

    // direction is true if clockwise else anticlockwise
    //the sequence is read_array = {valFrontLeft, valFrontRight, valLeft, valRight}; 
  int read_array[4] = {0,0,0,0};
  read(read_array);
  Serial.print(read_array[0]);
  Serial.print(read_array[1]);
  Serial.print(read_array[2]);
  Serial.println(read_array[3]);

  if ( read_array[0] == array_start[0] && read_array[1] == array_start[1]  && read_array[2] == array_start[2] && read_array[3] == array_start[3])
  {
   


 
    while(true)
    {
        Serial.println("Inside loop");
        if(read_array[0] == array_stop[0] && read_array[1] == array_stop[1] && read_array[2] == array_stop[2] && read_array[3] == array_stop[3])
        {
          Serial.println("break");
          break;
        }

        if(direction)
        {
          leftMotor->run(FORWARD);
          leftMotor->setSpeed(100);
          rightMotor->run(BACKWARD);
          rightMotor->setSpeed(100);
          Serial.println("turning");
        }
        else
        {
          leftMotor->run(BACKWARD);
          leftMotor->setSpeed(100);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(100);
          Serial.println("turning");
        }
        // add a delay if required 

        read(read_array);
        Serial.print(read_array[0]);
        Serial.print(read_array[1]);
        Serial.print(read_array[2]);
        Serial.println(read_array[3]);
     
       



    }

  }
  else
  {
    Serial.println("Assuming it goes straight");
    StraightLine();
  }

}



void loop()
{

  





int start[4] = {0,0,1,1};
int stop[4] = {1,1,0,1};
int direction = 1;

function(start, stop, direction);

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


    // hardcoding the path 
  // int step = 0;
  // int x = 0;
  // int y = 0;
  // int lidar = 0;
  // int nRight = 0;
  // int dir[4];

  // while(True)
  // {
    
  //   read(dir);
  //   if(step == 0  && dir[0] && dir[1] && dir[2] && dir [3]) // intersection 0 
  //   {
  //     int start = {1,1,1,1};
  //     int stop = {1,1,1,1};
  //     int direction  = false;
  //     function(start, stop, direction); 
  //     step = 1; // after entering the loop step becomes 1
  //   }
  //   else if (step == 1 && !dir[0] && !dir[1] && dir[2] && dir[3] ) // intersection 2 and 7
  //   {

  //     if (lidar)
  //     {
  //       int start = {0,0,1,1};
  //       int stop = {1,1,1,1};
  //       direction = true;
  //       function(start, stop, direction);     
  //       step = 2;
  //     }
  //     else
  //     { //right turn 
  //       int start = {0, 0, 1, 1};
  //       int stop = {1,1,0,1};
  //       int direction = true;
  //       nRight += 1; 
  //       function(start, stop, direction);
  //     }
      
  //   }
  //   else if (step == 1 && dir[0] && dir[1] && !dir[2] && dir[3]) // intersections 1, 3 , 4,5,6, 8, 9 
  //   {
  //     // number of right turns is odd i.e means it has just come from intersection 2 and 7 so we take a right turn otherwise not 
      
  //     if (lidar)
  //     {
  //       step = 2;
  //       int start = {1,1,0,1};
  //       int stop  = {1,1,1,0};
  //       int direction = true;
  //       function(start,stop, direction); 
  //     }
  //     else
  //     {       
      
  //       if (nRight % 2 == 1)
  //       {
  //         // we take a right turn
  //         int start = {1,1,0,1};
  //         int stop = {1,1,1,1};
  //         int direction = true;
  //         nRight += 1;
  //         function(start, stop, direction);
  //       }
  //       else
  //       {
  //         StraightLine();
  //       }
      
  //     }
      
  //   }
  //   else
  //   {
  //     StraightLine()
  //   }

  // }
    
  delay(100);
}
