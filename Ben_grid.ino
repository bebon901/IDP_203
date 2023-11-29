#include <Adafruit_MotorShield.h>
#include "Arduino.h"
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);
#define MAX_RANG (510)//the max measurement value of the module is 510cm(a little bit longer
#include "Wire.h"
#include "DFRobot_VL53L0X.h"
#include <Servo.h>
Servo myservo; // create servo object to control a servo
// twelve servo objects can be created on most boards
int pos = 0; // variable to store the servo position
int servo_pin = 9;
#define ADC_SOLUTION (1023.0)//ADC accuracy of Arduino UNO is 10bit
// select the input pin
float dist_t, sensity_t;
int block_magnetic = 0;
int magnetPin = 6;
float distance_0 = 100;
float distance_1 = 100;
float distance_2 = 100;
bool detected = false;
float lidar = 100;
DFRobot_VL53L0X sensor;

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *leftMotor = AFMS.getMotor(4);
Adafruit_DCMotor *rightMotor = AFMS.getMotor(3);
int frontLeftPin = 2;
int frontRightPin = 3; // Connect sensor to input pin 3
int leftlinesensorPin = 5;
int rightlinesensorPin = 4; // Connect sensor to input pin 3

int green_led = 13;
int red_led = 12;
int scanning = 0;
int sensityPin = A0; 

int blue_led = 11; //change this
int straight_speed = 200;
int left_turn_speed = 170;
int right_turn_speed = 150;
void setup() {
 Serial.begin(9600); // Init the serial port
 Wire.begin();
 pinMode(blue_led, OUTPUT); //Set Pin3 as output
 pinMode(red_led, OUTPUT);
pinMode(green_led, OUTPUT);
 pinMode(leftlinesensorPin, INPUT); // declare LED as output
 pinMode(rightlinesensorPin, INPUT); // declare Micro switch as input
 pinMode(frontRightPin, INPUT); // declare LED as output
 pinMode(frontLeftPin, INPUT); // declare Micro switch as input
 Wire.begin();
 // Open Grabber
 myservo.attach(servo_pin);
 myservo.write(0);

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


  // Drive up to the start!
 start_to_grid();
}


unsigned long previousMillis = 0;
int ledState = LOW;
void led_flash(){

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 250) { //flashes at 2 Hz
    // last time LED blinked
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(blue_led, ledState);

  }
}

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


void magnetic()
{
  if (digitalRead(magnetPin) == HIGH)
  {
    block_magnetic = 1;
  }
}

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
// void turn_left_at_center()
// {
//   led_flash();
//   Serial.println("Turning left at center");
//   int valLeft = digitalRead(leftlinesensorPin); // read left input value
//   int valRight = digitalRead(rightlinesensorPin); // read right input value
//   int valFrontLeft = digitalRead(frontLeftPin); // read left input value
//   int valFrontRight = digitalRead(frontRightPin); // read right input value
//   leftMotor->run(BACKWARD);
//   leftMotor->setSpeed(left_turn_speed);
//   rightMotor->run(FORWARD);
//   rightMotor->setSpeed(right_turn_speed);
//   delay(250);
//   while(true)
//   {
//   led_flash();
//     valLeft = digitalRead(leftlinesensorPin); // read left input value
//     valRight = digitalRead(rightlinesensorPin); // read right input value
//     valFrontLeft = digitalRead(frontLeftPin); // read left input value
//     valFrontRight = digitalRead(frontRightPin);

//     leftMotor->run(BACKWARD);
//     leftMotor->setSpeed(left_turn_speed);
//     rightMotor->run(FORWARD);
//     rightMotor->setSpeed(right_turn_speed);
//     //Serial.println(valRight);
//     //Serial.println("turning");
//     if(!valRight)
//       {
//         Serial.println("break");
//         break;
//       }
//   }
//   delay(150);
//   while (true)
//   {
//   led_flash();
//     valLeft = digitalRead(leftlinesensorPin); // read left input value
//     valRight = digitalRead(rightlinesensorPin); // read right input value
//     valFrontLeft = digitalRead(frontLeftPin); // read left input value
//     valFrontRight = digitalRead(frontRightPin);
//     //Serial.println(valRight);
//       if(valFrontLeft && valFrontRight)
//       {
//         //Serial.println("break");
//         Serial.println("break2");
//             leftMotor->run(FORWARD);
//           leftMotor->setSpeed(100);
//           rightMotor->run(FORWARD);
//           rightMotor->setSpeed(100);
//         break;
//       }
      
