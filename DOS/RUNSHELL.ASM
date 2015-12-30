%include "libzutl.inc"

%macro execSetupSegs 0
        mov ax, cs
        mov ds, ax
        mov es, ax
%endmacro

%macro execSaveSS 0
        mov ax, ss
        mov [saveSS], ax
%endmacro

%macro execRestoreSS 0
        mov ax, [saveSS]
        mov ss, ax
%endmacro

start:
        putz mBanner

        mov ax, cs
        mov es, ax
        mov ah, 4Ah
        mov bx, 1000h
        int 21h
        jc freeFailed

        mov bx, cmdLineZ
        call ASCIIZLength
        mov byte [cmdArgs], cl
        mov si, cmdLineZ
        mov di, cmdArgs
        inc di
        call MemoryCopy

        execSetupSegs
        execSaveSS

        mov word [epbCmdLinePtr+2], ds
        mov word [epbCmdLinePtr], cmdArgs
        mov ax, [2Ch]
        mov word [epbEnvSeg], ax

        mov dx, cmdToRun
        mov bx, epbBegin
        mov ah, 4bh
        mov al, 0
        int 21h
        jc execErr

        execSetupSegs
        execRestoreSS

        putz mFinished
        mov ah, 4Dh
        int 21h
        push ax

        putz mSysExitCode
        peek ax
        mov dl, ah
        call putdec8
        putz eol

        putz mChildExitCode
        pop ax
        mov dl, al
        call putdec8
        putz eol

        exit 0

execErr:
        mov word [errno], ax
        execSetupSegs
        execRestoreSS
        execSetupSegs

        putz mError
        mov dx, [errno]
        call putzhex16
        putz eol

        exit 1

freeFailed:
        mov word [errno], ax
        putz mMemError
        mov dx, [errno]
        call putzhex16
        putz eol

mError:
        db "INT 21,4C error ", 0
mMemError:
        db "INT 21,4A error ", 0
cmdToRun:
        db "C:\COMMAND.COM", 0
cmdLineZ:
        ; db " Some sample cmd line",13
        db 0
cmdArgs:
        times 80h db 0
epbBegin:
epbEnvSeg:      dw 0
epbCmdLinePtr:  dd 0
epbFCB1:        dd 0
epbFCB2:        dd 0
saveSS:         dw 0
errno:          dw 0
mBanner:
        db "Shell Executor", crlf
        db "Copyright (C) Simon Kissane 2015", crlf, crlf, 0
mFinished:
        db "**** SUBPROCESS TERMINATED ****", crlf, 0
mSysExitCode:
        db "System Exit Code = ", 0
mChildExitCode:
        db "Child Exit Code = ", 0
