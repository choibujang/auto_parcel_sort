const int ENB = 6;
const int IN3 = 4;
const int IN4 = 5;

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    pinMode(ENB, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(IN3, HIGH); 
  digitalWrite(IN4, LOW); 
  analogWrite(ENB, 64); 
}