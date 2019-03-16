#include <OneWire.h> 
#include "DHT.h"

int DS18S20_Pin = 9;  //DS18S20 Signal pin on digital 9
//Temperature chip i/o
OneWire ds(DS18S20_Pin); // on digital pin 2


#define DHTPIN 7// Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);


int analogValue = 0;
int period = 3000;
unsigned long time_now = 0;
int speed = 250;
int motorpin1 = 2 ;
int motorpin2 = 4 ;
int speedpin1 = 3 ;
int speedpin2 = 5 ;
int moisture_value = 0;

void setup(){
  pinMode(motorpin1, OUTPUT);
  pinMode(motorpin2, OUTPUT);
  pinMode(speedpin1, OUTPUT);
  pinMode(speedpin2, OUTPUT);
  Serial.begin(9600);
  dht.begin();

}

void loop(){
  if(millis() > time_now + period){
  moisture_value = map(moisture(), 1023, 0, 100, 0);
  Serial.print("Moisture Sensor Value:");
  Serial.println(moisture_value); 

  float temperature = soil_temperature(); //will take about 750ms to run
  Serial.println(temperature);

  int* humidity_ptr = humidity();
  
  Serial.print(F("Humidity: "));
  Serial.print(humidity_ptr[0]);
  Serial.print(F("%  Temperature: "));
  Serial.print(humidity_ptr[1]);
  Serial.print(F("°C "));
  Serial.print(humidity_ptr[2]);
  Serial.println(F("°C "));

  time_now = millis();

  }
  if(moisture_value >= 70){
    forward();
  }

  else{
    stop_robot();
  }
 
 }


int moisture(){
 return analogRead(A0);
}

float soil_temperature(){
  //returns the temperature from one DS18S20 in DEG Celsius

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
ds.write(0x44,1); // start conversion, with parasite power on at the end

delay(750); // Wait for temperature conversion to complete

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
float TemperatureSum = tempRead / 16;

return TemperatureSum;

}



int* humidity(){
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

void stop_robot(){
  analogWrite(speedpin1, 0);
  analogWrite(speedpin2, 0);
}
