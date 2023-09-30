		AREA CODE, CODE, READONLY
		CODE16


;void suFreeMem( void* ptr );
		EXPORT suFreeMem
suFreeMem
		NOP	; be cursed the scatter! >_<
		; check if we trying to free a NULL
		CMP		R0, #0
		BEQ		_null_param

		; normal case
		PUSH    {R4-R5,LR}
		SUB		SP, #4

		SUB		R4, R0, #4 			; get the ptr to the block header (contains part_id)

		; disable interrupts
		BL		suDisableInt
		MOV		R5, R0

		; release the block
		ADD		R2, SP, #0			; *err
		MOV		R1, R4				; block_ptr
		LDR		R0, [R1]			; part_id
		BL		sc_rblock			; void sc_rblock(int part_id, char *block_ptr, int *errp)

		; check errors
		LDR		R0, [SP, #0]		; *err
		CMP		R0, #0
		BNE		_exit				; should be panic actually...

		; restore the signature
		LDR		R0, =0xDEAD
		STR		R0, [R4]
		
_exit
		; enable interrupts
		MOV		R0, R5
		BL		suSetInt
		
		ADD		SP, #4
		POP		{R4-R5}
		POP		{R3}
		BX		R3

_null_param
		BX		LR

		LTORG
		
		CODE32
; --------------- S U B R O U T I N E ---------------------------------------
		EXPORT suSetInt
suSetInt
		MRS     R1, CPSR
		BIC     R2, R1, #0xC0
		AND     R0, R0, #0xC0
		ORR     R2, R2, R0
		MSR     CPSR_c, R2
		MOV     R0, R1
		BX      LR
; End of function suSetInt

; --------------- S U B R O U T I N E ---------------------------------------
		EXPORT suDisableInt
suDisableInt
		MRS     R1, CPSR
		MOV     R0, R1
		MOV     R0, R1
		ORR     R1, R1, #0x80
		MSR     CPSR_c, R1
		BX      LR
; End of function suDisableInt

; --------------- S U B R O U T I N E ---------------------------------------
		EXPORT suDisableAllInt
suDisableAllInt
		MRS     R0, CPSR
		ORR     R1, R0, #0xC0
		MSR     CPSR_c, R1
		BX      LR
; End of function suDisableAllInt
		LTORG

		IMPORT sc_rblock

		END