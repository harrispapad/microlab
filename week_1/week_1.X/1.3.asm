.include "m328PBdef.inc"
    
setup:
    .equ freq = 16
    ; initialize stack pointer
    ldi r24, LOW(RAMEND)
    out SPL, r24
    ldi r24, HIGH(RAMEND)
    out SPH, r24
    ; init portd as output
    ser r26
    out DDRD, r26
    .DEF wagon = r17
    ldi wagon, 0x01
    SET			; start with T = 1 -> go right
    .DEF counter = r16
 
main:
    out PORTD, wagon
    BRTS move_right	; T = 1 -> right
    BRTC move_left
    rjmp end

move_right:
    CPI wagon, 0x80	; 0b10000000 check if far right
    BREQ far_right
    ldi r24, 0x10
    clr r25
    rcall wait_x_ms
    LSL wagon
    jmp end
 
far_right:
    ldi r24, 0x15
    clr r25
    rcall wait_x_ms
    CLT			; set T = 0 -> go left now
    LSR wagon
    jmp end
    
move_left:
    CPI wagon, 0x01
    BREQ far_left
    ldi r24, 0x10
    clr r25
    rcall wait_x_ms
    LSR wagon
    jmp end
    
 far_left:
    ldi r24, 0x15
    clr r25
    rcall wait_x_ms
    SET
    LSL wagon
    jmp end
    

wait_x_ms:
    ldi counter, freq   ; 16MHz so 16 loops
    rcall delay_1ms	; 3 + 15.994 cycles
    sbiw r24, 1		; 1 cycle
    brne wait_x_ms	; 1 or 2 cycles
    ret			; 4 cycles

; 16.000 cycles ~ 1ms
delay_1ms:
    rcall delay_inner	; 3 + 993 cycles
    dec counter		; 1 cycle
    brne delay_1ms	; 1 or 2 cycles
    ret			; 4 cycles
    
; 993 cycles ~ 1/16 ms  
delay_inner:
    ldi r23, 247          ; 1 cycle
loop3:
    dec r23               ; 1 cycle
    nop                   ; 1 cycle
    brne loop3            ; 1 or 2 cycles
    nop					  ; 1 cycle
    ret                   ; 4 cycles
    
end:
    rjmp main