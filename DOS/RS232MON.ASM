%include "libzutl.inc"

start:
        BiosEnterVideoMode 3
        BiosEnterVideoPage 1
        call BiosClearScreen
        BiosHomeCursor

        SaveInterruptVector 1Ch, savedInt1C
        HookInt10
        HookInt28

        SaveInDOSAddr
        SaveCritErrAddr

        SetCustomExit restoreInterrupts
        putz mLayout
        call TimerTick
        InstallInterrupt 1Ch, Int1CHandler

        BiosGetKey
        putz eol
        exit 0
restoreInterrupts:
        RestoreInterruptVector 1Ch, savedInt1C
        RestoreInt10
        RestoreInt28
        ret

TestInt:
        iret

Int1CHandler:
        InterruptPrologue
        inc byte [tickCounter]
        cmp byte [tickCounter],18
        jae tick18
ChainInt1C:
        cmp byte [tickPending], 0
        jz noTickPending
        IntCanWork
        jnz noTickPending
        mov byte [tickPending], 0
        call TimerTick
noTickPending:
        ChainInterruptVector savedInt1C
        InterruptEpilogue
        iret
tick18:
        IntCanWork
        jnz tick18pend
        call TimerTick
        jmp ChainInt1C
tick18pend:
        mov byte [tickPending], 1
        jmp ChainInt1C

; So this method should be called once per a second
TimerTick:
        mov bh, 1
        BiosGetCursor
        push dx

        call DrawTicks

        GetBDAByte 84h
        mov dh, al
        mov dl, 60
        BiosSetCursorPos
        call PrintSysDate
        putc " "
        call PrintSysTime
        pop dx
        BiosSetCursorPos
        ret


%macro DrawOneTick 5
        mov dh, %1
        mov dl, %2
        BiosSetCursorPos
        peek %4
        and %5, (1 << %3)
        test %5, %5
        jz %%zclr
        mov bl, 251
        jmp %%zset
%%zclr:
        mov bl, " "
%%zset:
        putc bl
%endmacro

%macro COMDraw 1
        mov ah, 05h
        mov al, 00h
        mov dx, %1
        int 14h
        push bx
        push ax
        DrawOneTick 4, (36+5*%1), 0, ax, al
        DrawOneTick 5, (36+5*%1), 1, ax, al
        DrawOneTick 6, (36+5*%1), 2, ax, al
        DrawOneTick 7, (36+5*%1), 3, ax, al
        DrawOneTick 8, (36+5*%1), 4, ax, al
        DrawOneTick 9, (36+5*%1), 5, ax, al
        DrawOneTick 10, (36+5*%1), 6, ax, al
        DrawOneTick 11, (36+5*%1), 7, ax, al

        DrawOneTick 12, (36+5*%1), 0, ax, ah
        DrawOneTick 13, (36+5*%1), 1, ax, ah
        DrawOneTick 14, (36+5*%1), 2, ax, ah
        DrawOneTick 15, (36+5*%1), 3, ax, ah
        DrawOneTick 16, (36+5*%1), 4, ax, ah
        DrawOneTick 17, (36+5*%1), 5, ax, ah
        DrawOneTick 18, (36+5*%1), 6, ax, ah
        DrawOneTick 19, (36+5*%1), 7, ax, ah

        pop ax
        DrawOneTick 20, (36+5*%1), 0, bx, bl
        DrawOneTick 21, (36+5*%1), 1, bx, bl

        pop bx
%endmacro

DrawTicks:
        COMDraw 0
        COMDraw 1
        COMDraw 2
        COMDraw 3
        ret

savedInt1C:
        dd 0
mLayout:
        times 31 db " "
        db "SERIAL PORT STATUS"
        db crlf
        db 218
        times 33 db 196
        db 194
        times 4 db 196
        db 194
        times 4 db 196
        db 194
        times 4 db 196
        db 194
        times 4 db 196
        db 191
        db crlf
        db 179
        db "                                 ", 179
        db "COM1",179,"COM2",179,"COM3",179,"COM4", 179, crlf
        db 198
        times 33 db 205
        db 216
        times 4 db 205
        db 216
        times 4 db 205
        db 216
        times 4 db 205
        db 216
        times 4 db 205
        db 181
        db crlf
%define lend 179, "    ", 179, "    ", 179, "    ", 179, "    ", 179, crlf
        db 179, " clear to send status changed    ", lend
        db 179, " data set ready status changed   ", lend
        db 179, " trailing edge ring indicator    ", lend
        db 179, " receive line signal changed     ", lend
        db 179, " clear to send                   ", lend
        db 179, " data set ready                  ", lend
        db 179, " ring indicator                  ", lend
        db 179, " receive line signal detected    ", lend
        db 179, " data ready                      ", lend
        db 179, " overrun error                   ", lend
        db 179, " parity error                    ", lend
        db 179, " framing error                   ", lend
        db 179, " break detect                    ", lend
        db 179, " transmit holding register empty ", lend
        db 179, " transmit shift register empty   ", lend
        db 179, " time out                        ", lend
        db 179, " activate data terminal ready    ", lend
        db 179, " activate request to send        ", lend
        db 192
        times 33 db 196
        db 193
        times 4 db 196
        db 193
        times 4 db 196
        db 193
        times 4 db 196
        db 193
        times 4 db 196
        db 217
        db crlf, crlf
        db "Press any key to quit...", 0

; Count of clock ticks, when it gets to 18, go back to zero
; Done this way, we lose roughly 11 msec every second
; For our present purposes, that doesn't matter,
; we don't need clock to be that accurate
tickCounter:
        db 0
tickPending:
        db 0
