.include "m328PBdef.inc"

.def counter = r16				; for delay
.equ freq = 16
.def DC_VALUE = r19


.org 0x00
    rjmp setup

DC_TABLE:
    .db 8, 28, 48, 68, 88, 108, 128, 148, 168, 188, 208, 228, 248, 248

setup:
    clr DC_VALUE
    sei
    ldi r16, (1<<TOIE1)
    sts TIMSK1, r16
    
    ldi r16, (1<<CS10) | (1<<WGM12)		; fpwm = 62500 = 16.000.000/(N * 265) -> N = 1
    sts TCCR1B, r16
    
    ldi r16, (1<<WGM10) | (1<<COM1A1)
    sts TCCR1A, r16

    
    ldi r30, low(DC_TABLE*2)			; Load Z register (ZL) with low byte of array address
    ldi r31, high(DC_TABLE*2)			; Load Z register (ZH) with high byte of array address
    adiw ZL, 1
    
    ser r18
    out DDRB, r18				; Set PORTB as output
    
    
    clr r16
    out DDRD, r16				; PORTD input
    ser r16					; Enable pull-up resistors
    out portd, r16

    
main:  
    in r17, PIND
    sbrs r17, 3					; check if PD3 is pressed
    rjmp increment				; if yes, go to control lights
    sbrs r17, 4
    rjmp decrement
    rjmp main
    
increment:
    cpi DC_VALUE, 248
    breq main
    lpm DC_VALUE, Z				; Load next duty cycle value
    adiw ZL, 1
    sts OCR1AL, DC_VALUE			; Set OCR1A value
    ldi r24, low(100)
    ldi r25, high(100)
    rcall wait_x_ms
    rjmp main
    
decrement:
    cpi DC_VALUE, 8
    breq main
    lpm DC_VALUE, Z
    sbiw ZL, 1
    sts OCR1AL, DC_VALUE
    ldi r24, low(100)
    ldi r25, high(100)
    rcall wait_x_ms
    rjmp main
    

    
    
    

    
    





wait_x_ms:
    ldi counter, freq           ; 16MHz so 16 loops
    rcall delay_1ms             ; 3 + 15.994 cycles
    sbiw r24, 1                 ; 1 cycle
    brne wait_x_ms              ; 1 or 2 cycles
    ret                         ; 4 cycles

delay_1ms:
    rcall delay_inner           ; 3 + 993 cycles
    dec counter                 ; 1 cycle
    brne delay_1ms              ; 1 or 2 cycles
    ret                         ; 4 cycles

delay_inner:
    ldi r23, 247                ; 1 cycle
loop3:
    dec r23                     ; 1 cycle
    nop                         ; 1 cycle
    brne loop3                  ; 1 or 2 cycles
    nop                         ; 1 cycle
    ret                         ; 4 cycles