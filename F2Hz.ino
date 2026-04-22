int ledPin = 23; 
int freq = 1493; 
int ledChannel = 0; 
int resolution = 8; // 255 Scale 
int dutyCycle = 255 * 0.75; 
 
void setup() { 
  ledcSetup(ledChannel, freq, resolution); 
  ledcAttachPin(ledPin, ledChannel); 
  ledcWrite(ledChannel, dutyCycle); 
} 
 
void loop() { 
} 