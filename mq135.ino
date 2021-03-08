// MQ_135 air quality sensor

int mq135 = 0;

ADC_MODE(ADC_TOUT);    // default

void setup() {
        Serial.begin(9600);
}

void loop() {
        mq135 = analogRead(A0);

        Serial.print("Air Quality : ");
        Serial.print(mq135);
        delay(300);
}
