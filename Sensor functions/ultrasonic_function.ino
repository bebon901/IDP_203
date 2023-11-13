/*
Ultrasonic sensor
*/
#define MAX_RANG (520)//the max measurement value of the module is 520cm(a little bit longer than effective max range)
#define ADC_SOLUTION (1023.0)//ADC accuracy of Arduino UNO is 10bit


int sensityPin = A0;
//function which returns distance measurement from ultrasonic
void setup() {
  Serial.begin(9600);
}

void loop(){
float dist = ultrasonic_distance();
Serial.println(dist);
delay(500);
}

float ultrasonic_distance(){
float dist_t, sensity_t;
 // read the value from the sensor:
sensity_t = analogRead(sensityPin);
 // turn the ledPin on
dist_t = sensity_t * MAX_RANG / ADC_SOLUTION;//
//Serial.print(dist_t,0);
return dist_t;
//Serial.println("cm");dis

}



