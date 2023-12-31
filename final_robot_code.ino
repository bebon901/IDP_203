/*
THIS IS THE FINAL ROBOT CODE, CREATED BY MERGING THE FREE SPACE BLOCK AND THE GRID
BLOCK DETECTION CODE.
Merges from ben_grid.ino and v2inworking.ino to create the final peice of software.
These were worked upon separately until the end of the project, where we merged and tested together.
The robot will relatively reliably get both grid blocks and a few free space if lucky. 
However, the code to get from the start box to the colored box is proving unreliable.
*/

// All the libraries
#include <Adafruit_MotorShield.h>
#include "Arduino.h"
#include <Wire.h>
#include "DFRobot_VL53L0X.h"
#include <Servo.h>

#define MAX_RANG (520)//the max measurement value of the module is 520cm(a little bit longer
#define ADC_SOLUTION (1023.0)//ADC accuracy of Arduino UNO is 10bit
#define HIGH_ACCURACY
#define LONG_RANGE

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
DFRobot_VL53L0X sensor;
Servo myservo;

// Define some useful pin numbers
int step = 0;
int servo_pin = 9;
int green_ledPin = 13;
int red_ledPin = 12;
int magneticPin = 6;
int magnetic_block = 0; // you need to change magnetic_block in bens code
int blue_led = 11;


Adafruit_DCMotor *leftMotor = AFMS.getMotor(4);
Adafruit_DCMotor *rightMotor = AFMS.getMotor(3);


int frontLeftPin = 2;
int frontRightPin = 3; // Connect sensor to input pin 3
int leftlinesensorPin = 5;
int rightlinesensorPin = 4; // Connect sensor to input pin 3
int sensityPin = A0; 

int pos = 0;
float distance_0 = 100;
float distance_1 = 100;
float distance_2 = 100;
float lidar = 100;
int scanning  = 0;
int straight_speed = 200;
int left_turn_speed = 170;
int right_turn_speed = 150;



unsigned long previousMillis = 0;
int BlueLedState = LOW;

// Function to periodically flash the LED, called whenever robot is moving
void led_flash()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 250) 
  { 
    previousMillis = currentMillis;
    if (BlueLedState == LOW) 
    {
      BlueLedState = HIGH;
    } 
    else 
    {
      BlueLedState = LOW;
    }
    digitalWrite(blue_led, BlueLedState);
  }
}

void magnetic_stuff_reset() 
{ 
  //turns green and red LEDs off and turns boolean to false  
  digitalWrite(red_ledPin, LOW);
  digitalWrite(green_ledPin, LOW);
  return false;

}

// Called continuously as we approach a box and sets a global variable to HIGH if any magnetism sensed
void magnetic()
{
  if (digitalRead(magneticPin) == HIGH)
  {
    magnetic_block = 1;
  }
}

// Charu's version of the magentic sensor function.
void magnetic_Charu() 
{ 
  //detects magnetism and turns the red
  //or green LED on accordingly, which then stays on 
  //until turned off with another function
  
  magnetic_block = digitalRead(magneticPin);
  if (magnetic_block == HIGH) digitalWrite(red_ledPin, HIGH);  
  else digitalWrite(green_ledPin, HIGH);  
  
}

void setup()
{

  Serial.begin(9600); // Init the serial port
  Wire.begin();

  //lidar sensor set up 
  sensor.begin(0x50);
  sensor.setMode(sensor.eContinuous,sensor.eHigh);
  sensor.start();

  pinMode(leftlinesensorPin, INPUT); 
  pinMode(rightlinesensorPin, INPUT);
  pinMode(frontRightPin, INPUT); 
  pinMode(frontLeftPin, INPUT); 
  

  // motor shield settings
  if (!AFMS.begin())
  {      
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  Serial.println("Motor Shield found.");
 
  // Attach the servoMotos and magnetic LEDs
  myservo.attach(servo_pin); // attaches the servo on pin 9 to the servo object
  myservo.write(0);        // close it initially
  pinMode(red_ledPin, OUTPUT);
  pinMode(green_ledPin, OUTPUT);
  pinMode(magneticPin, INPUT);
  pinMode(blue_led, OUTPUT);

  // turn on the DC Motors
  leftMotor->run(RELEASE);
  rightMotor->run(RELEASE);

  // Navigate the robot out of the start position
  start_to_grid();


}

// Code that uses the front line sensors to drive in a straight line
void StraightLine(int valFrontLeft, int valFrontRight, int valRight, int valLeft){
  
  magnetic();
  led_flash();
  if (valFrontLeft && valFrontRight && !valRight && !valLeft)
  {
    // On a line, going straight. All good
    //continue straight
      leftMotor->setSpeed(150);
  leftMotor->run(FORWARD);
  rightMotor->setSpeed(140);
  rightMotor->run(FORWARD);
  }

  else if (!valFrontLeft  && valFrontRight && !valRight && !valLeft){
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(150);
    rightMotor->run(BACKWARD);
    rightMotor->setSpeed(150);
    //Serial.println("Correcting itself");
  }  
  else if (valFrontLeft && !valFrontRight && !valRight && !valLeft){
    leftMotor->run(BACKWARD);
    leftMotor->setSpeed(150);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(150);
    //Serial.println("Correcting itself");
  }

}

// Code to perform a right turn
void turn_right()
{
  int valLeft = digitalRead(leftlinesensorPin); // read left input value
  int valRight = digitalRead(rightlinesensorPin); // read right input value
  int valFrontLeft = digitalRead(frontLeftPin); // read left input value
  int valFrontRight = digitalRead(frontRightPin); // read right input value
  while(true)
  {
  led_flash();
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin);

    leftMotor->run(FORWARD);
    leftMotor->setSpeed(left_turn_speed);
    rightMotor->run(BACKWARD);
    rightMotor->setSpeed(right_turn_speed);
    //Serial.println("turning");
      // wait for the front two sensors to redetect, means we are lined up.
      if(valFrontLeft && valFrontRight)
      {
        //Serial.println("break");
            leftMotor->run(FORWARD);
          leftMotor->setSpeed(straight_speed);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(straight_speed);
        break;
      }
  }
  drive_straight_for_a_bit();
}
void turn_left()
{
  
  int valLeft = digitalRead(leftlinesensorPin); // read left input value
  int valRight = digitalRead(rightlinesensorPin); // read right input value
  int valFrontLeft = digitalRead(frontLeftPin); // read left input value
  int valFrontRight = digitalRead(frontRightPin); // read right input value
  while(true)
  {
  led_flash();
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin);

    leftMotor->run(BACKWARD);
    leftMotor->setSpeed(left_turn_speed);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(right_turn_speed);
    //Serial.println("turning");

      if(valFrontLeft && valFrontRight)
      {
        //Serial.println("break");
            leftMotor->run(FORWARD);
          leftMotor->setSpeed(straight_speed);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(straight_speed);
        break;
      }
  }
  drive_straight_for_a_bit();
}

