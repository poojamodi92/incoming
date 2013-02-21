			.ORIG X3000
			LEA R1, PTR1 	 ; LOAD THE ADDRESS OF THE STARTING ADDRESS
			LEA R2, PTR2  	; 
			LDW R1,R1,#0  	; R1 <-x4000
			LDW R1,R1,#0  	; R1 <- starting address of the first element in the character string
			LDW R2,R2,#0  	; R2 <- X4002
			LDW R2,R2,#0  	; R2 <- starting address of where to store the null-terminated upper-case character string
			AND R3,R3,#0  	; R3 -> COUNTER
			AND R4, R4, #0 	; R4 -> Reference check
Continue	LDB R5,R1,#0	; Load each of the characters
			ADD R3,R3,#1	; Increment counter
			ADD R5,R5,R4	; Check if it is null
			BRz DoneCounting	; If it is null, done counting
			ADD R1,R1,#1	; Get next address
			BR Continue		; Continue the loop	
DoneCounting	LEA R1, PTR1 ; LOAD THE ADDRESS OF THE STARTING ADDRESS
	 		LDW R1,R1,#0  	; R1 <-x4000
			LDW R1,R1,#0 	; R1 <- starting address of the first element in the character string
			NOT R6,R2       ;Subtract the address from R2 and decide where they stand
			ADD R6,R6,#1
			ADD R6, R6, R1
			BRn backwards   ; if dest address > source address, start from the back
normal		LDB R5,R1,#0	; Load the first charracter
			ADD R5,R5,#-16  ; Convert to upper case by subtracting
            ADD R5,R5,#-16
			STB R5,R2,#0	; Store it at the address of the destination
			ADD R1,R1,#1	; Increment to next source address
			ADD R2,R2,#1	; Increment to next destination address
			ADD R4,R4,#1	; Increment counter
			NOT R6,R4		; Check if all the characters have been counted 
			ADD R6,R6,#1
			ADD R6,R6,R3
			BRz Changed
			BR normal 		; If not, go back up
backwards	ADD R6,R3,#-1	
			ADD R2,R2,R6	; destination address[backward] = destination address[forward] + count (register R3) - 1
			ADD R1,R1,R6	; source address[backward] = source address[forward] + count (register R3) - 1

Changing	LDB R5,R1,#0	;Load the first charracter

			ADD R5,R5,#-16	;Convert to upper case by subtracting
            ADD R5,R5,#-16
			STB R5,R2,#0	;Store it at the address of the destination

			ADD R1,R1,#-1	; Decrement to next source address
			ADD R2,R2,#-1	; Decrement to next destination address
			ADD R4,R4,#1	;  Increment counter
			NOT R6,R4		; Check if all the characters have been counted
			ADD R6,R6,#1
			ADD R6,R6,R3
			BRz Changed
			BR Changing 	; If not, go back up 
		
PTR1		.FILL x4000
PTR2		.FILL x4002
Changed		.end


			
