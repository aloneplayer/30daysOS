; naskfunc

[FORMAT "WCOFF"]				;  format of the target file
[BITS 32]						;  create 32-bits code


; information  for target file

[FILE "naskfunc.nas"]			;   name of the source code
		GLOBAL	_io_hlt			;   export function name


; 

[SECTION .text]		            ;   code section

_io_hlt:	; void io_hlt(void);
		HLT
		RET