// Similar to above code, but with more cases since the front two sensors start HIGH when an intersection is detected
void turn_right_at_intersection()
{
 leftMotor->run(FORWARD);
  leftMotor->setSpeed(230);
  rightMotor->run(FORWARD);
  rightMotor->setSpeed(200);
  //delay(200);
  leftMotor->run(FORWARD);
  leftMotor->setSpeed(230);
  rightMotor->run(BACKWARD);
  rightMotor->setSpeed(130);
  delay(800);
  int valLeft = digitalRead(leftlinesensorPin); // read left input value
  int valRight = digitalRead(rightlinesensorPin); // read right input value
  int valFrontLeft = digitalRead(frontLeftPin); // read left input value
  int valFrontRight = digitalRead(frontRightPin); // read right input value


  while(true)
  {
    led_flash();
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin);
    if(valFrontLeft && valFrontRight)
      {

        //Serial.println("break");
            leftMotor->run(FORWARD);
          leftMotor->setSpeed(straight_speed);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(straight_speed);
        break;
      }
  }
  drive_straight_for_a_bit();
}


// Function to line follow for a second, very useful throughout the code.
void drive_straight_for_a_bit()
{
  int valLeft = digitalRead(leftlinesensorPin); // read left input value
  int valRight = digitalRead(rightlinesensorPin); // read right input value
  int valFrontLeft = digitalRead(frontLeftPin); // read left input value
  int valFrontRight = digitalRead(frontRightPin); // read right input value
  for (int i=0; i<100; i++)
  {
  led_flash();
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin); // read right input value
    StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
    delay(10);
  }
}

// Read the sensors and return to an array
void read(int read_array[])
{
    read_array[2] = digitalRead(leftlinesensorPin); // read left input value
    read_array[3] = digitalRead(rightlinesensorPin); // read right input value
    read_array[0] = digitalRead(frontLeftPin); // read frontleft input value
    read_array[1] = digitalRead(frontRightPin); // read frontright input value
    
}

// Function to turn right at the center of grid. Actually the same as turn_right_at_intersection() in the end.
void turn_right_at_center()
{
  int valLeft = digitalRead(leftlinesensorPin); // read left input value
  int valRight = digitalRead(rightlinesensorPin); // read right input value
  int valFrontLeft = digitalRead(frontLeftPin); // read left input value
  int valFrontRight = digitalRead(frontRightPin); // read right input value
  int curr_dir [4] = {0, 0, 0, 0};
  while(true)
    { 
      read(curr_dir);
      if(curr_dir[2])
      {
          Serial.println("Sees left");
          float time_turn_inter = millis();
          while((millis() - time_turn_inter < 1000 ) || !(curr_dir[0] && curr_dir[1]))
          {
            Serial.println("turn");
            leftMotor->run(FORWARD);
            leftMotor->setSpeed(200);
            rightMotor->run(BACKWARD);
            rightMotor->setSpeed(200);
            read(curr_dir);
          }
        
          float time_start_reverse = millis();
          while(millis() - time_start_reverse < 100)
          {
            leftMotor->run(FORWARD);
            leftMotor->setSpeed(200);
            rightMotor->run(FORWARD);
            rightMotor->setSpeed(200);
            read(curr_dir);
            if (!curr_dir[0])
            {
              break;
            }
           
            
          }

          drive_straight_for_a_bit();
          // while(true)
          // {
          //   leftMotor->run(BACKWARD);
          //   leftMotor->setSpeed(0);
          //   rightMotor->run(FORWARD);
          //   rightMotor->setSpeed(0);
          // }
          break;
      }
      else
      {
          led_flash();
            valLeft = digitalRead(leftlinesensorPin); // read left input value
            valRight = digitalRead(rightlinesensorPin); // read right input value
            valFrontLeft = digitalRead(frontLeftPin); // read left input value
            valFrontRight = digitalRead(frontRightPin); // read right input value
            StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
      } 
    }
  }

void turn_left_at_center()
{
  led_flash();
  Serial.println("Started");
  int valLeft = digitalRead(leftlinesensorPin); // read left input value
  int valRight = digitalRead(rightlinesensorPin); // read right input value
  int valFrontLeft = digitalRead(frontLeftPin); // read left input value
  int valFrontRight = digitalRead(frontRightPin); // read right input value

  float time_stamp = millis();
  
  while((millis() - time_stamp < 750) )
  {
    led_flash();      
    int valLeft = digitalRead(leftlinesensorPin); // read left input value
    int valRight = digitalRead(rightlinesensorPin); // read right input value
    int valFrontLeft = digitalRead(frontLeftPin); // read left input value
    int valFrontRight = digitalRead(frontRightPin); // read right input value
    leftMotor->run(BACKWARD);
    leftMotor->setSpeed(left_turn_speed);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(right_turn_speed);
  }
  Serial.println(" first");
  valLeft = digitalRead(leftlinesensorPin); // read left input value
  valRight = digitalRead(rightlinesensorPin); // read right input value
  valFrontLeft = digitalRead(frontLeftPin); // read left input value
  valFrontRight = digitalRead(frontRightPin); // read right input value
  while((!valFrontLeft || !valFrontRight))
  {
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin); // read right input value
  }
  time_stamp = millis();
  
  while((millis() - time_stamp < 300) )
  {
    led_flash();      
    int valLeft = digitalRead(leftlinesensorPin); // read left input value
    int valRight = digitalRead(rightlinesensorPin); // read right input value
    int valFrontLeft = digitalRead(frontLeftPin); // read left input value
    int valFrontRight = digitalRead(frontRightPin); // read right input value
    leftMotor->run(BACKWARD);
    leftMotor->setSpeed(left_turn_speed);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(right_turn_speed);
  }

    time_stamp = millis();
  
  while((millis() - time_stamp < 200) )
  {
    led_flash();      
    int valLeft = digitalRead(leftlinesensorPin); // read left input value
    int valRight = digitalRead(rightlinesensorPin); // read right input value
    int valFrontLeft = digitalRead(frontLeftPin); // read left input value
    int valFrontRight = digitalRead(frontRightPin); // read right input value
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(left_turn_speed);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(right_turn_speed);
  }

  Serial.println(" HEREE");
  // while (true)
  // {
  //   leftMotor->run(FORWARD);
  //   leftMotor->setSpeed(0);
  //   rightMotor->run(BACKWARD);
  //   rightMotor->setSpeed(0);
  // }
  Serial.println("Drive Straight");
  drive_straight_for_a_bit();
}

// Charu's version of drive_straight_for_a_bit.
void drive_straight_for_a_bit_2()
{
  float time =  millis();
  while(time - millis() < 2000)
  {
    
    led_flash();      
    int valLeft = digitalRead(leftlinesensorPin); // read left input value
    int valRight = digitalRead(rightlinesensorPin); // read right input value
    int valFrontLeft = digitalRead(frontLeftPin); // read left input value
    int valFrontRight = digitalRead(frontRightPin); // read right input value
    StraightLine(valLeft, valRight, valFrontLeft, valFrontRight);
    
  }

}

