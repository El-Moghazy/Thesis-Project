/* Example code for the moisture sensor
 Connect the sensor output signal to the A0 (Analog 0) pin on the Arduino board
 The sensor value description
 0  ~300     dry soil
 300~700     humid soil
700~950     in water
*/

int analogValue = 0;

void setup(){
  Serial.begin(9600);
}

void loop(){
  analogValue = analogRead(A0);
  Serial.print("Moisture Sensor Value:");
  Serial.println(map(analogValue, 1023, 0, 100, 0)); 
  delay(100);
}