//   }
//   drive_straight_for_a_bit();

// }
void read(int read_array[])
{
    read_array[2] = digitalRead(leftlinesensorPin); // read left input value
    read_array[3] = digitalRead(rightlinesensorPin); // read right input value
    read_array[0] = digitalRead(frontLeftPin); // read frontleft input value
    read_array[1] = digitalRead(frontRightPin); // read frontright input value
    
}

void turn_right_at_center()
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
/*
{
  led_flash();
  Serial.println("Started");
  int valLeft = digitalRead(leftlinesensorPin); // read left input value
  int valRight = digitalRead(rightlinesensorPin); // read right input value
  int valFrontLeft = digitalRead(frontLeftPin); // read left input value
  int valFrontRight = digitalRead(frontRightPin); // read right input value

  float time_stamp = millis();
  
  while((millis() - time_stamp < 1000) )
  {
    led_flash();      
    int valLeft = digitalRead(leftlinesensorPin); // read left input value
    int valRight = digitalRead(rightlinesensorPin); // read right input value
    int valFrontLeft = digitalRead(frontLeftPin); // read left input value
    int valFrontRight = digitalRead(frontRightPin); // read right input value
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(200);
    rightMotor->run(BACKWARD);
    rightMotor->setSpeed(200);
  }

  Serial.println(" first");
  valLeft = digitalRead(leftlinesensorPin); // read left input value
  valRight = digitalRead(rightlinesensorPin); // read right input value
  valFrontLeft = digitalRead(frontLeftPin); // read left input value
  valFrontRight = digitalRead(frontRightPin); // read right input value
  while((!valLeft && !valRight))
  {
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin); // read right input value
  }
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(0);
    rightMotor->run(BACKWARD);
    rightMotor->setSpeed(0);

  delay(100000);
  while (true)
  {
    if (valRight && !valLeft)
    {
      leftMotor->run(FORWARD);
      leftMotor->setSpeed(left_turn_speed);
      rightMotor->run(BACKWARD);
      rightMotor->setSpeed(right_turn_speed);
    }
    if (valLeft && !valRight)
    {
      leftMotor->run(BACKWARD);
      leftMotor->setSpeed(left_turn_speed);
      rightMotor->run(FORWARD);
      rightMotor->setSpeed(right_turn_speed);
    }
    if (valLeft && valRight)
    {
      break;
    }
  }
  Serial.println("Drive Straight");
  drive_straight_for_a_bit();


}

*/
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
// void turn_right_at_center()
// {
//   led_flash();
//   Serial.println("Turning right at center");
//   int valLeft = digitalRead(leftlinesensorPin); // read left input value
//   int valRight = digitalRead(rightlinesensorPin); // read right input value
//   int valFrontLeft = digitalRead(frontLeftPin); // read left input value
//   int valFrontRight = digitalRead(frontRightPin); // read right input value
//   leftMotor->run(FORWARD);
//   leftMotor->setSpeed(left_turn_speed);
//   rightMotor->run(BACKWARD);
//   rightMotor->setSpeed(right_turn_speed);
//   delay(250);
//   while(true)
//   {
//   led_flash();
//     valLeft = digitalRead(leftlinesensorPin); // read left input value
//     valRight = digitalRead(rightlinesensorPin); // read right inpuFbrt value
//     valFrontLeft = digitalRead(frontLeftPin); // read left input value
//     valFrontRight = digitalRead(frontRightPin);

//     leftMotor->run(FORWARD);
//     leftMotor->setSpeed(left_turn_speed);
//     rightMotor->run(BACKWARD);
//     rightMotor->setSpeed(right_turn_speed);
//     //Serial.println(valRight);
//     //Serial.println("turning");
//     if(!valRight)
//       {
//         Serial.println("break");
//         break;
//       }
//   }
//   delay(150);

//   while (true)
//   {
//   led_flash();
//     valLeft = digitalRead(leftlinesensorPin); // read left input value
//     valRight = digitalRead(rightlinesensorPin); // read right input value
//     valFrontLeft = digitalRead(frontLeftPin); // read left input value
//     valFrontRight = digitalRead(frontRightPin);
//     //Serial.println(valRight);
//       if(valFrontLeft && valFrontRight)
//       {
//         //Serial.println("break");
//         Serial.println("break2");
//         leftMotor->run(FORWARD);
//         leftMotor->setSpeed(100);
//         rightMotor->run(FORWARD);
//         rightMotor->setSpeed(100);
//         break;
//       }
      
