
		AREA CODE, CODE, READONLY
		CODE16


;void *suAllocMem( size_t size, INT32* err );
		EXPORT suAllocMem
suAllocMem
		PUSH    {R1,R4-R5,LR}
		SUB		SP, #4

		LDR     R4, =vmc_pcb_glb	; partitions table
		MOV		R5, #0				; loop counter (part_id)
		
		ADD		R0, #4				; req_size+header

		; find the partition with appropriate block size
_loop
		MOV		R2, #0x18			;sizeof(pentry)
		MUL		R2, R5				;R2 = offset in the pcb

		ADD		R2, R2, R4			;partition entry address
		LDR		R2, [R2, #4]		;partition's block size

		CMP		R2, R0
		BLO		_loop_continue		;if( block_size < reqested_size ) continue;

		; get the block from the partition
		; gblock returns a free block with header:
		; DCW	??
		; DCW	signature = 0xDEAD
		; DCD	next	- next freee block in linked list

		ADD		R1, SP, #0			; *err
		MOV		R0, R5				;
		BL		sc_gblock			; char *sc_gblock(int part_id, int *errp);
		MOV		R3, R0				; block address

		LDR		R0, [SP, #0]		; check sc_gblock errors
		CMP		R0, #3
		BEQ		_loop_continue		; if we got out_of_memory error, try to allocate bigger block
				;else
		CMP		R0, #0
		BNE		_save_error			; return with error, returned by sc_gblock

		LDRH	R0, [R3, #2]		; check signature validity
		LDR		R1, =0xDEAD
		CMP		R0, R1
		BNE		_proceed_with_error

		MOV		R0, #0				; no error code
		B		_save_error

_loop_continue
		ADD		R5, #1
		CMP		R5, #PCOUNT			; number of partitions
		BLO		_loop


_proceed_with_error
		MOV		R0, #2				; proceed with error if we got here (after the loop or branch)

_save_error		;R0 should be set to the errcode
		LDR		R1, [SP, #4]
		CMP		R1, #0
		BEQ		_check_error		; should consider making a panic tho...
		STR		R0, [R1]

_check_error
		CMP		R0, #0
		BNE		_exit_fail			;if there was a error

		; save the part_id the block was in to be able to free it later
		STR		R5, [R3]
		;MOV		R0, #0 ;already done
		STR		R0, [R3, #4]
		; return the block ptr+4
		ADD		R0, R3, #4
		B		_exit

_exit_fail
		;return NULL
		MOV		R0, #0

_exit
		ADD		SP, #8
		POP     {R4-R5}
		POP     {R3}
		BX      R3

		LTORG
		
		GET		equs.inc
		
		IMPORT	sc_gblock
		IMPORT	vmc_pcb_glb

		END
