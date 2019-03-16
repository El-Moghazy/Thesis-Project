int speed = 250;

int motorpin1 = 2 ;
int motorpin2 = 4 ;
int speedpin1 = 3 ;
int speedpin2 = 5 ;


void setup() {
  pinMode(motorpin1, OUTPUT);
  pinMode(motorpin2, OUTPUT);
  pinMode(speedpin1, OUTPUT);
  pinMode(speedpin2, OUTPUT);
  Serial.begin(9600);
}
void loop()
{

  forward();

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

void stop(){
  analogWrite(speedpin1, 0);
  analogWrite(speedpin2, 0);
}
