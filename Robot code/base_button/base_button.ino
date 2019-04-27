

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 2;     // the number of the pushbutton pin
const int prototypePin = 8;     // the number of the pushbutton pin
const int pump = 9;     // the number of the pushbutton pin

boolean stopped = false;
// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int prototypeState = 0;         // variable for reading the pushbutton status
const int pingPin = 10;
const int buzzer = 7;

void setup() {
  
  Serial.begin(9600);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  pinMode(prototypePin, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(pump, OUTPUT);
  digitalWrite(pump, HIGH);

  
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  prototypeState = digitalRead(prototypePin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH && stopped == false) {
    // send stop signal
    Serial.write("z");
    stopped = true;
    digitalWrite(pump, LOW);
  }

  if (prototypeState == HIGH && stopped == true){
    // send turn around signal
    Serial.write("t");
    delay(500);
    stopped = false;
    digitalWrite(pump, HIGH);
  }

//  Serial.println(distance_ultrasonic());

  if(distance_ultrasonic() <= 13){
    digitalWrite(buzzer, LOW);
    // lamp red
  }
  else if(distance_ultrasonic() <= 20){
    digitalWrite(buzzer, LOW);
    // lamp yellow
  }
  else{
    digitalWrite(buzzer, HIGH);
    // lamp green
  }
  
}


int distance_ultrasonic() {

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
