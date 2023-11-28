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

int step = 0;
int servo_pin = 9;
int green_ledPin = 13;
int red_ledPin = 12;
int magneticPin = 6;
int magnetic_block = 0;

Adafruit_DCMotor *leftMotor = AFMS.getMotor(4);
Adafruit_DCMotor *rightMotor = AFMS.getMotor(3);


int frontLeftPin = 2;
int frontRightPin = 3; // Connect sensor to input pin 3
int leftlinesensorPin = 5;
int rightlinesensorPin = 4; // Connect sensor to input pin 3
int sensityPin = A0; 


void magnetic_stuff_reset() 
{ 
  //turns green and red LEDs off and turns boolean to false  
  digitalWrite(red_ledPin, LOW);
  digitalWrite(green_ledPin, LOW);
  return false;

}

void magnetic() 
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
  myservo.write(145);        // close it initially
  pinMode(red_ledPin, OUTPUT);
  pinMode(green_ledPin, OUTPUT);
  pinMode(magneticPin, INPUT);

  // turn on the DC Motors
  leftMotor->run(RELEASE);
  rightMotor->run(RELEASE);


}

void read(int read_array[])
{
    read_array[2] = digitalRead(leftlinesensorPin); // read left input value
    read_array[3] = digitalRead(rightlinesensorPin); // read right input value
    read_array[0] = digitalRead(frontLeftPin); // read frontleft input value
    read_array[1] = digitalRead(frontRightPin); // read frontright input value
    
}

void StraightLine()
{

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

void Start_from_origin()
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
         // Serial.println("in the straight line")  ;
        }
      }
      if(nLeft == 2)
      {
        //Serial.println("2nd left detected")  ;
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
          StraightLine();         
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
          //Serial.println("Going in a straight line after 0 and 2");
        }
        else
        {
          if(nRight % 4 == 0)
          {
            return_after_grab();
            big_loop_reverse_back_top();
            break;
            
          }
          if(nRight % 4 ==2)
          {            
            return_after_grab();
            big_loop_reverse_back_bottom();
            break;
          }
        }

      }
      else
      {
        StraightLine();
      }
     
      
    }
     
      
    


  }

}

void return_after_grab() // being called from small rectangle function 
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
      leftMotor->setSpeed(100);
      rightMotor->run(FORWARD);
      rightMotor->setSpeed(100);
      read(curr_dir);
    }

    while(curr_dir[3] && !(curr_dir[2])){    
      leftMotor->run(FORWARD);
      leftMotor->setSpeed(100);
      rightMotor->run(BACKWARD);
      rightMotor->setSpeed(100);
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

void big_loop_reverse_back_bottom() // being called from small rectangle function
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
        StraightLine();
      } 
    }
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
        StraightLine();
      } 
    }
  }
}

void go_to_box()
{

    int read_array[4] = {0,0,0,0};


  while(true)
  {    
    read(read_array);
    StraightLine();

    
    if(!read_array[0] && !read_array[1] && read_array[3] && read_array[2])
    {
        if(magnetic_block)
        {
            // Turn right and head to the red box
            while(true)
            {
                read(read_array);                
                leftMotor->run(FORWARD);
                leftMotor->setSpeed(255);
                rightMotor->run(BACKWARD);
                rightMotor->setSpeed(255);
               
                if(read_array[0] && read_array[1])
                {
                    break;
                }
                
            }
            // Now facing the correct direction.
            // Follow this line for a while, until it finishes
            while(true)
            {
                // Following lines...
                read(read_array);
                StraightLine();
                
                if(!read_array[0] && !read_array[1])
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
                        read(read_array);
                        if(read_array[0] && read_array[1])break;
                        
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
        if (!magnetic_block)
        {
            // Turn left and head to the green box
            while(true)
            {
                read(read_array);
                leftMotor->run(BACKWARD);
                leftMotor->setSpeed(255);
                rightMotor->run(FORWARD);
                rightMotor->setSpeed(255);
                //Serial.println("turning");

                if(read_array[0] && read_array[1])
                {
                    break;
                }
                
            }
            // Now facing the correct direction.
            // Follow this line for a while, until it finishes
            while(true)
            {
                // Following lines...
                read(read_array);
                StraightLine();
                
                if(!read_array[0] && !read_array[1])
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
                        read(read_array);
                        if(read_array[0] && read_array[1])
                        {                            
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
            
            // Now facing the correct direction.
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
      myservo.write(0);

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

      float time_stamp = millis();
      while(millis() - time_stamp < 1200)
      {
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(200);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(200);
      }


      myservo.write(145);
      magnetic();

      //float time_stamp = millis();
//      while(millis() - time_stamp < 100)
//      {
//          leftMotor->run(FORWARD);
//          leftMotor->setSpeed(0);
//          rightMotor->run(FORWARD);
//          rightMotor->setSpeed(0);
//        
//      }
//      magnetic_stuff_reset();      
      return true;         

    }

    return false;

}





void loop()
{
  if(step == 0)
  {
    Start_from_origin();
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





