int blue_led = 11;
int buttonPin = 7;
long int time_is_up = 280000;// change millisecond value
long int push = 0;
void setup() {
pinMode(blue_led, OUTPUT);
pinMode(buttonPin, INPUT);
}
void loop() { 
  if (millis() < 3000) { //blue led on while in start box
                         //before button si pressed
    digitalWrite(blue_led, HIGH);
  }
  int button = digitalRead(buttonPin);
  if (button == HIGH) { //get starting time and turn led off
    long int push = millis();
    digitalWrite(blue_led, LOW);
  }
  if (millis() - push == time_is_up) { //go back and turn led on after 5 mins
    //return to start block
    digitalWrite(blue_led, HIGH);
  }
}
