int green_ledPin = 13;
int red_ledPin = 12;
int magnetPin = 2; //change these 3 values according to connections
bool block_is_delivered = false; //this could be used differently,
                                 //only here for example code
void setup() {
  pinMode(red_ledPin, OUTPUT);
  pinMode(green_ledPin, OUTPUT);
  pinMode(magnetPin, INPUT); //all 3 need to be in the setup
}
void loop(){ //example to use the functions
  magnetic(); //make sure to only call magnetic() once per block!!!
  if (block_is_delivered) {
    leds_turn_off();
  }
}
void magnetic() { //detects magnetism and turns the red
                  //or green LED on accordingly, which then stays on 
                  //until turned off with another functionb
  int val = 0;
  val = digitalRead(magnetPin);
  if (val == HIGH) {
  digitalWrite(red_ledPin, HIGH);
} else {
  digitalWrite(green_ledPin, HIGH);
}
}
void leds_turn_off() { //turns green and red LEDs off
  digitalWrite(red_ledPin, LOW);
  digitalWrite(green_ledPin, LOW);
}
