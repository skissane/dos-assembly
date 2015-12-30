%include "libzutl.inc"

start:
        putz mBanner

        xor cx, cx
loop:
        push cx
        putz mINT

        peek cx
        mov dl, cl
        call putzhex8
        putz mEQ

        peek cx
        GetInterruptVector cl
        push bx
        push es
        pop dx
        call putzhex16
        putc ":"
        pop dx
        call putzhex16
        putz eol

        pop cx
        inc cx
        test cl, cl
        jnz loop
        exit 0
mBanner:
        db "Interrupt Table Dumper",crlf
        db "Copyright (C) 2014 Simon Kissane",crlf
        db crlf,0
mINT:
        db "INT ",0
mEQ:    db " = ",0


mSEG:   db "Segment = ",0
mOFF:   db "Offset = ",0