// Code to turn 180 degrees on the spot.
// Will turn, align up the side 2 sensors, and then continue turning to attempt to turn on the spot.

void turn_180()
{

  int valLeft = digitalRead(leftlinesensorPin); // read left input value
  int valRight = digitalRead(rightlinesensorPin); // read right input value
  int valFrontLeft = digitalRead(frontLeftPin); // read left input value
  int valFrontRight = digitalRead(frontRightPin); // read right input value
  while(true)
  {
  led_flash();
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin);
    magnetic();
    leftMotor->run(BACKWARD);
    leftMotor->setSpeed(180);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(190);
    //Serial.println("turning");
      if (valRight && !valLeft)
      {
        // Correct the turn
        while (!valLeft)
        {
          valLeft = digitalRead(leftlinesensorPin); // read left input value
          valRight = digitalRead(rightlinesensorPin); // read right input value
          valFrontLeft = digitalRead(frontLeftPin); // read left input value
          valFrontRight = digitalRead(frontRightPin);
          leftMotor->run(BACKWARD);
          leftMotor->setSpeed(180);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(0);
        }
      }
      if (!valRight && valLeft)
      {
        // Correct the turn
        while (!valRight)
        {
          valLeft = digitalRead(leftlinesensorPin); // read left input value
          valRight = digitalRead(rightlinesensorPin); // read right input value
          valFrontLeft = digitalRead(frontLeftPin); // read left input value
          valFrontRight = digitalRead(frontRightPin);
          leftMotor->run(BACKWARD);
          leftMotor->setSpeed(0);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(170);
        }
      }
      if(valRight && valLeft)
      {
          leftMotor->run(BACKWARD);
          leftMotor->setSpeed(180);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(170);
        Serial.println("Half Spun!");
        break;
      }
  }

  while(true)
  {
  led_flash();
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin);

    //Serial.println("turning");

      if(!valFrontLeft && !valFrontRight)
      {
        Serial.println("Over Halfway! Spinning!");
        break;
      }
  } while(true)
  {
  led_flash();
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin);

    //Serial.println("turning");

      if(valFrontLeft && valFrontRight)
      {
        Serial.println("Over Halfway! Spinning!");
        
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(0);
    rightMotor->run(BACKWARD);
    rightMotor->setSpeed(0);
        break;
      }
  }
}
void drive_straight_for_a_short_bit()
{
  int valLeft = digitalRead(leftlinesensorPin); // read left input value
  int valRight = digitalRead(rightlinesensorPin); // read right input value
  int valFrontLeft = digitalRead(frontLeftPin); // read left input value
  int valFrontRight = digitalRead(frontRightPin); // read right input value
  for (int i=0; i<50; i++)
  {
  led_flash();
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin); // read right input value
    StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
    delay(10);
  }
}

void turn_left_at_intersection()
{
leftMotor->run(BACKWARD);
  leftMotor->setSpeed(130);
  rightMotor->run(FORWARD);
  rightMotor->setSpeed(230);
  delay(800);
  int valLeft = digitalRead(leftlinesensorPin); // read left input value
  int valRight = digitalRead(rightlinesensorPin); // read right input value
  int valFrontLeft = digitalRead(frontLeftPin); // read left input value
  int valFrontRight = digitalRead(frontRightPin); // read right input value


  while(true)
  {
    led_flash();
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin);
    if(valFrontLeft && valFrontRight)
      {

        //Serial.println("break");
            leftMotor->run(FORWARD);
          leftMotor->setSpeed(straight_speed);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(straight_speed);
        break;
      }
  }
  drive_straight_for_a_bit();
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
    //digitalWrite(red_ledPin, HIGH);
   // digitalWrite(green_ledPin, LOW);
  }
  else
  {
    //digitalWrite(red_ledPin, LOW);
    //digitalWrite(green_ledPin, HIGH);
  }
}

