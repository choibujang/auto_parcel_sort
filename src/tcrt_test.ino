const int DETECT_PIN = 13;

void setup() {
    Serial.begin(9600);
    pinMode(DETECT_PIN, INPUT);
}

void loop() {
    Serial.println(digitalRead(DETECT_PIN));

}