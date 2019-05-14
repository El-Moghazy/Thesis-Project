#include <OneWire.h>
#include "DHT.h"

#include <Wire.h>
#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.
int reading = 0;
int distance_reading = 0;
int light_value_total = 108000;
//Delete before usage
//int light_value_total = 10000;

int light_value_sensor = 0;
int light_value_difference = 0;
const int light_threshold = 18000;
/////Delete before usage
//const int light_threshold = 100;

unsigned long time_light_start = 0;


int DS18S20_Pin = 9;  //DS18S20 Signal pin on digital 9
//Temperature chip i/o
OneWire ds(DS18S20_Pin); // on digital pin 2


#define DHTPIN 7// Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);


int analogValue = 0;
int period = 3000;
unsigned long time_now = 0;
unsigned long t = 0;
unsigned long elapsed_time_to_base = 0;
unsigned long elapsed_time_to_origin = 0;
unsigned long start_time_to_base = 0;
unsigned long start_time_to_origin = 0;
boolean to_base = false;
int period_ultrasonic = 100;
unsigned long time_now_ultrasonic = 0;


int period_soil_temperature = 750;
unsigned long time_now_soil_temperature = 0;

int speed = 150;
int motorpin1 = 2 ;
int motorpin2 = 4 ;
int speedpin1 = 3 ;
int speedpin2 = 5 ;
int moisture_value = 0;

const int pingPin_left = 10;
const int pingPin_right = 11;
int cm_right = 0;
int cm_left = 0;
boolean stopped_base = false;
boolean stopped_origin = false;
boolean light = false;
int raspberry_from_base = 12;
int raspberry_to_base = 13;

char incomingByte = "";

String state = "";

void setup() {
  pinMode(motorpin1, OUTPUT);
  pinMode(motorpin2, OUTPUT);
  pinMode(speedpin1, OUTPUT);
  pinMode(speedpin2, OUTPUT);
  Wire.begin(); // join i2c bus

  pinMode(raspberry_to_base, OUTPUT);
  pinMode(raspberry_from_base, OUTPUT);

  Serial.begin(9600);
  dht.begin();
  time_light_start = millis();
}

void loop() {

//  Serial.println(map(analogRead(A1), 1023, 0, 100, 0));
  if((millis() - time_light_start >= 86400000) && light == false){
//           Serial.println(light);
//           Serial.println(light_value_total);
//           Serial.println(light_threshold);
//           Serial.println("/////////////////////////////////");

    if(light_value_sensor  < light_value_total - light_threshold){
//      Serial.println("Here");
      light_value_difference = (light_value_total - light_value_sensor);
      light = true;
    }
    
  }
  //  Serial.print("base ");
  //  Serial.println(elapsed_time_to_base);
  //  Serial.print("origin ");
  //  Serial.println(elapsed_time_to_origin);

  incomingByte = Serial.read();
  if (incomingByte == 'z') {
    elapsed_time_to_base = millis() - start_time_to_base;
    stopped_base = true;
    to_base = false;
    time_light_start = millis();
  }
  if (stopped_base == true || stopped_origin == true) {

    stop_robot();

    if (light && stopped_base == true){ // keep the robot idle till it gets the light it needs
        delay(light_value_difference);
        // reinitiate the light difference
        light_value_difference = 0;
        // reset the flag
        Serial.write("d");
        light_value_sensor = 0;
        state = "original";
        backward();
        delay(100);
        left();
        delay(2000);
        stopped_base = false;
        light = false;
        start_time_to_origin = millis();
}

  }


  if ( map(moisture(), 1023, 0, 100, 0) <= 50 && stopped_base == true) {
    Serial.write("t");
    digitalWrite(raspberry_from_base, HIGH);
    delay(500);
    digitalWrite(raspberry_from_base, LOW);

    state = "original";
    backward();
    delay(100);
    left();
    delay(2000);
    stopped_base = false;
    start_time_to_origin = millis();

  }

  if (stopped_base == false && state != "original") {

    if (millis() > time_now + period) {
      moisture_value = map(moisture(), 1023, 0, 100, 0);


//      float temperature = soil_temperature(); //will take about 750ms to run
//
//      int* humidity_ptr = humidity();

      light_value_sensor += map(analogRead(A0), 1023, 0, 50, 0);
                        

      time_now = millis();
    }

    if (moisture_value >= 70 || light == true) {
      
      if (to_base == false) {
        // this means that the robot just started going to the base and didn't change the flag yet
        if(!light){
        digitalWrite(raspberry_to_base, HIGH);
        delay(500);
        digitalWrite(raspberry_to_base, LOW);
        }
        else{
         
//          Serial.println((light_value_difference/3600000));
          Serial.write("l");          
        }
        
        //        Serial.println(moisture_value);

        start_time_to_base = millis();
      }
      
      to_base = true;
      seek_base();
    }
    else {
      stop_robot();
    }

  }
  else if (stopped_base == false && state == "original") {
    // TODO state should change once the robot reaches the original position of motion
    elapsed_time_to_origin = millis() - start_time_to_origin;
    if (elapsed_time_to_origin >= elapsed_time_to_base) {
      right();
      delay(2000);
      stop_robot();
      state = "";
      elapsed_time_to_origin = 0;
      stopped_base = false;
      stopped_origin = true;
    }
    else {
      seek_original_position();
    }

  }

}

