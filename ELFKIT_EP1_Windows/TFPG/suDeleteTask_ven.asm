		AREA CODE, CODE, READONLY
		CODE16


		IMPORT suDeleteTask

		EXPORT suDeleteTask_ven
suDeleteTask_ven
		LDR		R3, =suDeleteTask
		BX		R3

		LTORG

		EXPORT __call_via_r3
__call_via_r3
		BX		R3



		END