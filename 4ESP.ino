const int pulsePin = 23;

volatile unsigned long pulseCount = 0;
volatile bool newPulse = false;

void IRAM_ATTR onPulse() {
  pulseCount++;
  newPulse = true;
}

void setup() {
  pinMode(pulsePin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pulsePin), onPulse, FALLING);
  Serial.begin(115200);
  Serial.println("RxBoard Ready - Waiting for pulses...");
}

unsigned long lastPulseTime = 0;
unsigned long displayedCount = 0;
bool sessionActive = false;

void loop() {
  if (newPulse) {
    newPulse = false;
    lastPulseTime = millis();
    sessionActive = true;

    // แสดงตัวเลขนับขึ้น
    Serial.println(pulseCount);
  }

  // ถ้าหยุด pulse นานกว่า 500ms = จบชุดนี้แล้ว
  if (sessionActive && (millis() - lastPulseTime) > 500) {
    Serial.println("---- Done: " + String(pulseCount) + " pulses ----");
    Serial.println();
    pulseCount = 0;
    sessionActive = false;
  }
}