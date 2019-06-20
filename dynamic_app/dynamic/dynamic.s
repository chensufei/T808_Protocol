 
 AREA |DYNAMIC_ENGINE_AS|, CODE, READONLY
 CODE32

 IMPORT |Image$$ZI$$Base| 
 IMPORT |Image$$ZI$$Limit|
 IMPORT dynamic_main 
 
 EXPORT dynamic_start
 
dynamic_start
	STMFD   r13!, {r14, r4-r6}
	
	LDR		r5, =|Image$$ZI$$Base|
	LDR		r6, =|Image$$ZI$$Limit|
	BL		dynamic_zi_init

	BLX 	dynamic_main
	
	LDMFD   r13!, {r14, r4-r6}
	BX 		r14

dynamic_zi_init
	MOV		r4, #0
	CMP		r5, r6
	STRLO	r4, [r5], #4
	BLO		dynamic_zi_init
	MOV		pc, lr
 END


