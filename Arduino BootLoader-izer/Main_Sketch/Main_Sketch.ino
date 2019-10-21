#define DATA_PIN 8
#define CLOCK_PIN 7

void clockOut(int value) {
  digitalWrite(DATA_PIN, value);
  digitalWrite(CLOCK_PIN, 0);
  delayMicroseconds(7);
  digitalWrite(CLOCK_PIN, 1);
  delayMicroseconds(3);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  clockOut(0);
  clockOut(0);
  clockOut(0);
  
  clockOut(1);
  
  clockOut(0);
  
  clockOut(1);
  clockOut(1);
  
  clockOut(0);
  
  clockOut(1);
  clockOut(1);
  clockOut(1);
  
  clockOut(0);
  
  clockOut(1);
  clockOut(1);
  clockOut(1);
  clockOut(1);

  delayMicroseconds(2);
  digitalWrite(DATA_PIN, 0);
  digitalWrite(CLOCK_PIN, 0);
}
