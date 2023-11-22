#include <Adafruit_MotorShield.h>
#include "Arduino.h"
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);
#define MAX_RANG (520)//the max measurement value of the module is 520cm(a little bit longer
#include <Wire.h>
#include "DFRobot_VL53L0X.h"
DFRobot_VL53L0X sensor;
#define ADC_SOLUTION (1023.0)//ADC accuracy of Arduino UNO is 10bit
#define HIGH_ACCURACY
#define LONG_RANGE
int step = 0;
int magnetic = 0;
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

void magnetic_stuff_reset() 
{ //turns green and red LEDs off and turns boolean to false
  block_found = true;
  digitalWrite(red_ledPin, LOW);
  digitalWrite(green_ledPin, LOW);
  return false;
}

void magnetic() 
{ //detects magnetism and turns the red
  //or green LED on accordingly, which then stays on 
  //until turned off with another function
  int val = 0;
  val = digitalRead(magnetPin);
  if (val == HIGH)
  {
    digitalWrite(red_ledPin, HIGH);  
  } 
  else 
  {
    digitalWrite(green_ledPin, HIGH);  
  }
}

void setup() 
{
 Serial.begin(9600); // Init the serial port
 Wire.begin();
 sensor.begin(0x50);
//Set to Back-to-back mode and high precision mode
 sensor.setMode(sensor.eContinuous,sensor.eHigh);
//Laser rangefinder begins to work
 sensor.start();
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
  myservo.attach(servo_pin); // attaches the servo on pin 9 to the servo object
  pinMode(red_ledPin, OUTPUT);
  pinMode(green_ledPin, OUTPUT);
  pinMode(magnetPin, INPUT);

  leftMotor->run(RELEASE);
  rightMotor->run(RELEASE);

}


void mag_and_grab()
{
  


}

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
//  if(step == 0)
//  {
//    start_from_origin();
//    step += 1;
//  }
//  else
//  {
//    
//     
//    small_rectangle();
//  } 
//
    bool flag = true;
   //big_loop_reverse_back_bottom();
    if (flag){
      return_after_grab();
      big_loop_reverse_back_top();
      
    }
      flag = false;
      leftMotor->run(BACKWARD);
      leftMotor->setSpeed(0);
      rightMotor->run(FORWARD);
      rightMotor->setSpeed(0);



}


void start_from_origin()
{
  int curr_dir[4] = {0,0,0,0};
  int nLeft = 0;
  while(true)
  {
    read(curr_dir);
    StraightLine();

    if(curr_dir[2] == 1 )
    {
      nLeft+= 1;
      Serial.println(nLeft);
      if(nLeft == 1)
      {
        float time_start = millis();
        while(millis() - time_start < 500)
        {
          StraightLine();
          Serial.println("in the straight line")  ;
        }
      }
      if(nLeft == 2)
      {
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
        while(millis() - time_2 < 3000)
        {
          Serial.println("broken");
          StraightLine();
          Serial.println("in the straight line")  ;
        }
        break;        
      }
    }
  }

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
      if(nRight % 4 == 0 || nRight % 4 == 2)
      {
        bool var2 = block_detection();
        if(!var2)
        {
          StraightLine();
          Serial.println("Going in a straight line after 0 and 2");
        }

      }
      else
      {
        StraightLine();
      }
     
      
    }


  }

}

void return_after_grab()
{
  int curr_dir[4] = {0,0,0,0};
  read(curr_dir);

  while(true)
  { 
    read(curr_dir);
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(150);
    rightMotor->run(FORWARD);
    rightMotor->setSpeed(130);
    
    while(curr_dir[2] && !(curr_dir[3])){    
      leftMotor->run(BACKWARD);
      leftMotor->setSpeed(70);
      rightMotor->run(FORWARD);
      rightMotor->setSpeed(70);
      read(curr_dir);
    }

    while(curr_dir[3] && !(curr_dir[2])){    
      leftMotor->run(FORWARD);
      leftMotor->setSpeed(70);
      rightMotor->run(BACKWARD);
      rightMotor->setSpeed(70);
      read(curr_dir);
    }

    if(curr_dir[3] && curr_dir[2])
    {
      Serial.println('now align');
      leftMotor->run(FORWARD);
      leftMotor->setSpeed(0);
      rightMotor->run(BACKWARD);
      rightMotor->setSpeed(0);
      break;
    }
  }
}

