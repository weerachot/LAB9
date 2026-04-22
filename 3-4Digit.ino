const int pulsePin = 23;

// ===== Digit pins =====
int digitPins[4] = {13, 12, 14, 27};

// ===== Segment pins a b c d e f g =====
int segPins[7] = {15, 2, 4, 5, 18, 19, 21};

// ===== ตัวเลข 0-9 =====
// a b c d e f g
byte numbers[10][8] = {
  {1,1,1,1,1,1,0}, // 0
  {0,1,1,0,0,0,0}, // 1
  {1,1,0,1,1,0,1}, // 2
  {1,1,1,1,0,0,1}, // 3
  {0,1,1,0,0,1,1}, // 4
  {1,0,1,1,0,1,1}, // 5
  {1,0,1,1,1,1,1}, // 6
  {1,1,1,0,0,0,0}, // 7
  {1,1,1,1,1,1,1}, // 8
  {1,1,1,1,0,1,1}  // 9
};

// ===== Pulse =====
volatile unsigned long pulseCount = 0;
volatile bool newPulse = false;

void IRAM_ATTR onPulse() {
  pulseCount++;
  newPulse = true;
}

// ===== แสดง 1 digit =====
void showDigit(int digit, int num) {
  // ปิดทุก digit ก่อน
  for(int i=0;i<4;i++) digitalWrite(digitPins[i], HIGH);

  // set segment
  for(int i=0;i<7;i++) {
    digitalWrite(segPins[i], numbers[num][i]);
  }

  // เปิด digit ที่ต้องการ
  digitalWrite(digitPins[digit], LOW);
}

// ===== แสดงเลข 4 หลัก =====
void displayNumber(int num) {
  int d[4];
  d[0] = (num / 1000) % 10;
  d[1] = (num / 100) % 10;
  d[2] = (num / 10) % 10;
  d[3] = num % 10;

  for(int i=0;i<4;i++) {
    showDigit(i, d[i]);
    delay(2); // multiplex speed
  }
}

unsigned long lastPulseTime = 0;
bool sessionActive = false;

void setup() {
  Serial.begin(115200);

  pinMode(pulsePin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pulsePin), onPulse, FALLING);

  // setup digit pins
  for(int i=0;i<4;i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], HIGH);
  }

  // setup segment pins
  for(int i=0;i<7;i++) {
    pinMode(segPins[i], OUTPUT);
  }

  Serial.println("RX Ready");
}

void loop() {
  if (newPulse) {
    newPulse = false;
    lastPulseTime = millis();
    sessionActive = true;

    Serial.println(pulseCount);
  }

  // แสดงตลอดเวลา (สำคัญมาก!)
  displayNumber(pulseCount);

  // จบ session
  if (sessionActive && (millis() - lastPulseTime) > 500) {
    Serial.println("---- Done: " + String(pulseCount) + " ----");

    delay(20);
    pulseCount = 0;
    sessionActive = false;
  }
}