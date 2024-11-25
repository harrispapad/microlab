.include "m328PBdef.inc" 
    
.org 0x00 
    rjmp setup
    
.org 0x04
    rjmp ISR1

setup:
    clr r20
    ldi r17, LOW(RAMEND)
    out SPL, r17
    ldi r17, HIGH(RAMEND)
    out SPL, r17
    
    .equ delay = 5000			;ms
    .equ freq = 16
    .DEF counter = r17
    ldi r24, low(delay)
    ldi r25, high(delay)
    
    
    .def status = r28			; to be used as a flag
    clr status
    
    ser r26
    out DDRB, r26
    
    ldi r17, (1<<ISC11) | (1<<ISC10)
    sts EICRA, r17
    
    ldi r17, (1<<INT1)
    out EIMSK, r17
    
    sei
    
main:
    clr r19
    out PORTB, r19
    cpi status, 0xFF			; check if we're coming from a "simple" interrupt
    breq lights_on
    cpi status, 0x0F			; check if we're coming from a refresh interrupt
    breq refresh
    rjmp main

lights_on:
    ldi r19, 1
    out PORTB, r19
    ldi r24, low(5000)
    ldi r25, high(5000)
    rcall wait_delay_ms
    cpi status, 0x0F			; check if we're coming from a refresh interrupt
    breq refresh
    clr r19
    out PORTB, r19
    clr status
    rjmp main
    
refresh:
    ldi r19, 0b00001111			
    out PORTB, r19
    ldi r24, low(500)
    ldi r25, high(500)
    rcall wait_delay_ms
    ser status
    rjmp lights_on
 
    
ISR1:
    push r17
    push r22
    in r22, SREG
    push r22
    
    spark:
    ldi r17, (1<<INTF1)
    out EIFR, r17
    
    rcall short_delay
    
    in r17, EIFR
    cpi r17, 0b00000010
    brne skip
    rjmp spark
    
    skip:
    cpi status, 0
    brne refresh_int
    ser status
    rjmp int_end
    
    refresh_int:
    ldi r24, 1
    clr r25
    ldi status, 0x0F
    
    int_end:
    pop r22
    out SREG, r22
    pop r22
    pop r17
    
    reti
 
    
long_delay:
    ldi r24,low(5000)
    ldi r25, high(5000)
    rcall wait_delay_ms
    ret
    
short_delay:
    ldi r24, low(5)
    ldi r25, high(5)
    rcall wait_delay_ms
    ret
    
wait_delay_ms:
    ldi counter, freq	    ; 16MHz so 16 loops
    rcall delay_1ms	    ; 3 + 15.994 cycles
    sbiw r24, 1		    ; 1 cycle
    brne wait_delay_ms	    ; 1 or 2 cycles
    ret			    ; 4 cycles
 
; 16.000 cycles ~ 1ms
delay_1ms:
    rcall delay_inner	    ; 3 + 993 cycles
    dec counter		    ; 1 cycle
    brne delay_1ms	    ; 1 or 2 cycles
    ret			    ; 4 cycles
    
; 993 cycles ~ 1/16 ms  
delay_inner:
    ldi r23, 247	    ; 1 cycle
loop3:
    dec r23		    ; 1 cycle
    nop			    ; 1 cycle
    brne loop3		    ; 1 or 2 cycles
    nop			    ; 1 cycle
    ret			    ; 4 cycles