bool block_detection()
{
    float sensity_t = analogRead(sensityPin);
    float dist_t = sensity_t * MAX_RANG / ADC_SOLUTION;//
    Serial.print("Ultrasonic readings");
    Serial.println(dist_t);
    if(dist_t < 40)
    {  
      Serial.println("block detected!");
      int read_array[4] = {0,0,0,0};
      read(read_array);
      while(!(read_array[2] && read_array[3]))
      {
          read(read_array);
          leftMotor->run(FORWARD);
          leftMotor->setSpeed(150);
          rightMotor->run(BACKWARD);
          rightMotor->setSpeed(150);
                 
      }
      myservo.write(-90);
      float lidar = sensor.getDistance() ;
      while(lidar > 10)
      {    
          lidar = sensor.getDistance();
          Serial.println(lidar); 
          leftMotor->run(FORWARD);
          leftMotor->setSpeed(150);
          rightMotor->run(FORWARD);
          rightMotor->setSpeed(130);

      }
      myservo.write(90);
      return_after_grab();          

    }

    return false;

}

 void big_loop_reverse_back_bottom()
 {
  int curr_dir[4] = {0,0,0,0};
  int turn_counter = 0;
  read(curr_dir);

  //turn left
  if(turn_counter == 0)
  { 
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
    // while(true)
    // {
    //   leftMotor->run(BACKWARD);
    //   leftMotor->setSpeed(0);
    //   rightMotor->run(FORWARD);
    //   rightMotor->setSpeed(0);
    // }


   }
  

  //at grid corner
  if (turn_counter == 1)
  {
    Serial.println(turn_counter);
    while(true)
    {
      read(curr_dir);
      if(curr_dir[2])
      {
        while(!(curr_dir[0]&& curr_dir[1]))
        {
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(255);
        rightMotor->run(BACKWARD);
        rightMotor->setSpeed(255);
        read(curr_dir);
        }
        float time_start_reverse = millis();
        while(millis() - time_start_reverse < 1000)
        {
          StraightLine();
        }
        turn_counter += 1;
      
        break;
       
      }
      else
      {
        StraightLine();
      }
      
    }
  }

  //take second right turn
  if(turn_counter == 2)
  {
    
    Serial.println(turn_counter);
    while(true)
    {
        read(curr_dir);
        if(curr_dir[3])
        {
          Serial.println("before turning");
          while(!(curr_dir[2] && curr_dir[3]))
          {
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
             leftMotor->run(FORWARD);
            leftMotor->setSpeed(200);
            rightMotor->run(FORWARD);
            rightMotor->setSpeed(200);
           
          }

          while(millis() - time_start_reverse < 1000)
          {
            StraightLine();
           
          }

          turn_counter += 1;       
          break;
        }
      else
      {
        StraightLine();
      }
    }
    
    
  }



  //go to stem of grid
  if(turn_counter == 3)
  {

    
    while(true)
    { 
      read(curr_dir);
      if(curr_dir[2])
      {
          Serial.println("Sees left");
          float time_turn_inter = millis();
          while((millis() - time_turn_inter < 500 ) || !(curr_dir[0] && curr_dir[1]))
          {
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
            leftMotor->run(FORWARD);
            leftMotor->setSpeed(200);
            rightMotor->run(FORWARD);
            rightMotor->setSpeed(200);
            
          }

           while(millis() - time_start_reverse < 1000)
          {
            StraightLine();
           
          }



          turn_counter += 1;
          while(true)
          {
            leftMotor->run(BACKWARD);
            leftMotor->setSpeed(0);
            rightMotor->run(FORWARD);
            rightMotor->setSpeed(0);
          }
          break;
      }
      else
      {
        StraightLine();
      } 
    }
  }
}
void big_loop_reverse_back_top()
{
  int curr_dir[4] = {0,0,0,0};
  int turn_counter = 0;
  int Right_increment = 0;
  read(curr_dir);

  //turn right
  if(turn_counter == 0)
  {
    float time_start_reverse_0 = millis();
    while(((millis() - time_start_reverse_0) < 500) || !(curr_dir[0] && curr_dir[1]))
    {
    leftMotor->run(FORWARD);
    leftMotor->setSpeed(200);
    rightMotor->run(BACKWARD);
    rightMotor->setSpeed(200);
    read(curr_dir);
    }
    turn_counter += 1;
   }

  //at top corner
  
  if (turn_counter == 1)
  {
    while(true)
    {
      read(curr_dir);
      if(curr_dir[3])
      {

        while(!(curr_dir[0]&& curr_dir[1]))
        {
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(200);
        rightMotor->run(BACKWARD);
        rightMotor->setSpeed(200);
        read(curr_dir);
        }
        float time_start_reverse = millis();
        while(millis() - time_start_reverse < 500)
        {
          StraightLine();
        }
        turn_counter += 1;
        break;
       
      }
      else
      {
        StraightLine();
      }
      
    }
  }

  //take second right turn
  if(turn_counter == 2)
  {

    while(true)
    {
        read(curr_dir);
        if(curr_dir[3])
        { 
          Right_increment += 1;
          if(Right_increment == 1)
          {
              float time_start = millis();
              while(millis() - time_start < 500)
              {
                StraightLine();
                Serial.println("in the straight line")  ;
              }
            
          }
          else
          {
              while(!(curr_dir[2] && curr_dir[3]))
              {
                leftMotor->run(FORWARD);
                leftMotor->setSpeed(255);
                rightMotor->run(BACKWARD);
                rightMotor->setSpeed(235);
                read(curr_dir);
              }

              Serial.println("after turning");
      
            float time_start_reverse = millis();
            while(millis() - time_start_reverse < 50)
            {
              leftMotor->run(FORWARD);
              leftMotor->setSpeed(200);
              rightMotor->run(FORWARD);
              rightMotor->setSpeed(200);
            
            }

            while(millis() - time_start_reverse < 1000)
            {
              StraightLine();            
            }

            turn_counter += 1;       
            break;
          }
        }     
      else
      {
        StraightLine();
      }
    }   
    
  }



  //go to stem of grid
 if(turn_counter == 3)
  {

    
    while(true)
    { 
      read(curr_dir);
      if(curr_dir[2])
      {
          Serial.println("Sees left");
          float time_turn_inter = millis();
          while((millis() - time_turn_inter < 500 ) || !(curr_dir[0] && curr_dir[1]))
          {
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
            leftMotor->run(FORWARD);
            leftMotor->setSpeed(200);
            rightMotor->run(FORWARD);
            rightMotor->setSpeed(200);
            
          }

           while(millis() - time_start_reverse < 1000)
          {
            StraightLine();
           
          }



          turn_counter += 1;
          while(true)
          {
            leftMotor->run(BACKWARD);
            leftMotor->setSpeed(0);
            rightMotor->run(FORWARD);
            rightMotor->setSpeed(0);
          }
          break;
      }
      else
      {
        StraightLine();
      } 
  }
}