// Goes from the line out of the start box and navigates through to a colored box, dependent on the global variable, block_magnetic
void go_to_box()
{
  int valLeft = digitalRead(leftlinesensorPin); // read left input value
 int valRight = digitalRead(rightlinesensorPin); // read right input value
 int valFrontLeft = digitalRead(frontLeftPin); // read left input value
 int valFrontRight = digitalRead(frontRightPin); // read right input value
 int deposit_box = 0;
  while(true)
  {
  led_flash();
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
        if (magnetic_block)
        {
          // Turn right and head to the red box
          while(true)
          {
  led_flash();
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
            valLeft = digitalRead(leftlinesensorPin); // read left input value
            valRight = digitalRead(rightlinesensorPin); // read right input value
            valFrontLeft = digitalRead(frontLeftPin); // read left input value
            valFrontRight = digitalRead(frontRightPin);
            StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
            if(!valFrontLeft && valLeft && !valFrontRight)
            {
              break;
            }


          }
          delay(800);
          // Turn to go down the side of the start box
          while(true)
          {
            valLeft = digitalRead(leftlinesensorPin); // read left input value
            valRight = digitalRead(rightlinesensorPin); // read right input value
            valFrontLeft = digitalRead(frontLeftPin); // read left input value
            valFrontRight = digitalRead(frontRightPin);
            leftMotor->run(BACKWARD);
            leftMotor->setSpeed(150);
            rightMotor->run(FORWARD);
            rightMotor->setSpeed(150);
            if(valFrontLeft && valFrontRight)
            {
              break;
            }
          }
          // drive straight for a short while (align up)
          float time_5 = millis();
          while (millis() - time_5 < 150)
          {
            valLeft = digitalRead(leftlinesensorPin); // read left input value
            valRight = digitalRead(rightlinesensorPin); // read right input value
            valFrontLeft = digitalRead(frontLeftPin); // read left input value
            valFrontRight = digitalRead(frontRightPin); // read right input value
            StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
          }
  
          delay(500);
          leftMotor->run(FORWARD);
          leftMotor->setSpeed(150);
          rightMotor->run(BACKWARD);
          rightMotor->setSpeed(150);
          // Code to turn, and align up with the red box
          while(true)
          {
            valLeft = digitalRead(leftlinesensorPin); // read left input value
            valRight = digitalRead(rightlinesensorPin); // read right input value
            valFrontLeft = digitalRead(frontLeftPin); // read left input value
            valFrontRight = digitalRead(frontRightPin);

            Serial.println("Turning");
            if (valRight)
            {
              leftMotor->run(FORWARD);
              leftMotor->setSpeed(250);
              rightMotor->run(BACKWARD);
              rightMotor->setSpeed(50);
              Serial.println("Right");
            }
            if (valLeft)
            {
              leftMotor->run(FORWARD);
              leftMotor->setSpeed(50);
              rightMotor->run(BACKWARD);
              rightMotor->setSpeed(250);
              Serial.println("Left");
            }
            if (valLeft && valRight)
            {
              Serial.println("Stopped!");
              delay(100);
              break;
            }
            delay(50);

          }

          
          // Now drive forward until the front 2 detect again.
          leftMotor->run(FORWARD);
          leftMotor->setSpeed(255);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(220);
          delay(1500);

          while(true)
          {
            led_flash();
            // Wait for the front line sensors to detect again
            valLeft = digitalRead(leftlinesensorPin); // read left input value
            valRight = digitalRead(rightlinesensorPin); // read right input value
            valFrontLeft = digitalRead(frontLeftPin); // read left input value
            valFrontRight = digitalRead(frontRightPin);

            if(valFrontLeft && valFrontRight)
            {
              Serial.println("break");
              deposit_box = 1;
              break;
              
            }
          }

        if(deposit_box == 1)
        {
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(0);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(0);
        myservo.write(90);
        delay(1000);
        break;

        }
        // We're now in the box
    
     
      }
      // Same as block above just with different directions
        if (!magnetic_block)
        {

          // Turn left and head to the green box
          while(true)
          {
  led_flash();
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
                Serial.println("break");
                break;
              }
              
          }
          for (int i=0; i<100; i++)
          {
            valLeft = digitalRead(leftlinesensorPin); // read left input value
            valRight = digitalRead(rightlinesensorPin); // read right input value
            valFrontLeft = digitalRead(frontLeftPin); // read left input value
            valFrontRight = digitalRead(frontRightPin); // read right input value
            StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
            delay(10);
          }  
          leftMotor->run(FORWARD);
          leftMotor->setSpeed(100);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(100);
          while (!valRight)
          {
            leftMotor->run(FORWARD);
            leftMotor->setSpeed(100);
            rightMotor->run(FORWARD);
            rightMotor->setSpeed(100);
            valLeft = digitalRead(leftlinesensorPin); // read left input value
            valRight = digitalRead(rightlinesensorPin); // read right input value
            valFrontLeft = digitalRead(frontLeftPin); // read left input value
            valFrontRight = digitalRead(frontRightPin); // read right input value
          }
 
          // Now facing the correct direction.

          delay(800);
          leftMotor->run(FORWARD);
          leftMotor->setSpeed(150);
          rightMotor->run(BACKWARD);
          rightMotor->setSpeed(150);
          delay(700);
          leftMotor->run(FORWARD);
          leftMotor->setSpeed(255);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(230);

          while(true)
          {
            led_flash();
            // Wait for the front line sensors to detect again
            valLeft = digitalRead(leftlinesensorPin); // read left input value
            valRight = digitalRead(rightlinesensorPin); // read right input value
            valFrontLeft = digitalRead(frontLeftPin); // read left input value
            valFrontRight = digitalRead(frontRightPin);

            if(valFrontLeft && valFrontRight)
            {
              Serial.println("break");
              deposit_box = 1;
              break;
              
            }
          }

        if(deposit_box == 1)
        {
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(0);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(0);
        myservo.write(90);
        delay(1000);
        break;

        }
        // We're now in the box
    
     
      }
    }
  }


// Aligning up!

    leftMotor->run(FORWARD);
    leftMotor->setSpeed(200);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(200);
while (true)
{
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin);
    if (valRight)
    {
      leftMotor->run(FORWARD);
      leftMotor->setSpeed(200);
      rightMotor->run(FORWARD);
      rightMotor->setSpeed(0);
    }
    if (valLeft)
    {
      leftMotor->run(FORWARD);
      leftMotor->setSpeed(0);
      rightMotor->run(FORWARD);
      rightMotor->setSpeed(200);
    }
    if (valLeft && valRight)
    {
      break;
    }
}



    leftMotor->run(BACKWARD);
    leftMotor->setSpeed(200);
    rightMotor->run(BACKWARD);
    rightMotor->setSpeed(155);
    delay(1000);

  // Now, we will return to the start position
  while (true)
  {
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin);
    if (valLeft || valRight)
    {
      break;
    }
  }
  // Align horizontally!

   leftMotor->run(BACKWARD);
    leftMotor->setSpeed(200);
    rightMotor->run(BACKWARD);
    rightMotor->setSpeed(200);
while (true)
{
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin);
    if (valRight)
    {
      leftMotor->run(BACKWARD);
      leftMotor->setSpeed(200);
      rightMotor->run(FORWARD);
      rightMotor->setSpeed(0);
    }
    if (valLeft)
    {
      leftMotor->run(FORWARD);
      leftMotor->setSpeed(0);
      rightMotor->run(BACKWARD);
      rightMotor->setSpeed(200);
    }
    if (valLeft && valRight)
    {
      break;
    }
}

// Reverse backwards for a bit...

      leftMotor->run(BACKWARD);
      leftMotor->setSpeed(230);
      rightMotor->run(BACKWARD);
      rightMotor->setSpeed(200);
      delay(800);
      leftMotor->run(BACKWARD);
      leftMotor->setSpeed(0);
      rightMotor->run(BACKWARD);
      rightMotor->setSpeed(0);
      // Turn on the blue LED
      digitalWrite(blue_led, HIGH);
      delay(5000);
      digitalWrite(blue_led, LOW);
      // Waiting in the box here
// drive forward for a bit
while (true)
{
  
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin);
     leftMotor->run(FORWARD);
    leftMotor->setSpeed(200);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(200);
    if (valLeft && valRight)
    {
      break;
    }
}
  // Now need to align with the forward direction
  while (true)
  {
    if (magnetic_block)
    {
      leftMotor->run(FORWARD);
      leftMotor->setSpeed(200);
      rightMotor->run(BACKWARD);
      rightMotor->setSpeed(200);
      valLeft = digitalRead(leftlinesensorPin); // read left input value
      valRight = digitalRead(rightlinesensorPin); // read right input value
      valFrontLeft = digitalRead(frontLeftPin); // read left input value
      valFrontRight = digitalRead(frontRightPin);
    }
    if (!magnetic_block)
    {
      leftMotor->run(BACKWARD);
      leftMotor->setSpeed(200);
      rightMotor->run(FORWARD);
      rightMotor->setSpeed(200);
      valLeft = digitalRead(leftlinesensorPin); // read left input value
      valRight = digitalRead(rightlinesensorPin); // read right input value
      valFrontLeft = digitalRead(frontLeftPin); // read left input value
      valFrontRight = digitalRead(frontRightPin);
    }
    if (valFrontLeft && valFrontRight)
    {
      break;
    }
  }

  // Now go to the start line. stright, Turn left/right, go straight, turn left/right.

  while (true)
  {
    
      valLeft = digitalRead(leftlinesensorPin); // read left input value
      valRight = digitalRead(rightlinesensorPin); // read right input value
      valFrontLeft = digitalRead(frontLeftPin); // read left input value
      valFrontRight = digitalRead(frontRightPin);
      StraightLine(valFrontLeft, valFrontRight, valRight , valLeft);

      if (valLeft && !valFrontLeft && !valFrontRight)
      {
        turn_left();
      }
      if (valRight && !valFrontLeft && !valFrontRight)
      {
        turn_right();
      }
      if (valLeft && valFrontLeft && valFrontRight)
      {
        turn_left_at_intersection();
        break;
      }
      if (valRight && valFrontLeft && valFrontRight)
      {
        turn_right_at_intersection();
        break;
      }
  }
  drive_straight_for_a_bit();
}

