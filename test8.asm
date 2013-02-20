.ORIG x3000              ; 0x3000     error        x         0
;ADD R3, R1, R0         ; 0X1640     0x3000       x         0
;ADD R3, R1, x0         ; 0X1660     0x3000       x         0
;ADD R6, R2, #15        ; 0X1CAF     0x3000       x         0
;ADD R3, R1, #-3 ; test ; 0X167D     0x3000       x         0
;ADD R0, R1, #-16       ; 0X1070     0x3000       x         0
;AND R6, R6, R6         ; 0X5D86     0x3000       x         0
;AND R4, R5, #1         ; 0X5961     0x3000       x         0
;AND R1, R2, x-10       ; 0X52B0     0x3000       x         0
;XOR R2, R3, R6         ; 0X94C6     0x3000       x         0
;XOR R4, R1, #1         ; 0X9861     0x3000       x         0
;XOR R0, R2, x-10       ; 0X90B0     0x3000       x         0
;LSHF R1, R2, #15       ; 0XD28F     0x3000       x         0
;LSHF R2, R2, xA        ; 0XD48A     0x3000       x         0
;RSHFL R2, R3, xF       ; 0XD4DF     0x3000       x         0
;RSHFL R4, R5, x1       ; 0XD951     0x3000       x         0
;RSHFA R4, R5, #15      ; 0XD97F     0x3000       x         0
;RSHFA R4, R5, x0       ; 0XD970     0x3000       x         0
;label BR  label        ; 0X0FFF     error        x         0
;label BRn label        ; 0X09FF     error        x         0
label BRnzp label      ; 0X0FFF     error        x         0
;label BRnp label       ; 0X0BFF     error        x         0
;label BRzp label       ; 0x07FF     error        x         0
;label JSR label        ; 0X4FFF     error        x         0
;JMP  R2 ; test         ; 0XC080     0x3000       x         0
;JMP  R0                ; 0XC000     0x3000       x         0
;JSRR R1 ;te;st         ; 0X4040     0x3000       x         0
;JSRR R7                ; 0X41C0     0x3000       x         0
;label LDB R0, R1, #-10 ; 0X2076     error        x         0
;LDB R2, R3, #31;       ; 0X24DF     0x3000       x         0
;LDB R3, R4, x-1F       ; 0X2721     0x3000       x         0
;LDW R4, R5, #1          ;0X6941     0x3000       x         0
;LDW R5, R6, #31         ;0X6B9F     0x3000       x         0
;LDW R6, R1, x-1F        ;0X6C61     0x3000       x         0
;STB R1, R2, #1          ;0X3281     0x3000       x         0
;STB R2, R3, #31         ;0X34DF     0x3000       x         0
;STB R3, R4, x-1F        ;0X3721     0x3000       x         0
;STW R4, R5, #1          ;0X7941     0x3000       x         0
;STW R5, R6, #31         ;0X7B9F     0x3000       x         0
;STW R6, R7, x-1F        ;0X7DE1     0x3000       x         0
;label LEA R7, label     ;0XEFFF     error        x         0
;label LEA R1, label     ;0xE3FF     error        x         0
;NOT R0, R1              ;0X907F     0x3000       x         0
;NOT R7, R7              ;0x9FFF     0x3000       x         0
;RET                     ;0XC1C0     0x3000       x         0
;RTI                     ;0X8000     0x3000       x         0
;NOP                     ;0X0000     0x3000       x         0
;TRAP x25                ;0XF025     0x3000       x         0
;TRAP x22                ;0XF022     0x3000       x         0
;HALT                    ;0XF025     0x3000       x         0
;HALT ; comment          ;0XF025     0x3000       x         0
;.FILL x25               ;0X0025     0x3000       x         0
;.FILL #3000             ;0X0BB8     0x3000       x         0
;.FILL x-1               ;0xFFFF     0x3000       x         0
;.FILL #25000            ;0x61A8     0x3000       x         0
.END

