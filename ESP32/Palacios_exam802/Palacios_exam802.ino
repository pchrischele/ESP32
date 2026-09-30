const uint8_t LED_0 = 34;
const uint8_t LED_1 = 35;
const uint8_t LED_2 = 32;
const uint8_t LED_3 = 33;
const uint8_t LED_4 = 26;
const uint8_t LED_5 = 27;
const uint8_t LED_6 = 14;

void setup() {
  // put your setup code here, to run once:
pinMode(LED_0, OUTPUT);
pinMode(LED_1, OUTPUT);
pinMode(LED_2, OUTPUT);
pinMode(LED_3, OUTPUT);
pinMode(LED_4, OUTPUT);
pinMode(LED_5, OUTPUT);
pinMode(LED_6, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(34,1);
digitalWrite(35,1);
digitalWrite(32,1);
digitalWrite(33,1);
digitalWrite(26,1);
digitalWrite(27,1);
digitalWrite(14,1);

delay(1000);
 
digitalWrite(34,1);
digitalWrite(35,1);
digitalWrite(32,1);
digitalWrite(33,1);
digitalWrite(26,1);
digitalWrite(27,1);
digitalWrite(14,0);

delay(1000);
 
digitalWrite(34,1);
digitalWrite(35,1);
digitalWrite(32,0);
digitalWrite(33,1);
digitalWrite(26,1);
digitalWrite(27,0);
digitalWrite(14,1);

delay(1000);
 
}