// Code to navigate the grid. Goes round in a loop until a block is detected
void traverse_grid()
{
  int position = 1;
    int valLeft = digitalRead(leftlinesensorPin); // read left input value
    int valRight = digitalRead(rightlinesensorPin); // read right input value
    int valFrontLeft = digitalRead(frontLeftPin); // read left input value
    int valFrontRight = digitalRead(frontRightPin); // read right input value
    int found_block = 0;
    // This assumes we start from the middle intersection, facing to the left.
    while(true)
    {
  led_flash();
      while(true)
      {
  led_flash();
        valLeft = digitalRead(leftlinesensorPin); // read left input value
        valRight = digitalRead(rightlinesensorPin); // read right input value
        valFrontLeft = digitalRead(frontLeftPin); // read left input value
        valFrontRight = digitalRead(frontRightPin); // read right input value
        int x = check_for_block();
        magnetic();
        //Serial.println(x);
        // If found a block:
        if (x)
      {
        // Stop
          leftMotor->run(FORWARD);
          leftMotor->setSpeed(0);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(0);
          Serial.println("Grabbing Block");
          myservo.write(160);
          if (magnetic_block)
          {
            // Turn on the Red LED!
            digitalWrite(red_ledPin, HIGH);
          }
          else
          {
            // Turn on the green LED
            digitalWrite(green_ledPin, HIGH);
          }
          // Wait for 5 seconds
          delay(5000);
          
          digitalWrite(green_ledPin, LOW);
          digitalWrite(red_ledPin, LOW);
          Serial.println(position);
          delay(1000);
          turn_180();
          Serial.println("Drive Straignt");
          drive_straight_for_a_short_bit();
          get_home(position + 1);
          
          leftMotor->run(FORWARD);
          leftMotor->setSpeed(0);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(0);
          found_block = 1;
        // Break exits the code above, and then goes into a return to home fn.
          break;
          // /delay(10000);

        }
        // Reached first right
        // Use cases for each of the positions in the grid.
        if(valRight)
        {
          Serial.println("Saw a right!!");
          Serial.println(position);
          position += 1;

          if (position == 2)
          {
            turn_right();
            for (int i=0; i<50; i++)
            {
  led_flash();
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
            
            Serial.println("Straight_liene");
            for (int i=0; i<100; i++)
            {
              
  led_flash();
              valLeft = digitalRead(leftlinesensorPin); // read left input value
              valRight = digitalRead(rightlinesensorPin); // read right input value
              valFrontLeft = digitalRead(frontLeftPin); // read left input value
              valFrontRight = digitalRead(frontRightPin); // read right input value
              StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
              delay(10);
            }
          }
          else if (position == 7)
          {
            turn_right();
            Serial.println("Going in straight line for a git");
            for (int i=0; i<50; i++)
            {
  led_flash();
              valLeft = digitalRead(leftlinesensorPin); // read left input value
              valRight = digitalRead(rightlinesensorPin); // read right input value
              valFrontLeft = digitalRead(frontLeftPin); // read left input value
              valFrontRight = digitalRead(frontRightPin); // read right input value
              StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
              delay(20);
              
            }
          }
          else if (position == 8)
          {
            Serial.println("HERE");
            turn_right_at_intersection();
            
            Serial.println("Straight_liene");
            for (int i=0; i<100; i++)
            {
              
  led_flash();
              valLeft = digitalRead(leftlinesensorPin); // read left input value
              valRight = digitalRead(rightlinesensorPin); // read right input value
              valFrontLeft = digitalRead(frontLeftPin); // read left input value
              valFrontRight = digitalRead(frontRightPin); // read right input value
              StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
              delay(20);
              
            }
          }
          else
          {
            Serial.println("Going over intersection!");
            for (int i=0; i<50; i++)
            {
            
  led_flash();
              valLeft = digitalRead(leftlinesensorPin); // read left input value
              valRight = digitalRead(rightlinesensorPin); // read right input value
              valFrontLeft = digitalRead(frontLeftPin); // read left input value
              valFrontRight = digitalRead(frontRightPin); // read right input value
              StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
              delay(20);
              
            }
          }
          break;
        }
        StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
      }
      if (position == 10)
      {
        break;
      }
      if (found_block)
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

// Does what it says on the tin.
void drive_till_intersection()
{    int valLeft = digitalRead(leftlinesensorPin); // read left input value
    int valRight = digitalRead(rightlinesensorPin); // read right input value
    int valFrontLeft = digitalRead(frontLeftPin); // read left input value
    int valFrontRight = digitalRead(frontRightPin); // read right input value

  while(true)
  {
  led_flash();
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin); // read right input value
    StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
    int x = check_for_block();
    if (valLeft or valRight)
    {
      Serial.println("Intersection Detected!");
      break;
    }
  }
}

// Drives over the intersection. Useful for ignoring intersections along the grid.
void pass_over_intersection()
{   
  int valLeft = digitalRead(leftlinesensorPin); // read left input value
  int valRight = digitalRead(rightlinesensorPin); // read right input value
  int valFrontLeft = digitalRead(frontLeftPin); // read left input value
  int valFrontRight = digitalRead(frontRightPin); // read right input value
  while(true)
  {
  led_flash();
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin); // read right input value
    StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
    //StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(straight_speed);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(straight_speed);
    if (!valLeft && !valRight)
    {
      break;
    }
  }
}

int check_for_block()
{
  int x = sensor.getDistance();
  if (x < 50 && x > 30)
  {
    //d/igitalWrite(red_ledPin, HIGH);
    //digitalWrite(green_ledPin, LOW);
    return 1;
  }
  else
  {
    //digitalWrite(red_ledPin, LOW);
    //digitalWrite(green_ledPin, HIGH);
    return 0;
  }
}
// How to drive up to the grid position from the start box.
void start_to_grid()
{
  leftMotor->run(FORWARD);
  leftMotor->setSpeed(straight_speed);
  rightMotor->run(FORWARD);
  rightMotor->setSpeed(straight_speed);
  drive_till_intersection();
  pass_over_intersection();
  drive_till_intersection();
  turn_left_at_center();
  drive_till_intersection();
  pass_over_intersection();
  Serial.println("Done Turning!!");
  //drive_till_intersection();
  int valLeft = digitalRead(leftlinesensorPin); // read left input value
  int valRight = digitalRead(rightlinesensorPin); // read right input value
  int valFrontLeft = digitalRead(frontLeftPin); // read left input value
  int valFrontRight = digitalRead(frontRightPin); // read right input value
  for (int i=0; i<100; i++)
  {
  led_flash();
    //Serial.println("straing!");
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin); // read right input value
    StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
    delay(10);
  }
}

