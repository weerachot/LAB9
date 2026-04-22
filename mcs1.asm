;==== Lab8_Q1_MCS51_1493Hz.Asm ====
; Crystal 11.0592 MHz, 1MC = 1.085 us
; Freq = 1493 Hz, Period = 669.8 us
; T_ON  = 502.3 us = 463 MC, reload = 65536-463 = 0xFE31
; T_OFF = 167.4 us = 154 MC, reload = 65536-154 = 0xFF66
        ORG     0000H
        JMP     0100H

        ORG     0100H
        MOV     SP,#3FH
        CLR     EA
        MOV     TMOD,#00010001B   ; Timer1 Mode1 16-bit

T_OFF:  CLR     TR1
        MOV     P2,#00H           ; Output LOW
        MOV     DPTR,#-154+13     ; reload T_OFF + overhead
        MOV     TH1,DPH
        MOV     TL1,DPL
        CLR     TF1
        SETB    TR1
        JNB     TF1,$

T_ON:   CLR     TR1
        MOV     P2,#0FFH          ; Output HIGH
        MOV     DPTR,#-463+15     ; reload T_ON + overhead
        MOV     TH1,DPH
        MOV     TL1,DPL
        CLR     TF1
        SETB    TR1
        JNB     TF1,$

        JMP     T_OFF

	END