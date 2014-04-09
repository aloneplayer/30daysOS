; hhos-ch3

CYLS	EQU		10		

    ORG     0x7c00

    ; FAT 12
    JMP SHORT entry
    DB      0x90
    DB      "HELLOIPL"      ; Boot sector name (8 bytes)
    DW      512             ; Size of sector (Must be 512)
    DB      1               ; Size of cluster (Must be 1)
    DW      1               ; FAT start
    DB      2               ; FAT count
    DW      224             ; Size of root dir
    DW      2880            ; Size of this media (Must be 2880)
    DB      0xf0            ; Media type (Must be 0xf0)
    DW      9               ; FAT length
    DW      18              ; Number of sector in a track
    DW      2               ; Number of head
    DD      0               ; No partition
    DD      2880            ; Drive size
    DB      0,0,0x29        ; Magic
    DD      0xffffffff      ; Volume serial number
    DB      "HELLO-OS   "   ; Disk name (11 bytes)
    DB      "FAT12   "      ; Format name (8 bytes)
    TIMES 18 DB 0
    

; Code 

entry:
    MOV		AX,0			; init regs
    MOV		SS,AX
    MOV		SP,0x7c00
    MOV		DS,AX

; Read disk

    MOV		AX,0x0820       ; buffer
    MOV		ES,AX
    MOV		CH,0			; Cylinder 0
    MOV		DH,0			; head 0
    MOV		CL,2			; section 2

readloop:
	MOV		SI,0			; failed times
retry:
    MOV		AH,0x02			; AH=0x02 : reading
    MOV		AL,1			; read 1 section
    MOV		BX,0
    MOV		DL,0x00			; driver A
    INT		0x13			; call INT 13
    JNC		next		    ; FLAGES.CF = 0 reading successful and read next section
    ADD		SI,1			; failed times + 1  
    CMP		SI,5			; compare failed times , 5
    JAE		error			; 
    MOV		AH,0x00         ; reset driver A
    MOV		DL,0x00			; 
    INT		0x13			; 
    JMP		retry
    
; read next section 2 to 18
next: 
    MOV  AX, ES; address + 0x200
    ADD  AX, 0x20
    MOV  ES, AX  ; There is no ADD EX, N
    ADD  CL, 1
    CMP  CL, 18
    JBE  readloop  ; jump if below or equal
    MOV  CL, 1
    ADD  DH, 1
    CMP  DH, 2
    JB   readloop
    MOV  DH, 0
    ADD  CH, 1
    CMP  CH, CYLS
    JB   readloop   ; jump if below
    
;   Jump to os from the loader 
    MOV   [0x0ff0], CH    ; tell os how many cylinder were read
    JMP	  0xc200    
            
fin:
	HLT						; 
	JMP		fin				; 

error:
	MOV		SI,msg
putloop:
    MOV		AL,[SI]
    ADD		SI,1			; char code 
    CMP		AL,0
    JE		fin
    MOV		AH,0x0e			; 
    MOV		BX,15			;
    INT		0x10			; 
    JMP		putloop
msg:
    DB		0x0a, 0x0a		; 
    DB		"load error"
    DB		0x0a			;
    DB		0

    TIMES 0x01fe-($-$$) DB 0   ; Fill 0x00 until 510 byte

    DB		0x55, 0xaa