void seek_original_position() {

  if ( distance() >= 35) {
    forward();
    delay(20);
    follow_wall("original");
  }

  else if (distance() < 35)
  {
    right();
    delay(800);
  }
}

void seek_base() {

  if ( distance() >= 35) {
    forward();
    delay(20);
    follow_wall("base");
  }

  else if (distance() < 35)
  {
    left();
    delay(800);
  }
}

void follow_wall(String state_wall_following) {
  // takes state as an input which clarifies if it is seeking base or seeking the original position


  if ( state_wall_following == "base") {
    cm_right = distance_ultrasonic(pingPin_left, "Left");
    cm_left  = distance_ultrasonic(pingPin_right, "Right");

    if (cm_right > 30 and cm_right <= 40) {
      right();
      delay(50);
    }

    else if (cm_right < 25) {
      left();
      delay(50);
    }

    else if (cm_right > 45) {
      right();
      delay(100);
    }
  }

  if ( state_wall_following == "original") {
    cm_right = distance_ultrasonic(pingPin_left, "Left");
    cm_left  = distance_ultrasonic(pingPin_right, "Right");

    if (cm_left > 30 and cm_left <= 40) {
      left();
      delay(50);


    }

    else if (cm_left < 25) {
      right();
      delay(50);

    }

    else if (cm_left > 45) {
      left();
      delay(100);
    }

  }

}


int moisture() {
  return analogRead(A1);
}

float soil_temperature() {
  //returns the temperature from one DS18S20 in DEG Celsius
  float TemperatureSum = 0;
  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
    //no more sensors on chain, reset search
    ds.reset_search();
    return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
    Serial.print("Device is not recognized");
    return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1); // start conversion, with parasite power on at the end

  if (millis() > time_now_soil_temperature + period_ultrasonic) {


    byte present = ds.reset();
    ds.select(addr);
    ds.write(0xBE); // Read Scratchpad


    for (int i = 0; i < 9; i++) { // we need 9 bytes
      data[i] = ds.read();
    }

    ds.reset_search();

    byte MSB = data[1];
    byte LSB = data[0];

    float tempRead = ((MSB << 8) | LSB); //using two's compliment
    TemperatureSum = tempRead / 16;

    time_now_soil_temperature = millis();
  }

  return TemperatureSum;

}



int* humidity() {
  int* res = new int[3];
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  res[0] = h;
  res[1] = t;
  res[2] = hic;
  return res;
}



void backward() {
  digitalWrite (motorpin1, LOW);
  digitalWrite (motorpin2, HIGH);
  analogWrite(speedpin1, speed);
  analogWrite(speedpin2, speed);
}

void forward() {
  digitalWrite (motorpin1, HIGH);
  digitalWrite (motorpin2, LOW);
  analogWrite(speedpin1, speed);
  analogWrite(speedpin2, speed);
}

void right() {
  digitalWrite (motorpin1, HIGH);
  digitalWrite (motorpin2, HIGH);
  analogWrite(speedpin1, speed);
  analogWrite(speedpin2, speed);
}

void left() {
  digitalWrite (motorpin1, LOW);
  digitalWrite (motorpin2, LOW);
  analogWrite(speedpin1, speed);
  analogWrite(speedpin2, speed);
}

void stop_robot() {
  analogWrite(speedpin1, 0);
  analogWrite(speedpin2, 0);
}

int distance() {

  Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
  Wire.write((int)RegisterMeasure); // sets register pointer to  (0x00)
  Wire.write((int)MeasureValue); // sets register pointer to  (0x00)
  Wire.endTransmission(); // stop transmitting

  delay(20); // Wait 20ms for transmit

  Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
  Wire.write((int)RegisterHighLowB); // sets register pointer to (0x8f)
  Wire.endTransmission(); // stop transmitting

  delay(20); // Wait 20ms for transmit

  Wire.requestFrom((int)LIDARLite_ADDRESS, 2); // request 2 bytes from LIDAR-Lite

  if (2 <= Wire.available()) // if two bytes were received
  {
    reading = Wire.read(); // receive high byte (overwrites previous reading)
    reading = reading << 8; // shift high byte to be high 8 bits
    reading |= Wire.read(); // receive low byte as lower 8 bits
    reading = reading - 15 ;

  }
  return reading;

}


int distance_ultrasonic(int pingPin, String sensor) {

  // establish variables for duration of the ping, and the distance result
  // in inches and centimeters:
  long duration, inches, cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingPin, OUTPUT);

  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH pulse
  // whose duration is the time (in microseconds) from the sending of the ping
  // to the reception of its echo off of an object.
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);

  delay(100);

  return cm;
}

long microsecondsToInches(long microseconds) {
  // According to Parallax's datasheet for the PING))), there are 73.746
  // microseconds per inch (i.e. sound travels at 1130 feet per second).
  // This gives the distance travelled by the ping, outbound and return,
  // so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}
