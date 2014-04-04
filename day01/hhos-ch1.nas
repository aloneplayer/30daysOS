;hhos-ch1.ns
;
; FAT 12
    DB 0xeb, 0x4e, 0x90   ;
    DB "HELLOIPL"         ; Name of the boot section (8 bytes)
    DW 512                ; Size of the section
    DB 1                  ; Size of the cluster, should be 1
    DW 1                  ; Start section of the FAT
    DB 2                  ; FAT count 
    DW 224                ; 
    DW 2880               ; How many sections on the disk
    DB 0xf0               ; Type of disk
    DW 9                  ; FAT ????
    DW 18                 ; How many section on one cylinbasyof track
    DW 2                  ; How many disk heads
    DD 0                  ; No partion
    DD 28880              ; How many sections on the disk
    DB 0, 0 , 0x29        ; Don't known
    DD 0xffffffff         ; 
    DB "HELLO-OS"         ; Disk name (11 bytes)
    DB "FAT 12"           ; Format name (8 bytes)
    RESB 18               ; Reserve 18 bytes
; Code
    DB      0xb8, 0x00, 0x00, 0x8e, 0xd0, 0xbc, 0x00, 0x7c  
    DB      0x8e, 0xd8, 0x8e, 0xc0, 0xbe, 0x74, 0x7c, 0x8a  
    DB      0x04, 0x83, 0xc6, 0x01, 0x3c, 0x00, 0x74, 0x09  
    DB      0xb4, 0x0e, 0xbb, 0x0f, 0x00, 0xcd, 0x10, 0xeb  
    DB      0xee, 0xf4, 0xeb, 0xfd  

; Text 
    DB  0x0a, 0x0a
    DB  "hello, world"
    DB  0x0a
    DB  0
    
    RESB 0x1fe-$
    DB   0x55,0xaa
; outside of boot section  
    DB      0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00  
    RESB    4600  
    DB      0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00  
    RESB    1469432     
 