// Very nice (if I say so myself) recursive function that goes from one position to the next, then calls itself again until it reaches the home position.
void get_home(int position)
{
  
  int valLeft = digitalRead(leftlinesensorPin); // read left input value
  int valRight = digitalRead(rightlinesensorPin); // read right input value
  int valFrontLeft = digitalRead(frontLeftPin); // read left input value
  int valFrontRight = digitalRead(frontRightPin); // read right input value
  Serial.println(position);
  if (position != 0)
  {
    if (position == 1)
    {
      // Drive forwards
      drive_till_intersection();
      //pass_over_intersection();
      //drive_straight_for_a_bit();
    }
    if (position == 2)
    {
      // Drive forwards
      drive_till_intersection();
      pass_over_intersection();
      drive_straight_for_a_bit();
      
    }
    if (position == 3)
    {
      // Drive forwards, turn left
      drive_till_intersection();
      turn_left_at_intersection();
      drive_straight_for_a_bit();
    }
    if (position == 4)
    {
      // Drive forwards, turn left
      drive_till_intersection();
      turn_left();
      drive_straight_for_a_bit();

    }
    if (position == 5)
    {
      // Drive forwards
      drive_till_intersection();
      pass_over_intersection();
      drive_straight_for_a_bit();
    }
    if (position == 6)
    {
      // Drive forwards
      drive_till_intersection();
      pass_over_intersection();
      drive_straight_for_a_bit();
    }
    if (position == 7)
    {
      // Drive forwards
      drive_till_intersection();
      pass_over_intersection();
      drive_straight_for_a_bit();
    }
    if (position == 8)
    {
      // Drive forwards, turn left
      drive_till_intersection();
      turn_left_at_intersection();
      drive_straight_for_a_bit();

    }
    if (position == 9)
    {
      // Drive forwd, turn left
      drive_till_intersection();
      turn_left();
      drive_straight_for_a_bit();
    }
    position = position - 1;
    get_home(position);
  }
  else
  {
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(0);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(0);
  }
}




void StraightLineCharu()
{

  int read_dir[4];
  read(read_dir);
  if (read_dir[0] && read_dir[1] && !read_dir[2] && !read_dir[3])
  {
    // On a line, going straight. All good
    //continue straight
  led_flash();
  leftMotor->setSpeed(150);
  leftMotor->run(FORWARD);
  rightMotor->setSpeed(140);
  rightMotor->run(FORWARD);
  //Serial.println("straight");

  }
  
  else if (!read_dir[0]  && read_dir[1] && !read_dir[2] && !read_dir[3])
  {
    led_flash();
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(150);
    rightMotor->run(BACKWARD);
    rightMotor->setSpeed(150);
    //Serial.println("Correcting itself");
  }  
  else if (read_dir[0] && !read_dir[1] && !read_dir[2] && !read_dir[3])
  {
    led_flash();
    leftMotor->run(BACKWARD);
    leftMotor->setSpeed(150);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(150);
    //Serial.println("Correcting itself");
  }


}


void Start_from_origin()
{
  int curr_dir[4] = {0,0,0,0};
  int nLeft = 0;
  while(true)
  {
    read(curr_dir);
    StraightLineCharu();

    if(curr_dir[2] == 1 )
    {
      nLeft+= 1;
      Serial.println(nLeft);
      if(nLeft == 1)
      {
        StraightLineCharu();
        delay(500);
        // float time_start = millis();
        // while(millis() - time_start < 500)
        // {
        //   StraightLineCharu();
        //  // Serial.println("in the straight line")  ;
        // }
      }
      if(nLeft == 2)
      {
        while( curr_dir[0] || curr_dir[1])
        {
          read(curr_dir);
          leftMotor->run(FORWARD);
          rightMotor->run(FORWARD);
          leftMotor->setSpeed(150);
          rightMotor->setSpeed(150);
          
        }
        
        Serial.println("2nd left detected")  ;
        while(!(curr_dir[1]) || !curr_dir[0])
        {
            read(curr_dir);
            leftMotor->run(BACKWARD);
            leftMotor->setSpeed(200);
            rightMotor->run(FORWARD);
            rightMotor->setSpeed(200);
        }
        float time_2 = millis();
        while(millis() - time_2 < 1000)
        {          
          StraightLineCharu();         
        }
        break;  

      }
    }
  }

}

// Navigating the area where there are free space blocks
void small_rectangle()
{

  led_flash();
  int nRight = 0;
  int curr_dir[4] = {0,0,0,0};
  read(curr_dir);
  
  while(true)
  { 
    led_flash();
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
              led_flash();
              StraightLineCharu();
            }


        }
          
          

        if(nRight % 4 == 2)
        {
          led_flash();
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
              led_flash();       
              StraightLineCharu();           
            
          }
          

        }
        

        if (nRight % 4 == 3)
        { 
          
          Serial.println("in the 3rdt case");
            while(!(curr_dir[1] && curr_dir[0]))
            {
                led_flash();
                leftMotor->run(FORWARD);
                leftMotor->setSpeed(200);
                rightMotor->run(BACKWARD);
                rightMotor->setSpeed(200);
                read(curr_dir);
                
            }
           
            float time_start3 = millis();
            while(millis() - time_start3 < 2000)
            {   
                led_flash();
                StraightLineCharu();
              
            }
        }
          
        

        

        if (nRight % 4 == 0)
        {
          
          Serial.println("in the 4th case");
          while(!(curr_dir[2] && curr_dir[3]))
          {
              led_flash();
              leftMotor->run(FORWARD);
              leftMotor->setSpeed(200);
              rightMotor->run(BACKWARD);
              rightMotor->setSpeed(200);
              read(curr_dir);

          }
          
          float time_start4 = millis();
          while(millis() - time_start4 < 1000)
          {
            led_flash();
            leftMotor->run(FORWARD);
            leftMotor->setSpeed(200);
            rightMotor->run(FORWARD);
            rightMotor->setSpeed(180);
          }
          
        }
    }
        
       
    else
    {
      if(nRight % 4 == 0 || nRight % 4 == 2)
      {
        bool var2 = block_detection();
        if(!var2)
        {
          led_flash();
          StraightLineCharu();
          //Serial.println("Going in a straight line after 0 and 2");
        }
        else
        {
          if(nRight % 4 == 0)
          { 
            led_flash();
            Serial.println("Calling the grab");
            return_after_grab();
            delay(1000);
            big_loop_reverse_back_top();
            return;
            
          }
          if(nRight % 4 ==2)
          {          
            led_flash();  
            return_after_grab();
            delay(1000);
            big_loop_reverse_back_bottom();
            return;
          }
        }

      }
      else
      {
        led_flash();
        StraightLineCharu();
      }
     
      
    }
     
      
    


  }

}

