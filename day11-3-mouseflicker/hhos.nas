;hhos.nas
CYLS  EQU  0x0ff0    ; 
LEDS  EQU  0x0ff1    ; record boot information 
VMODE EQU  0x0ff2    ; bits of color
SCRNX EQU  0x0ff4    ; screen X
SCRNY EQU  0x0ff6    ; screen Y
VRAM  EQU  0x0ff8    ; address of video ram

ORG 0xC200

MOV AL, 0x13  ; VGA, 320*200 8bit color
MOV AH, 0x00
INT 0x10
MOV BYTE  [VMODE], 8   ; record mode 
MOV WORD  [SCRNX], 320
MOV WORD  [SCRNY], 200 
MOV DWORD [VRAM], 0x000a0000 

; get led state for keybord
MOV AH, 0x02
INT 0x16
MOV [LEDS], AL
fin:   
    HLT
    JMP fin