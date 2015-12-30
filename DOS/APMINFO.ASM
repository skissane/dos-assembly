%include "libzutl.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: putbcdver
; Purpose: Print 16-bit BCD version number
;          e.g. 1234h prints as 12.34
;               0100h prints as 1.00
; Input Registers:
;   DX = number to print
; Output Registers:
;   None
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
putbcdver:
        push dx
        shr dh, 4
        jz putbcdver1
        add dh, 30h
        putc dh

putbcdver1:
        peek dx
        and dh, 0Fh
        add dh, 30h
        putc dh

        putc '.'

        peek dx
        shr dl, 4
        add dl, 30h
        putc dl

        pop dx
        and dl, 0Fh
        add dl, 30h
        putc dl

        ret

start:
        putz mHdr
        mov ax, 5300h   ; APM Installation Check
        mov bx, 0000h   ; Power Device ID
        int 15h
        jc noAPM        ; CF=0: APM not installed
        cmp bx, 504dh   ; magic value not in BX: APM not installed
        jne noAPM       ; useful in case something else is sitting
                        ; on int 15,AX=5300

        putz mAPMVersion
        mov dx, ax
        call putbcdver
        putz eol

        exit 0
noAPM:
        putz mNoAPM
        exit 0
mHdr:
        db "APM Information Utility",crlf
        db "Copyright (C) Simon Kissane 2015",crlf
        db crlf, 0
mNoAPM:
        db "APM is not installed.", crlf, 0
mAPMVersion:
        db "APM Version ", 0
