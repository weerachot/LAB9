const int segmentPins[] = { 17, 5, 18, 19, 21, 22, 23, 15 };
const int positionPins[] = { 13, 12, 14, 27 };
const int BTN1_PIN = 4;
const int BTN2_PIN = 26;

const byte digitPatterns[] = {
  0b00111111, 0b00000110, 0b01011011, 0b01001111,
  0b01100110, 0b01101101, 0b01111101, 0b00000111,
  0b01111111, 0b01101111
};

volatile int Counter = 2493;
// แยกตัวแปร Debounce ออกจากกัน
volatile unsigned long last_add_time = 0;
volatile unsigned long last_sub_time = 0;

// ใช้สำหรับ ESP32 เพื่อป้องกันการเขียนตัวแปรพร้อมกัน
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR isr_add() {
  unsigned long now = millis();
  if (now - last_add_time > 250) {  // เพิ่มหน่วงเวลาเล็กน้อย
    portENTER_CRITICAL_ISR(&mux);
    Counter = (Counter + 2) % 10000;
    portEXIT_CRITICAL_ISR(&mux);
    last_add_time = now;
  }
}

void IRAM_ATTR isr_sub() {
  unsigned long now = millis();
  if (now - last_sub_time > 250) {
    portENTER_CRITICAL_ISR(&mux);
    if (Counter > 0) Counter--;
    else Counter = 9999;
    portEXIT_CRITICAL_ISR(&mux);
    last_sub_time = now;
  }
}

void setup() {
  for (int i = 0; i < 8; i++) pinMode(segmentPins[i], OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(positionPins[i], OUTPUT);
    digitalWrite(positionPins[i], HIGH);  // ดับไฟไว้ก่อน (สำหรับ Common Anode)
  }

  pinMode(BTN1_PIN, INPUT_PULLUP);
  pinMode(BTN2_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BTN1_PIN), isr_add, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN2_PIN), isr_sub, FALLING);
}

void displayDigit(int digit, int pos) {
  // เคลียร์ Segment (Common Anode ส่ง 1 คือดับ)
  for (int i = 0; i < 8; i++) digitalWrite(segmentPins[i], HIGH);

  byte pattern = digitPatterns[digit];
  for (int i = 0; i < 8; i++) {
    // ส่งค่าตาม bit ใน pattern (ถ้า Anode และ bit เป็น 1 ต้องส่ง 0 ให้ไฟติด)
    // ตรงนี้ขึ้นอยู่กับการต่อวงจรของคุณ ถ้าไฟไม่ติด/กลับด้าน ให้ใส่ ! หน้า bitRead
    digitalWrite(segmentPins[i], bitRead(pattern, i));
  }

  digitalWrite(positionPins[pos], LOW);  // เลือกหลัก (Common Anode ดึงลง Ground)
  delayMicroseconds(2000);               // ใช้ delayMicroseconds จะนิ่งกว่า delay(2) ในงาน Multiplex
  digitalWrite(positionPins[pos], HIGH);
}

void loop() {
  int tempCounter;

  // คัดลอกค่ามาใช้เพื่อความปลอดภัย
  portENTER_CRITICAL(&mux);
  tempCounter = Counter;
  portEXIT_CRITICAL(&mux);

  displayDigit((tempCounter / 1000) % 10, 3);
  displayDigit((tempCounter / 100) % 10, 2);
  displayDigit((tempCounter / 10) % 10, 1);
  displayDigit(tempCounter % 10, 0);
}