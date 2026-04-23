from machine import Pin
import time

# --- การตั้งค่า Pin ---
# Pin Mapping: a, b, c, d, e, f, g, dp (เรียงลำดับตามดัชนี 0-7)
seg_pins_numbers = [17, 5, 18, 19, 21, 22, 23, 15]
# Position Pins: Dig1, Dig2, Dig3, Dig4
pos_pins_numbers = [13, 12, 14, 27]

# เริ่มต้นกำหนด Pin เป็น Output
segments = [Pin(p, Pin.OUT) for p in seg_pins_numbers]
positions = [Pin(p, Pin.OUT) for p in pos_pins_numbers]

# --- ข้อมูลรูปแบบตัวเลข (Bit Mask) ---
# รูปแบบ 0b(dp)(g)(f)(e)(d)(c)(b)(a)
digit_patterns = [
    0b00111111, # 0
    0b00000110, # 1
    0b01011011, # 2
    0b01001111, # 3
    0b01100110, # 4
    0b01101101, # 5
    0b01111101, # 6
    0b00000111, # 7
    0b01111111, # 8
    0b01101111  # 9
]

def display_digit(digit, pos_index):
    """ แสดงตัวเลขเฉพาะหลักที่กำหนด """
    pattern = digit_patterns[digit]
    
    # 1. ปิดทุกหลักก่อน (เพื่อป้องกันเงาตัวเลข หรือ Ghosting)
    for p in positions:
        p.value(0) # สำหรับ Common Anode เปลี่ยนเป็น 1

    # 2. ตั้งค่า Segment (a-g และ dp)
    for i in range(8):
        bit_value = (pattern >> i) & 0x01
        # ถ้าหน้าจอเป็น Common Anode ใช้ not bit_value (0=ติด, 1=ดับ)
        # ถ้าหน้าจอเป็น Common Cathode ใช้ bit_value (1=ติด, 0=ดับ)
        segments[i].value(not bit_value) 

    # 3. เปิดเฉพาะหลักที่ต้องการแสดงผล
    positions[pos_index].value(1) # สำหรับ Common Anode เปลี่ยนเป็น 0
    time.sleep_ms(2) # หน่วงเวลาสั้นๆ เพื่อให้สว่างพอดี
    
    # 4. ปิดหลักนี้เมื่อจบฟังก์ชัน (Multiplexing)
    positions[pos_index].value(0)

def display_4_digit(number):
    """ แยกหลักตัวเลข 0-9999 และวนแสดงผลทีละหลัก """
    number %= 10000
    
    # ดึงค่าแต่ละหลัก (พัน, ร้อย, สิบ, หน่วย)
    d_thousands = number // 1000
    d_hundreds = (number // 100) % 10
    d_tens = (number // 10) % 10
    d_ones = number % 10
    
    # วนแสดงผลทีละหลักอย่างรวดเร็ว (Persistence of Vision)
    display_digit(d_thousands, 0) # Dig 1
    display_digit(d_hundreds, 1)  # Dig 2
    display_digit(d_tens, 2)      # Dig 3
    display_digit(d_ones, 3)      # Dig 4

# --- Main Loop ---
counter = 5678

while True:
    # เรียกฟังก์ชันแสดงผลวนไปเรื่อยๆ
    display_4_digit(counter)