; haribote-os boot asm
; TAB=4

BOTPAK	EQU		0x00280000		; bootpack�Υ�`����
DSKCAC	EQU		0x00100000		; �ǥ���������å���Έ���
DSKCAC0	EQU		0x00008000		; �ǥ���������å���Έ������ꥢ���`�ɣ�

; BOOT_INFO�v�S
CYLS	EQU		0x0ff0			; �֩`�ȥ��������O������
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2			; ɫ�����v������󡣺Υӥåȥ���`����
SCRNX	EQU		0x0ff4			; ����Ȥ�X
SCRNY	EQU		0x0ff6			; ����Ȥ�Y
VRAM	EQU		0x0ff8			; ����ե��å��Хåե����_ʼ����

		ORG		0xc200			; ���Υץ���ब�ɤ����i���z�ޤ��Τ�

; �����`�ɤ��O��

		MOV		AL,0x13			; VGA����ե��å�����320x200x8bit����`
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	; �����`�ɤ��⤹�루C���Z�����դ��룩
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

; ���`�ܩ`�ɤ�LED״�B��BIOS�˽̤��Ƥ�餦

		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL

; PIC disable all INT
;	AT compatible computer must init PIC before CLI
;	Or, the system will be hung up some times
;	Init PIC

		MOV		AL,0xff
		OUT		0x21,AL         ; io_out(PIC1_IMR, 0xff) disable PIC0
		NOP						; Some machine can't work when call "out" continuously
		OUT		0xa1,AL         ; io_out(PIC1_IMR, 0xff) disable PIC1   

		CLI                     ; Disable all INT

; CPU access more than 1MB memory set A20GATE

		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL         ; io_out8(PORT_KEYCMD, KEYCMD_WRITE_OUTPORT)
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL         ; io_out8(PORT_KEYDAT, KEYCMD_A20G_ENABLE)
		CALL	waitkbdout

; Switch to protect mode

[INSTRSET "i486p"]				; 486 instruction, can use LGDT, EAX, CR0...

		LGDT	[GDTR0]			; temple GDT
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; set CR0 bit 31 to 0, disable memory pagination
		OR		EAX,0x00000001	; set CR0 bit 0 to 1, switch to protected mode
		MOV		CR0,EAX
		JMP		pipelineflush
pipelineflush:
		MOV		AX,1*8			;  readable and writeable segment 32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX

; Read bootpack into memory

		MOV		ESI,bootpack	; the source
		MOV		EDI,BOTPAK		; the target
		MOV		ECX,512*1024/4
		CALL	memcpy

; Transfer the data on the disk

; Transfer boot section from 0x7c00 to 0x00100000

		MOV		ESI,0x7c00		; the source
		MOV		EDI,DSKCAC		; the target
		MOV		ECX,512/4
		CALL	memcpy

; All of the rest data from 0x8200 to 0x00100200

		MOV		ESI,DSKCAC0+512	; the source
		MOV		EDI,DSKCAC+512	; the target
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL	ECX,512*18*2/4	; convert cylinder to bytes
		SUB		ECX,512/4		; IPL�η֤��������
		CALL	memcpy

; The work of asmhead is finished.
; Following is the work of bootpack

; bootpack boot

		MOV		EBX,BOTPAK
		MOV		ECX,[EBX+16]
		ADD		ECX,3			; ECX += 3;
		SHR		ECX,2			; ECX /= 4;
		JZ		skip			; No data need transfer
		MOV		ESI,[EBX+20]	; source
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]	; target
		CALL	memcpy
skip:
		MOV		ESP,[EBX+12]	; stack 
		JMP		DWORD 2*8:0x0000001b

waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		JNZ		waitkbdout		; AND�νY����0�Ǥʤ����waitkbdout��
		RET

memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; jump to memcpy if ecx != 1
		RET
; memcpy�ϥ��ɥ쥹�������ץ�ե��������������ʤ���С����ȥ������Ǥ������

		ALIGNB	16
GDT0:
		RESB	8				; GDT0 NULL selector
		DW		0xffff,0x0000,0x9200,0x00cf	; GDT1 readable segment 32bit
		DW		0xffff,0x0000,0x9a28,0x0047	; GDT2executable segment 32bit��bootpack use it��

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16
bootpack: