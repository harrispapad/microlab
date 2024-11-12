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
    
    .equ delay = 200			;ms
    .equ freq = 16
    .DEF delay_counter = r17
    
    .DEF int_counter = r16		; Interrupt Counter
    clr int_counter
    
    ser r26				
    out DDRB, r26			; PORTB output
    
    ser r26
    out DDRC, r26			; PORTC input
    
    clr r26
    out DDRD, r26			; PORTD input
    ser r26				; Enable pull-up resistors
    out portd, r26
    
    ldi r17, (1<<ISC11) | (1<<ISC10)	; Enable INT1 on rising edge
    sts EICRA, r17
    
    ldi r17, (1<<INT1)
    out EIMSK, r17
    
    sei
    
main:
    in r17, PIND			
    sbrs r17, 5				; Check if PD5 is pressed
    rjmp disable_int			; If yes, diable interrupts
    rcall enable_int
 
continue:
    out PORTB, r20
    
    rcall long_delay			; 500ms
    
    inc r20
    
    sbrc r20, 4
    clr r20
    rjmp main
    
    
ISR1:
    push r24
    push r25
    in r24, SREG
    push r24
    

    ldi r17, (1<<INTF1)
    out EIFR, r17
    
    rcall short_delay
    
    
    in r17, EIFR
    cpi r17, 0b00000010
    brne skip
    pop r24
    out SREG, r24
    pop r25
    pop r24
    rjmp ISR1

    skip:
    inc int_counter
    SBRC int_counter, 6			; Check for overflow
    clr int_counter
    out PORTC, int_counter
    
    pop r24
    out SREG, r24
    pop r25
    pop r24
    
    reti

 
 disable_int:
    cli
    ldi r27, (0<<INT1)
    out EIMSK, r27
    
    ldi r17, (1<<INTF1)
    out EIFR, r17
    
    rjmp continue
    
enable_int:
    ldi r17, (1<<ISC11) | (1<<ISC10)
    sts EICRA, r17
    
    ldi r17, (1<<INT1)
    out EIMSK, r17
    
    ldi r17, (0<<INTF1)
    out EIFR, r17
    
    sei
    
    ret   
  
    
short_delay:
    ldi r24, low(5)
    ldi r25, high(5)
    rcall wait_delay_ms
    ret
    
long_delay:
    ldi r24, low(500)
    ldi r25, high(500)
    rcall wait_delay_ms
    ret
    
wait_delay_ms:
    ldi delay_counter, freq	    ; 16MHz so 16 loops
    rcall delay_1ms		    ; 3 + 15.994 cycles
    sbiw r24, 1			    ; 1 cycle
    brne wait_delay_ms		    ; 1 or 2 cycles
    ret				    ; 4 cycles
 
; 16.000 cycles ~ 1ms
delay_1ms:
    rcall delay_inner		    ; 3 + 993 cycles
    dec delay_counter		    ; 1 cycle
    brne delay_1ms		    ; 1 or 2 cycles
    ret				    ; 4 cycles
    
; 993 cycles ~ 1/16 ms  
delay_inner:
    ldi r23, 247		    ; 1 cycle
loop3:
    dec r23			    ; 1 cycle
    nop				    ; 1 cycle
    brne loop3			    ; 1 or 2 cycles
    nop				    ; 1 cycle
    ret				    ; 4 cycles