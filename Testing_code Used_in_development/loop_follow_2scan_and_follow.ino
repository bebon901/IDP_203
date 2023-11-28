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



float ultrasonic_distance(){
float dist_t, sensity_t;
 // read the value from the sensor:
sensity_t = analogRead(sensityPin);
 // turn the ledPin on
dist_t = sensity_t * MAX_RANG / ADC_SOLUTION;//
//Serial.print(dist_t,0);
return (dist_t * 10);
//Serial.println("cm");dis

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
  //Serial.println("straight");

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

void read(bool read_array[])
{
    //
    read_array[2] = digitalRead(leftlinesensorPin); // read left input value
    read_array[3] = digitalRead(rightlinesensorPin); // read right input value
    read_array[0] = digitalRead(frontLeftPin); // read left input value
    read_array[1] = digitalRead(frontRightPin);
    
   //the sequence is read_array = {valFrontLeft, valFrontRight, valLeft, valRight};  
    

}

void function(bool array_start[4], bool array_stop[4], bool direction)
{

    // direction is true if clockwise else anticlockwise
    //the sequence is read_array = {valFrontLeft, valFrontRight, valLeft, valRight}; 
  bool read_array [4] = {0,0,0,0};
  read(read_array);

  if ( read_array[0] == array_start[0] && read_array[1] == array_start[1] && read_array[2] == array_start[2] && read_array[3] == array_start[3])
  {
    while(true)
    {
        if(direction)
        {
          leftMotor->run(FORWARD);
          leftMotor->setSpeed(255);
          rightMotor->run(BACKWARD);
          rightMotor->setSpeed(255);
          //Serial.println("turning");
        }
        else
        {
          leftMotor->run(BACKWARD);
          leftMotor->setSpeed(255);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(255);
          //Serial.println("turning");
        }
        // add a delay if required 
        delay(10);
        read(read_array);

        if(read_array[0] == array_stop[0] && read_array[1] == array_stop[1] && read_array[2] == array_stop[2] && read_array[3] == array_stop[3])
        {
          Serial.println("break");
          break;
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
  // Simple Right Turn
  if (valRight && !valFrontRight && valFrontLeft && !valLeft){
      bool start [4] = {0,0,0,1};
      bool stop [4] = {1,1,0,1};
      bool direction = true;
      Serial.println("TURNING!!!");
      function(start, stop, direction);
  }
    // Right Turn at Intersection
  if (valRight && valFrontRight && valFrontLeft && !valLeft){
      bool start [4] = {1,1,0,1};
      bool stop [4] = {1,1,1,1};
      bool direction  = true;
      function(start, stop, direction);
  }
  else {
    StraightLine(valFrontLeft, valFrontRight, valRight , valLeft);
  }
  float dist = ultrasonic_distance();
  //Serial.println(dist);
}