// To get home once free space blocks are grabbed
void return_after_grab() // being called from small rectangle function 
{
  int curr_dir[4] = {0,0,0,0};
  read(curr_dir);

  while(true)
  { 
    led_flash();
    read(curr_dir);
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(250);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(230);
    
    while(curr_dir[2] && !(curr_dir[3])){    
      led_flash();
      leftMotor->run(BACKWARD);
      leftMotor->setSpeed(200);
      rightMotor->run(FORWARD);
      rightMotor->setSpeed(200);
      read(curr_dir);
    }

    while(curr_dir[3] && !(curr_dir[2])){   
      led_flash(); 
      leftMotor->run(FORWARD);
      leftMotor->setSpeed(200);
      rightMotor->run(BACKWARD);
      rightMotor->setSpeed(200);
      read(curr_dir);
    }

    if(curr_dir[3] && curr_dir[2])
    {
      led_flash();
      Serial.println('now align');
      leftMotor->run(FORWARD);
      leftMotor->setSpeed(0);
      rightMotor->run(BACKWARD);
      rightMotor->setSpeed(0);
      break;
    }
  }
}
void big_loop_reverse_back_bottom() // being called from small rectangle function
{
  int curr_dir[4] = {0,0,0,0};
  int turn_counter = 0;
  read(curr_dir);

  //turn left
  if(turn_counter == 0)
  { 
    led_flash();
    Serial.println(turn_counter);
    float time_start_reverse_0 = millis();
    while(((millis() - time_start_reverse_0) < 500) || !(curr_dir[0] && curr_dir[1]))
    {
    leftMotor->run(BACKWARD);
    leftMotor->setSpeed(255);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(255);
    read(curr_dir);
    }  
    turn_counter += 1;
 }
  

  //at grid corner
  if (turn_counter == 1)
  {
    Serial.println(turn_counter);
    while(true)
    {
      led_flash();
      read(curr_dir);
      if(curr_dir[2])
      {
        while(curr_dir[0]&& curr_dir[1])
        {
          led_flash();
          leftMotor->run(FORWARD);
          leftMotor->setSpeed(255);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(255);
          read(curr_dir);

        }

        while(!(curr_dir[0]&& curr_dir[1]))
        {
        
        led_flash();
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(255);
        rightMotor->run(BACKWARD);
        rightMotor->setSpeed(255);
        read(curr_dir);
        Serial.println("Should reach here .");
        }
        float time_start_reverse = millis();
        while(millis() - time_start_reverse < 1000)
        {
          led_flash();
          StraightLineCharu();
        }
        turn_counter += 1;
      
        break;
       
      }
      else
      {
        led_flash();
        StraightLineCharu();
      }
      
    }
  }

  //take second right turn
  if(turn_counter == 2)
  {
    
    Serial.println(turn_counter);
    while(true)
    {
        led_flash();
        read(curr_dir);
        if(curr_dir[3])
        {
          Serial.println("before turning");
          while(!(curr_dir[2] && curr_dir[3]))
          {
            led_flash();
            leftMotor->run(FORWARD);
            leftMotor->setSpeed(255);
            rightMotor->run(BACKWARD);
            rightMotor->setSpeed(235);
            read(curr_dir);
          }

          Serial.println("after turning");
          // float time_start_turn_reverse = millis();
          // while(millis() - time_start_turn_reverse < 10)
          // {
          //   leftMotor->run(BACKWARD);
          //   leftMotor->setSpeed(100);
          //   rightMotor->run(FORWARD);
          //   rightMotor->setSpeed(100);
            
          // }

          float time_start_reverse = millis();
          while(millis() - time_start_reverse < 50)
          {
            led_flash();
             leftMotor->run(FORWARD);
            leftMotor->setSpeed(200);
            rightMotor->run(FORWARD);
            rightMotor->setSpeed(200);
           
          }

          while(millis() - time_start_reverse < 1000)
          {
            led_flash();
            StraightLineCharu();
           
          }

          turn_counter += 1;       
          break;
        }
      else
      {
        led_flash();
        StraightLineCharu();
      }
    }
    
    
  }



  //go to stem of grid
  if(turn_counter == 3)
  {
    
    Serial.println("Turn 3");
    
    while(true)
    { 
      led_flash();
      read(curr_dir);
      if(curr_dir[2])
      {
          led_flash();
          Serial.println("Sees left");
          float time_turn_inter = millis();
          while((millis() - time_turn_inter < 500 ) || !(curr_dir[0] && curr_dir[1]))
          {
            led_flash();
            Serial.println("turn");
            leftMotor->run(BACKWARD);
            leftMotor->setSpeed(200);
            rightMotor->run(FORWARD);
            rightMotor->setSpeed(200);
            read(curr_dir);
          }
        
          float time_start_reverse = millis();
          while(millis() - time_start_reverse < 100)
          {
            led_flash();
            leftMotor->run(FORWARD);
            leftMotor->setSpeed(200);
            rightMotor->run(FORWARD);
            rightMotor->setSpeed(200);
            
          }

          while(millis() - time_start_reverse < 1000)
          {
            led_flash();
            StraightLineCharu();
           
          }

          turn_counter += 1;
          // while(true)
          // {
          //   leftMotor->run(BACKWARD);
          //   leftMotor->setSpeed(0);
          //   rightMotor->run(FORWARD);
          //   rightMotor->setSpeed(0);
          // }
          break;
      }
      else
      {
        led_flash();
        StraightLineCharu();
      } 
    }

      return;
  }
}

