%include "libzutl.inc"

start:
        putz mBanner
        putz mPCType

        push ds
        mov ax, 0F000h
        mov ds, ax
        xor dx, dx
        mov dl, [0FFFEh]
        pop ds

        call puthex8
        putz eol

        exit 0

mBanner:
        db "PC Type Retriever", crlf
        db "Copyright (C) Simon Kissane 2015", crlf, crlf, 0
mPCType:
        db "PC Type = ", 0
