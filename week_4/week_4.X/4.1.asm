.include "m328PBdef.inc" 

.equ PD0 = 0
.equ PD1 = 1
.equ PD2 = 2
.equ PD3 = 3
.equ PD4 = 4
.equ PD5 = 5
.equ PD6 = 6
.equ PD7 = 7
    
.def ADC_L = r21
.def ADC_H = r22
.def voltage = r18
    
.org 0x00
    rjmp setup
    
.org 0x2A
    rjmp ADC_INT

setup:  
    ldi r24, low(RAMEND)
    out SPL, r24
    ldi r24, high(RAMEND)
    out SPH, r24					    ; init stack pointer

    ldi voltage, 5    
    
    sei     
   
    ser r24
    out DDRD, r24					    ; set PORTD as output
    clr r24
    rcall lcd_init
    ldi r24, low(100)
    ldi r25, high(100)					    ; delay 100 mS
    rcall wait_msec
    rcall lcd_clear_display
    
    ldi r17, 0b01000001					    ;right adjusted
    sts ADMUX, r17
    ldi r17, 0b10001111
    sts ADCSRA, r17

  
    
    
main:
    ldi voltage, 5
    rcall lcd_clear_display
    lds r17, ADCSRA
    ori r17,(1<<ADSC)
    sts ADCSRA, r17					    ; start conversion

    MUL ADC_L, voltage					    ;multiply 8 lsb with voltage
    MOV R30, R0
    MOV R31, R1
    
    ;we want to add 8 zeros(multiply by 2^8) to the high bit after we multiply it by the voltage
    MUL ADC_H, voltage  
    add r31, r0
    
   ; r31-r30 hold the wanted value 
        
    mov r29, r31						    ; We take the first 6 bits(integer part)
    lsr r29
    lsr r29							    ;int part saved in r29
    
    
    ;multiply lower 10 bits by 100 and divide by 1024 to get 2 fractional digits precision
    ;first multiply low bit
    ldi r16, 100
    mul r30, r16
    mov r28, r1							    ;keep high bit of multiplication(it's like we divided by 2^8)
    andi r31, 0b00000011					    ;keep last 2 bits(2 MSBS of fractional parts)
    mul r31, r16    
    clc
    ror r1
    ror r0
    ror r1
    ror r0
    clc
    lsr r28
    lsr r28
    clc
    add r28, r0							    ;keep low byte of multiplication
		    
    ;fraction part saved in r28
    
    fraction_convert:
    ;convert to bcd and load integer part to display
    mov r24, r29
    rcall bcd_conversion
    mov r24, r26
    call load_to_display
       ;load '.' to display
    ldi r24, '.'
    call lcd_data
    ;load fraction part to r24
    mov r24, r28
    rcall bcd_conversion
        ;load tens bcd to display
    mov r24, r17
    rcall load_to_display
    ;load units bcd to display
    mov r24, r26
    rcall load_to_display



    ldi r24, low(1000)
    ldi r25, high(1000)
    rcall wait_msec
    rjmp main
    
    
    
   
ADC_INT:
    lds ADC_L, ADCL
    lds ADC_H, ADCH
    out PORTD, ADC_H
    reti
    

    
bcd_conversion:
    clr R17              ; Clear R17 for tens
    clr r26             ; Clear R18 for units

loop:
    cpi r24, 10
    brlo done            ; If less than 10, branch to done

    ; Subtract 10 from R16 and increment R17 (tens digit)
    subi R24, 10
    inc R17
    rjmp loop   ; Repeat until R16 is less than 10

done:
    ; Now R16 contains the units (0-9), so copy it to R18
    mov r26, R24

    ret
    
load_to_display:
    ;take input from r24 and load to display
    cpi r24, 0
    brne one
    ldi r24, '0'
    call lcd_data
    rjmp end
one: 
    cpi r24, 1
    brne two
    ldi r24, '1'
    call lcd_data
    rjmp end
two: 
    cpi r24, 2
    brne three
    ldi r24, '2'
    call lcd_data
    rjmp end
three: 
    cpi r24, 3
    brne four
    ldi r24, '3'
    call lcd_data
    rjmp end
four: 
    cpi r24, 4
    brne five
    ldi r24, '4'
    call lcd_data
    rjmp end
five: 
    cpi r24, 5
    brne six
    ldi r24, '5'
    call lcd_data
    rjmp end
six: 
    cpi r24, 6
    brne seven
    ldi r24, '6'
    call lcd_data
    rjmp end
seven: 
    cpi r24, 7
    brne eight
    ldi r24, '7'
    call lcd_data
    rjmp end
eight: 
    cpi r24, 8
    brne nine
    ldi r24, '8'
    call lcd_data
    rjmp end
nine: 
    ldi r24, '9'
    call lcd_data
    
end:
    ret
    
      
lcd_init:
    ldi r24 ,low(200) ;
    ldi r25 ,high(200)				    ; Wait 200 mSec
    rcall wait_msec 
    ldi r24 ,0x30					    ; command to switch to 8 bit mode
    out PORTD ,r24 ;
    sbi PORTD ,PD3					    ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    ldi r24 ,250 
    ldi r25 ,0					    ; Wait 250uSec
    rcall wait_usec 
    ldi r24 ,0x30					    ; command to switch to 8 bit mode
    out PORTD ,r24 
    sbi PORTD ,PD3					    ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    ldi r24, 250 
    ldi r25, 0					    ; Wait 250uSec
    rcall wait_usec 
    ldi r24, 0x30					    ; command to switch to 8 bit mode
    out PORTD ,r24 
    sbi PORTD ,PD3					    ; Enable Pulse
    nop
    nop
    cbi PORTD, PD3
    ldi r24, 250 
    ldi r25, 0					    ; Wait 250uSec
    rcall wait_usec
    ldi r24 ,0x20					    ; command to switch to 4 bit mode
    out PORTD ,r24
    sbi PORTD ,PD3					    ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    ldi r24, 250 
    ldi r25, 0					    ; Wait 250uSec
    rcall wait_usec
    ldi r24, 0x28					    ; 5x8 dots, 2 lines
    rcall lcd_command
    ldi r24, 0x0c					    ; dislay on, cursor off
    rcall lcd_command
    rcall lcd_clear_display
    ldi r24, 0x06					    ; Increase address, no display shift
    rcall lcd_command ;
    ret
    
lcd_clear_display:
    ldi r24, 0x01				    ; clear display command
    rcall lcd_command
    ldi r24, low(5) 
    ldi r25, high(5)				    ; Wait 5 mSec
    rcall wait_msec 
    ret
    
lcd_command:
    cbi PORTD, PD2				    ; LCD_RS=0(PD2=0), Instruction
    rcall write_2_nibbles			    ; send Instruction
    ldi r24, 250 
    ldi r25, 0					    ; Wait 250uSec
    rcall wait_usec
    ret
    
lcd_data:
    sbi PORTD ,PD2				    ; LCD_RS=1(PD2=1), Data
    rcall write_2_nibbles			    ; send data
    ldi r24 ,250 ;
    ldi r25 ,0					    ; Wait 250uSec
    rcall wait_usec
    ret

write_2_nibbles:
    push r24 ; save r24(LCD_Data)
    in r25 ,PIND ; read PIND
    andi r25 ,0x0f ;
    andi r24 ,0xf0 ; r24[3:0] Holds previus PORTD[3:0]
    add r24 ,r25 ; r24[7:4] <-- LCD_Data_High_Byte
    out PORTD ,r24 ;
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    pop r24 ; Recover r24(LCD_Data)
    swap r24 ;
    andi r24 ,0xf0 ; r24[3:0] Holds previus PORTD[3:0]
    add r24 ,r25 ; r24[7:4] <-- LCD_Data_Low_Byte
    out PORTD ,r24
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    ret    
   
    
    
    
wait_msec:
    push r24 ; 2 cycles
    push r25 ; 2 cycles
    ldi r24 , low(999) ; 1 cycle
    ldi r25 , high(999) ; 1 cycle
    rcall wait_usec ; 998.375 usec
    pop r25 ; 2 cycles
    pop r24 ; 2 cycles
    nop ; 1 cycle
    nop ; 1 cycle
    sbiw r24 , 1 ; 2 cycles
    brne wait_msec ; 1 or 2 cycles
    ret ; 4 cycles
wait_usec:
    sbiw r24 ,1 ; 2 cycles (2/16 usec)
    call delay_8cycles			; 4+8=12 cycles
    brne wait_usec				; 1 or 2 cycles
    ret
delay_8cycles:
    nop
    nop
    nop
    ret