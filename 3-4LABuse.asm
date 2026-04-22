; TxBoard MCS-51
; P1.0=A(100p,12ms) P1.1=B(300p,6ms) P1.2=C(500p,3ms) P1.3=D(700p,1ms)
; Output pulse on P3.4

DLY_VAL   EQU 30H
NP_H      EQU 32H
NP_L      EQU 33H
ACTIVE    EQU 34H

  ORG 0000H
  JMP 0100H

  ORG 0100H
  MOV SP,#3FH
  CLR EA
  MOV ACTIVE,#0
  MOV P1,#0FFH
  CLR P3.4

MAIN_LOOP:
  MOV A,ACTIVE
  JNZ SEND_PULSE

  MOV A,P1
  ANL A,#0FH
  CJNE A,#0FH,CHK_SW
  JMP MAIN_LOOP

CHK_SW:
  JB P1.0,NOT_A
  MOV DLY_VAL,#12
  MOV NP_H,#00H
  MOV NP_L,#100         ; 100 pulses
  JMP SET_ACTIVE

NOT_A:
  JB P1.1,NOT_B
  MOV DLY_VAL,#6
  MOV NP_H,#01H
  MOV NP_L,#2CH         ; 300 = 0x012C
  JMP SET_ACTIVE

NOT_B:
  JB P1.2,NOT_C
  MOV DLY_VAL,#3
  MOV NP_H,#01H
  MOV NP_L,#0F4H        ; 500 = 0x01F4
  JMP SET_ACTIVE

NOT_C:
  JB P1.3,NOT_D
  MOV DLY_VAL,#1
  MOV NP_H,#02H
  MOV NP_L,#0BCH        ; 700 = 0x02BC
  JMP SET_ACTIVE

NOT_D:
  JMP MAIN_LOOP

SET_ACTIVE:
  MOV ACTIVE,#1
WAIT_REL:
  MOV A,P1
  ANL A,#0FH
  CJNE A,#0FH,WAIT_REL
  JMP MAIN_LOOP

;===================================
SEND_PULSE:
  MOV A,NP_H
  JNZ DO_PULSE
  MOV A,NP_L
  JNZ DO_PULSE
  ; Done - clear active
  MOV ACTIVE,#0
  JMP MAIN_LOOP

DO_PULSE:
  ; Generate pulse
  SETB P3.4
  CALL HALF_MS
  CLR P3.4

  ; Delay (DLY_VAL ms)
  MOV R0,DLY_VAL
DELAY_LOOP:
  CALL HALF_MS
  CALL HALF_MS
  DJNZ R0,DELAY_LOOP

  ; Decrement counter
  CALL DEC16

  JMP MAIN_LOOP

;===================================
; DEC16: ?? NP_H:NP_L ?? 1
;===================================
DEC16:
  MOV A,NP_L
  JNZ DEC16_L
  DEC NP_H
DEC16_L:
  DEC NP_L
  RET

;===================================
; HALF_MS (~500us @ 12MHz)
;===================================
HALF_MS:
  MOV R7,#250
  DJNZ R7,$
  RET

  END