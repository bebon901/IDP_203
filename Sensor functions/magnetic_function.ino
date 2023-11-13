int green_ledPin = 13;
int red_ledPin = 12;
int magnetPin = 2; //change these 3 values according to connections
bool block_is_magnetic = false; //variable can be used for determining
                                //where to bring the block
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
    magnetic_stuff_reset();
  }
}
void magnetic() { //detects magnetism and turns the red
                  //or green LED on accordingly, which then stays on 
                  //until turned off with another function
  int val = 0;
  val = digitalRead(magnetPin);
  if (val == HIGH) {
  digitalWrite(red_ledPin, HIGH);
  block_is_magnetic = true
} else {
  digitalWrite(green_ledPin, HIGH);
}
}
void magnetic_stuff_reset() { //turns green and red LEDs off and turns boolean to false
  is_block_magnetic = false;
  digitalWrite(red_ledPin, LOW);
  digitalWrite(green_ledPin, LOW);
}
