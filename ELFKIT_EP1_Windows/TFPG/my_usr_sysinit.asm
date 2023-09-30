				AREA CODE, CODE, READONLY
				CODE16

				LTORG

				EXPORT vmc_usr_sysinit
vmc_usr_sysinit ROUT

				PUSH    {R1-R7,LR}

				;LDR     R4, =su_stacklocations_glb+4
				;LDR     R5, =su_stacksizes_glb+4
				ADR     R6, vmc_task
				MOV     R7, #0

0vmc_usr_sysinit
				STR     R7, [SP,#0]
				STR     R7, [SP,#4]
				LDMIA   R6!, {R0-R3}
				CMP     R3, #0
				BEQ     %1vmc_usr_sysinit
				;STR     R1, [R4]
				;STR     R2, [R5]
				;ADD     R4, R4, #4
				;ADD     R5, R5, #4

				BL      vmc_tcreate
				CMP     R0, #0
				BNE     %6vmc_usr_sysinit
				B       %0vmc_usr_sysinit ; task_loop
1vmc_usr_sysinit ; task_done

				ADR     R4, vmc_part
2vmc_usr_sysinit ; vmc_part_loop
				LDMIA   R4!, {R0-R3}
				CMP     R3, #0
				BEQ     %3vmc_usr_sysinit
				BL      vmc_pcreate
				CMP     R0, #0
				BNE     %6vmc_usr_sysinit
				B       %2vmc_usr_sysinit
3vmc_usr_sysinit


				MOV     R4, #0

4vmc_usr_sysinit
				MOV     R0, #0
				BL      vmc_fcreate
				CMP     R0, #0
				BNE     %6vmc_usr_sysinit
				ADD     R4, #1
				CMP     R4, #FCOUNT
				BLT     %4vmc_usr_sysinit
				MOV     R4, #0


5vmc_usr_sysinit
				BL      vmc_vtcreate
				CMP     R0, #0
				BNE     %6vmc_usr_sysinit
				ADD     R4, #1
				CMP     R4, #VTCOUNT
				BGE     %7vmc_usr_sysinit
				B       %5vmc_usr_sysinit

6vmc_usr_sysinit
				MOV     R0, #0x11

7vmc_usr_sysinit
				POP     {R1-R7}
				POP     {R3}
				BX      R3
; End of function vmc_usr_sysinit

				LTORG

				GET		equs.inc
				GET		tables.inc

				IMPORT	vmc_tcreate
				IMPORT	vmc_pcreate
				IMPORT	vmc_fcreate
				IMPORT	vmc_vtcreate

				END
