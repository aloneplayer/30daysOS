; naskfunc

[FORMAT "WCOFF"]				;  format of the target file
[INSTRSET "i486p"]              ;  tell compiler to use EXX registers 
[BITS 32]						;  create 32-bits code


; information  for target file

[FILE "naskfunc.nas"]			;   name of the source code
	;   export function name
    GLOBAL	_io_hlt, _write_mem8			


; 

[SECTION .text]		            ;   code section

_io_hlt:	; void io_hlt(void);
		HLT
		RET

_write_mem8:	; void write_mem8(int addr, int data);
		MOV		ECX, [ESP+4]	 ; read data at [ESP+4] into ECX
		MOV		AL, [ESP+8]		 ; read data at [ESP+8] into AL
		MOV		[ECX], AL
		RET