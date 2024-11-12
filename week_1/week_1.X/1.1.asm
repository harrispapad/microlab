.include "m328PBdef.inc"
	
	ldi r24, LOW(RAMEND)
	out SPL, r24
	ldi r24, HIGH(RAMEND)
	out SPH, r24

setup:
	.equ delay = 10000	    ;ms
	ldi r24, LOW(delay)
	ldi r25, HIGH(delay)
	.equ freq = 16
	.DEF counter = r16
	ldi counter, freq
	; initialize stack pointer
	; init portd as output
	ser r26
	out DDRD, r26
	

main:
    rcall wait_x_ms
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
    nop			  ; 1 cycle
    ret                   ; 4 cycles
    
end:
    ser r17
    out PORTD, r17 
    rjmp setup

