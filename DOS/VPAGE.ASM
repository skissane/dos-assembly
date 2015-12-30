%include "libzutl.inc"

start:
        KeepVideoMode
        call CopyCmdLine
        mov bx, bufCmdLine
        call skipws

        mov ah, [bx]
        IsDigit ah
        jnz BadArgs
        call parsedec8
        push ax

        call skipws
        pop ax
        mov ch, [bx]
        test ch, ch
        jnz BadArgs
        push ax

        BiosSetVideoPage ah
        BiosGetVideoMode
        peek ax
        cmp ah, bh
        jne vSetFailed

        putz mHdr
        putz mSet
        pop ax
        mov dl, ah
        call putdec8
        putz eol
        exit 0
BadArgs:
        putz mHdr
        putz mUsage
        jmp FailCase
vSetFailed:
        putz mHdr
        putz mSetFailed
        pop ax
        mov dl, ah
        call putdec8
        putz eol
FailCase:
        putz mActivePage
        BiosGetVideoMode
        mov dl, bh
        call putdec8
        putz eol
        exit 1

mHdr:   db "Video Page Setter", crlf
        db "Copyright (C) Simon Kissane 2014", crlf
        db crlf, 0
mUsage: db "usage: vpage PAGE", crlf
        db "PAGE must be decimal video page", crlf
        db crlf, 0
mSet:   db "Set video page to ",0
mSetFailed:
        db "Failed to set video page to ",0
mActivePage:
        db "Active video page is "
        db 0
