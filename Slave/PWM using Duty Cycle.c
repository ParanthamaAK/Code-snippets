/*
  This is PWM control by getting the duty cycle value from Master through UART. The ledPin will be the OUPUT_ENABLE (OE) pin of the shift register. 
	The max value is 255 for 100% Brightness
	The min value is   1 for 1% Brightness
*/

const int ledPin = 4;  

const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

void setup() {
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(ledPin, ledChannel);
  Serial.begin(115200);
}

void loop() {
  int dutyCyclePercentage;
  while(!Serial.available()) {
  }
  
  dutyCyclePercentage = Serial.parseInt();
  
  if (dutyCyclePercentage >= 0 && dutyCyclePercentage <= 100) {
    int dutyCycleValue = map(dutyCyclePercentage, 0, 100, 0, 255);
    ledcWrite(ledChannel, dutyCycleValue);
  } 
}