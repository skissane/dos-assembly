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

        BiosSetVideoMode ah
        BiosGetVideoMode
        mov bl, al
        peek ax
        cmp ah, bl
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
        putz mActiveMode
        BiosGetVideoMode
        mov dl, al
        call putdec8
        putz eol
        exit 1

mHdr:   db "Video Mode Setter", crlf
        db "Copyright (C) Simon Kissane 2014", crlf
        db crlf, 0
mUsage: db "usage: vmode MODE", crlf
        db "MODE must be decimal video mode", crlf
        db crlf, 0
mActiveMode:
        db "Active video mode is "
        db 0
mSet:   db "Set video mode to ",0
mSetFailed:
        db "Failed to set video mode to ",0
