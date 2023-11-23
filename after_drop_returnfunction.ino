void After_drop_return(int magnetic)
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
