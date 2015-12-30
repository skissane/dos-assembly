%include "libzutl.inc"

start:
        putz mBanner
        putz mLine1
        xor dx, dx
        mov dl, byte [80h]
        call putdec8
        putz eol

        putz mLine2

        mov bx, 81h
        xor cx, cx
        mov cl, byte [80h]
beginLoop:
        jcxz endLoop
        mov byte dl, [bx]
        push bx
        push cx
        putc dl
        pop cx
        pop bx
        inc bx
        dec cx
        jmp beginLoop
endLoop:
        putz eol
        putz mLine3
        putz mLine4

        push ds
        mov ax, [2Ch]
        mov ds, ax

        mov bx, 0
envLoop:
        cmp byte [bx], 0
        jz endEnv
        putz bx
        putc 13
        putc 10
findNextZero:
        inc bx
        cmp byte [bx], 0
        jnz findNextZero
        inc bx
        jmp envLoop
endEnv:
        pop ds
        putz mLine5

        exit 0
mLine1:
        db "Argument Count = ", 0
mLine2:
        db "**** BEGINS CMDLINE ****", crlf, 0
mLine3:
        db "**** ENDING CMDLINE ****", crlf, crlf, 0
mLine4:
        db "**** BEGINS ENVIRON ****", crlf, 0
mLine5:
        db "**** ENDING ENVIRON ****", crlf, 0
mBanner:
        db "Argument Dumper", crlf
        db "Copyright (C) Simon Kissane 2015", crlf, crlf, 0
