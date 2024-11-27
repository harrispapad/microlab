.include "m328PBdef.inc"          ; ATmega328P microcontroller definitions

.equ FOSC_MHZ=16                 ; Microcontroller operating frequency in MHz
.equ DEL_mS=3000                  ; Delay in mS (valid number from 1 to 4095)
.equ DEL_NU=FOSC_MHZ*DEL_mS      ; delay_mS routine: (1000*DEL_NU+6) cycles

.org 0x00 
    rjmp setup
    
.org 0x02
    rjmp ISR0
    
    
 setup:
    ldi r17, (1<<ISC01) | (1<<ISC00)
    sts EICRA, r17
    
    ldi r17, (1<<INT0)
    out EIMSK, r17
    
    sei
    
; Init Stack Pointer
    ldi r24, LOW(RAMEND)
    out SPL, r24
    ldi r24, HIGH(RAMEND)
    out SPH, r24

; Init PORTC as output
    ser r26
    out DDRC, r26

    clr r26
    out DDRB, r26
    ser r26
    out PORTB, r26
    
    
loop1:
    clr r26

loop2:
    out PORTC, r26

    ldi r24, low(DEL_NU)
    ldi r25, high(DEL_NU)            ; Set delay (number of cycles)
    rcall delay_mS

    inc r26

cpi r26, 32                      ; compare r26 with 16
breq loop1
rjmp loop2


    
ISR0:
    push r24
    push r25
    in r24, SREG
    push r24
    push r26
    
    in r17, PINB
    com r17
    andi r17, 0b00001111
    
    ldi r18, 0b00001111		; auxillary register
    sbrc r17, 0			; whenever we find a pressed button we rotate the aux-reg
    lsl r18			; causing the overflow to bring one extra bit upfront
    sbrc r17, 1
    lsl r18
    sbrc r17, 2
    lsl r18
    sbrc r17, 3
    lsl r18
    
    com r18
    andi r18,0b00001111
    out PORTC, r18
    
    pop r26
    pop r24
    out SREG, r24
    pop r24
    pop r25
    
    reti
    
    
; delay of 1000*F1+6 cycles (almost equal to 1000*F1 cycles)

delay_mS:
; Total delay of next 4 instruction group = 1+(249*4+1) = 996 cycles
    ldi r23, 249                     ; (1 cycle)

loop_inn:
    dec r23                          ; 1 cycle
    nop                              ; 1 cycle
    brne loop_inn                    ; 1 or 2 cycles

    sbiw r24, 1                      ; 2 cycles
    brne delay_mS                    ; 1 or 2 cycles

    ret                               ; 4 cycles