void big_loop_reverse_back_top() // being called from small rectangle function
{
  int curr_dir[4] = {0,0,0,0};
  int turn_counter = 0;
  int Right_increment = 0;
  read(curr_dir);

  //turn right
  if(turn_counter == 0)
  {
    Serial.println(turn_counter);
    while(curr_dir[0] || curr_dir[1])
    {
      led_flash();
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(200);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(200);
    read(curr_dir);


    }
    
    while(!(curr_dir[0] && curr_dir[1]))
    {
      led_flash();
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(255);
    rightMotor->run(BACKWARD);
    rightMotor->setSpeed(255);
    read(curr_dir);
    }
    turn_counter += 1;
   }

  //at top corner
  
  if (turn_counter == 1)
  {
    Serial.println(turn_counter);
    while(true)
    {
      led_flash();
      read(curr_dir);
      if(curr_dir[3])
      {
        Serial.println("saw the turn");
        while(curr_dir[0] || curr_dir[1])
        {
            led_flash();
            leftMotor->run(FORWARD);
            leftMotor->setSpeed(200);
            rightMotor->run(FORWARD);
            rightMotor->setSpeed(200);
            read(curr_dir);
        }

        while(!(curr_dir[0]&& curr_dir[1]))
        {
          led_flash();
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(200);
        rightMotor->run(BACKWARD);
        rightMotor->setSpeed(200);
        read(curr_dir);
        }
        float time_start_reverse = millis();
        while(millis() - time_start_reverse < 500)
        {
          led_flash();
          StraightLineCharu();
        }
        turn_counter += 1;
        break;
       
      }
      else
      {
        led_flash();
        StraightLineCharu();
        delay(500);
      }
      
    }
  }

  //take second right turn
  if(turn_counter == 2)
  {
    Serial.println(turn_counter);
    while(true)
    {led_flash();
        read(curr_dir);
        if(curr_dir[3])
        { 
          Right_increment += 1;
          if(Right_increment == 1)
          {
              float time_start = millis();
              while(millis() - time_start < 500)
              {led_flash();
                StraightLineCharu();
                Serial.println("in the straight line")  ;
              }
            
          }
          else
          {
              while(!(curr_dir[2] && curr_dir[3]))
              {led_flash();
                leftMotor->run(FORWARD);
                leftMotor->setSpeed(255);
                rightMotor->run(BACKWARD);
                rightMotor->setSpeed(235);
                read(curr_dir);
              }

              Serial.println("after turning");
      
            float time_start_reverse = millis();
            while(millis() - time_start_reverse < 50)
            {led_flash();
              leftMotor->run(FORWARD);
              leftMotor->setSpeed(200);
              rightMotor->run(FORWARD);
              rightMotor->setSpeed(200);
            
            }

            while(millis() - time_start_reverse < 1000)
            {led_flash();
              StraightLineCharu();            
            }

            turn_counter += 1;       
            break;
          }
        }     
      else
      {led_flash();
        StraightLineCharu();
      }
    }   
    
  }



  //go to stem of grid
 if(turn_counter == 3)
  {

    Serial.println(turn_counter);
    while(true)
    { 
      led_flash();
      read(curr_dir);
      if(curr_dir[2])
      {
          Serial.println("Sees left");
          float time_turn_inter = millis();
          while((millis() - time_turn_inter < 500 ) || !(curr_dir[0] && curr_dir[1]))
          {
            led_flash();
            Serial.println("turn");
            leftMotor->run(BACKWARD);
            leftMotor->setSpeed(200);
            rightMotor->run(FORWARD);
            rightMotor->setSpeed(200);
            read(curr_dir);
          }
        
          float time_start_reverse = millis();
          while(millis() - time_start_reverse < 100)
          {
            led_flash();
            leftMotor->run(FORWARD);
            leftMotor->setSpeed(200);
            rightMotor->run(FORWARD);
            rightMotor->setSpeed(200);
            
          }

          while(millis() - time_start_reverse < 1000)
          {
            led_flash();
            StraightLineCharu();           
          }

          turn_counter += 1;
          // while(true)
          // {
          //   leftMotor->run(BACKWARD);
          //   leftMotor->setSpeed(0);
          //   rightMotor->run(FORWARD);
          //   rightMotor->setSpeed(0);
          // }
          break;
      }
      else
      {
        led_flash();
        StraightLineCharu();
      } 
    }

    return;
  }
}

// How to detect blocks during free space navigation
bool block_detection()
{
    float sensity_t = analogRead(sensityPin);
    float dist_t = sensity_t * MAX_RANG / ADC_SOLUTION;//
    Serial.print("Ultrasonic readings");
    Serial.println(dist_t);
    if(dist_t < 50)
    {  
      Serial.println("block detected!");
      int read_array[4] = {0,0,0,0};
      read(read_array);

      float time_stamp = millis();
      while(millis() - time_stamp < 200)
      {
        led_flash();
          read(read_array);
          leftMotor->run(BACKWARD);
          leftMotor->setSpeed(200);
          rightMotor->run(BACKWARD);
          rightMotor->setSpeed(200);

      }
      float time_stamp_1 = millis();
      while((millis() - time_stamp_1 < 1300) || !(read_array[0] && read_array[1]))
      {
        led_flash();
          read(read_array);
          leftMotor->run(FORWARD);
          leftMotor->setSpeed(150);
          rightMotor->run(BACKWARD);
          rightMotor->setSpeed(150);
          if((read_array[2] || read_array[3]))
          {
            break;
          }
      }
      
      Serial.println("before aligning");
      int curr_dir[4] = {0,0,0,0};
      while(true)
      { 
        led_flash();
        read(curr_dir);
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(200);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(200);
        
        while(curr_dir[2] && !(curr_dir[3])){   
          led_flash(); 
          leftMotor->run(BACKWARD);
          leftMotor->setSpeed(200);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(200);
          read(curr_dir);
        }

        while(curr_dir[3] && !(curr_dir[2])){   
          led_flash(); 
          leftMotor->run(FORWARD);
          leftMotor->setSpeed(200);
          rightMotor->run(BACKWARD);
          rightMotor->setSpeed(200);
          read(curr_dir);
        }

        if(curr_dir[3] && curr_dir[2])
        {
          led_flash();
          Serial.println('now align');
          leftMotor->run(FORWARD);
          leftMotor->setSpeed(0);
          rightMotor->run(BACKWARD);
          rightMotor->setSpeed(0);
          break;
        }
      }
      
      myservo.write(0);
      float time_s = millis();    
      while(millis() - time_s < 3000 )
      {
        led_flash();
          read(read_array);
          leftMotor->run(FORWARD);
          leftMotor->setSpeed(200);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(200);
          Serial.println("in loop of going forward");
      }
      
      // Assuming we do not use the lidar
      // float lidar = sensor.getDistance() ;
  
      // while(lidar > 10)
      // {    
      //     Serial.println("block detected!");
      //     lidar = sensor.getDistance();
      //     Serial.println(lidar); 
      //     leftMotor->run(FORWARD);
      //     leftMotor->setSpeed(200);
      //     rightMotor->run(FORWARD);
      //     rightMotor->setSpeed(200);

      // }
  
      myservo.write(160);
      magnetic_Charu();
         leftMotor->run(FORWARD);
           leftMotor->setSpeed(0);
           rightMotor->run(FORWARD);
           rightMotor->setSpeed(0);
           delay(5000);
      
      

      
          
      return true;         

    }

    return false;

}



// Finally, the code that runs
void loop()
{

Serial.println("Traversing");
traverse_grid();
Serial.println("turning right at center");

turn_right_at_center();
Serial.println("going to bocs");

  go_to_box();
  drive_till_intersection();

  turn_left_at_center();
  magnetic_block = 0;
  myservo.write(0);
  drive_till_intersection();
  pass_over_intersection();
  drive_straight_for_a_bit();

  traverse_grid();
  turn_right_at_center();
  go_to_box();
  drive_till_intersection();
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(0);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(0);
  delay(5);
// Now do free space navigation
  for(int i = 0; i < 2; i++)
  { 
    step = 0;
    if(step == 0)  
  {  

  

    Start_from_origin();
    Serial.println("after");
    step += 1;
  }
  else if(step == 1)
  {
    small_rectangle(); 
    step += 1;   
  }
  else if(step == 2)
  {
    go_to_box();
    // go to one of the blocks
    //and reset
    //ans reset step to 0
  }

  }


  
}