//   }
//   drive_straight_for_a_bit();

// }
  
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
void go_to_box(int magnetic_block)
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
  delay(100);
  // We now reach the intersection, we either go right or left:

if(valRight && valLeft)
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
//          float time_0 = millis();
//          while (millis() - time_0 < 1600)
//          {
//            valLeft = digitalRead(leftlinesensorPin); // read left input value
//            valRight = digitalRead(rightlinesensorPin); // read right input value
//            valFrontLeft = digitalRead(frontLeftPin); // read left input value
//            valFrontRight = digitalRead(frontRightPin); // read right input value
//            StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
//          }
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

          float time_5 = millis();
          while (millis() - time_5 < 150)
          {
            valLeft = digitalRead(leftlinesensorPin); // read left input value
            valRight = digitalRead(rightlinesensorPin); // read right input value
            valFrontLeft = digitalRead(frontLeftPin); // read left input value
            valFrontRight = digitalRead(frontRightPin); // read right input value
            StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
          }
//          leftMotor->run(BACKWARD);
//          leftMotor->setSpeed(150);
//          rightMotor->run(FORWARD);
//          rightMotor->setSpeed(150);
//          delay(300);
//          leftMotor->run(FORWARD);
//          leftMotor->setSpeed(255);
//          rightMotor->run(FORWARD);
//          rightMotor->setSpeed(230);
          delay(500);
          Serial.println("Hello!!");
                    leftMotor->run(FORWARD);
          leftMotor->setSpeed(150);
          rightMotor->run(BACKWARD);
          rightMotor->setSpeed(150);
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
      myservo.write(0);
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
        if (x)
        {
          leftMotor->run(FORWARD);
          leftMotor->setSpeed(0);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(0);
          Serial.println("Grabbing Block");
          myservo.write(160);
          if (block_magnetic)
          {
            // Turn on the Red LED!
            digitalWrite(red_led, HIGH);
          }
          else
          {
            // Turn on the green LED
            digitalWrite(green_led, HIGH);
          }
          // Wait for 5 seconds
          delay(5000);
          
          digitalWrite(green_led, LOW);
          digitalWrite(red_led, LOW);
          Serial.println(position);
          delay(1000);
          Serial.println("Calling 190");
          turn_180();
          Serial.println("Druve Straignt");
          drive_straight_for_a_short_bit();
          get_home(position + 1);
          
          leftMotor->run(FORWARD);
          leftMotor->setSpeed(0);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(0);
          found_block = 1;
          break;
          // /delay(10000);

        }
        // Reached first right
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
void reverse_to_position_1()
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
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(straight_speed);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(straight_speed);
    if (valLeft)
    {
      break;
    }
  }
  // we are over the start box now
  // turn right till aligned
  while (true)
  { 
  led_flash();
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin); // read right input value
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(straight_speed);
    rightMotor->run(BACKWARD);
    rightMotor->setSpeed(straight_speed);
    if (valFrontLeft && valFrontRight)
    {
      break;
    }
  }
  while (true)
  {
  led_flash();
    valLeft = digitalRead(leftlinesensorPin); // read left input value
    valRight = digitalRead(rightlinesensorPin); // read right input value
    valFrontLeft = digitalRead(frontLeftPin); // read left input value
    valFrontRight = digitalRead(frontRightPin); // read right input value
    StraightLine(valFrontLeft, valFrontRight, valRight, valLeft);
    if (valRight)
    {
      turn_right();
      break;
      // Finish this 
    }
  }
}
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
void loop()
{

Serial.println("Traversing");
traverse_grid();
Serial.println("turning right at center");

turn_right_at_center();
Serial.println("going to bocs");

  go_to_box(1);
  drive_till_intersection();

  turn_left_at_center();
  block_magnetic = 0;
  drive_till_intersection();
  pass_over_intersection();
  drive_straight_for_a_bit();

  traverse_grid();
  turn_right_at_center();
  go_to_box(1);
  drive_till_intersection();
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(0);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(0);
  delay(5);

}
