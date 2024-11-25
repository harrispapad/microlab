.include "m328PBdef.inc"

.DEF A = r16
.DEF B = r17
.DEF C = r18
.DEF D = r19
.DEF F0 = r20
.DEF F1 = r21
.DEF temp = r22
.DEF counter = r23
    
setup:
    ldi A, 0x51
    ldi r24, 0x01
    ldi B, 0x41
    ldi r25, 0x02
    ldi C, 0x21
    ldi r26, 0x03
    ldi D, 0x01
    ldi r27, 0x04
    ldi counter, 0x06
    
main:
;F0
    push A
    push B
    push C
    push D
    COM B	;B = B'
    AND A, B	;A = A AND B'
    AND D, B	;D = D AND B'
    OR A, D	;A = (A AND B') OR (D AND B')
    COM A
    MOV F0, A

;F1
    pop D
    pop C
    pop B
    pop A
    
    push A
    push B
    push C
    push D
    COM C
    OR A, C	;A = A OR C'
    COM D
    OR B, D	;B = B OR D'
    AND A, B
    MOV F1, A

    pop D
    pop C
    pop B
    pop A
    
    ADD A, r24
    ADD B, r25
    ADD C, r26
    ADD D, r27
    DEC counter
    BRNE main


