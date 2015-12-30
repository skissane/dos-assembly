%include "libzutl.inc"

start:
        mov ax, 5300h   ; APM Installation Check
        mov bx, 0000h   ; Power Device ID
        int 15h
        jc noAPM        ; CF=0: APM not installed
        cmp bx, 504dh   ; magic value not in BX: APM not installed
        jne noAPM       ; useful in case something else is sitting
                        ; on int 15,AX=5300

        mov ax, 5301h   ; 15,AX=5301: Interface Connect
        mov bx, 0000h   ; BX=0000 (Power Device ID = System BIOS)
        int 15h
        jc mConnFail

        mov ax, 5307h   ; 15,AX=5307: Set Power State
        mov bx, 0000h   ; BX=0000 (Power Device ID = System BIOS)
        mov cx, 0003h   ; CX=0003 (Power State = Off)
        int 15h
        jc mOffFail

        exit 0
noAPM:
        putz mNoAPM
        exit 0
mNoAPM:
        db "APM is not installed.", crlf, 0
mConnFail:
        db "APM connection failure", crlf, 0
mOffFail:
        db "APM power off failure", crlf, 0
