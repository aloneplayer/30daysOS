[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	EXTERN	_write_mem8
	EXTERN	_io_hlt
[FILE "bootpack.c"]
[SECTION .text]
	GLOBAL	_HariMain
_HariMain:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	MOV	EBX,655360
L6:
	PUSH	EBX
	PUSH	EBX
	INC	EBX
	CALL	_write_mem8
	CMP	EBX,720894
	POP	EAX
	POP	EDX
	JLE	L6
L7:
	CALL	_io_hlt
	JMP	L7
