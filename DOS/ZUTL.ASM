%include "libzutl.inc"

start:
        putz header

        ;; Display BIOS Video Mode
        putz mVidModeDec
        BiosGetVideoMode
        push bx
        push ax
        mov dl,al
        call putdec8
        putz mVidModeHex
        peek ax
        mov dl,al
        call putzhex8
        putz mVidModeEnd

        ;; Display Columns per Line
        putz mColsPerLine
        pop ax
        mov dl,ah
        call putdec8
        putz eol

        ;; Display Active Display Page
        putz mActivePage
        peek bx
        mov dl,bh
        call putdec8
        putz eol
        putz eol

        ;; Cursor Information Section
        putz mCursorInfo

        ;; Display Start Scan Line
        putz mStartScan
        pop bx
        BiosGetCursor
        push dx
        push cx
        mov dl,ch
        call putdec8
        putz eol

        ;; Display End Scan Line
        putz mEndScan
        pop cx
        mov dl,cl
        call putdec8
        putz eol

        ;; Display Row
        putz mRow
        peek dx
        mov dl,dh
        call putdec8
        putz eol

        ;; Display Column
        putz mCol
        pop dx
        call putdec8
        putz eol
        putz eol

        ;; Display Command Line Bytes
        putz mCmdLineBytes
        mov dl,[cmdLineBytes]
        call putdec8
        putz eol

        ;; Display command line
        call CopyCmdLine
        putz mCmdLineStart
        putz bufCmdLine
        putz mCmdLineEnd

        ;; Get device info for stdout
        putz mStdout
        mov bx, 1
        call DosGetDevInfo
        push dx

%macro devInfoTest 2
        putz %2
        peek dx
        test dl, %1
        putyesno
        putz eol
%endmacro

        devInfoTest 80h, mIsDev
        peek dx
        test dl, 80h
        jz notADevice

        devInfoTest 40h, mIsEOF
        devInfoTest 20h, mIsRaw
        devInfoTest 10h, mIsSpecial
        devInfoTest 08h, mIsClock
        devInfoTest 04h, mIsNull
        devInfoTest 02h, mIsConOut
        devInfoTest 01h, mIsConIn
notADevice:
        pop dx

        ;; Get DOS version
        putz eol
        putz mDosVersion
        mov ah, 30h
        call syscall
        push ax
        mov dl,al
        call putdec8
        putz mDosVerSep
        pop ax
        mov dl,ah
        call putdec8
        putz eol

        ;; Display Current Drive
        putz mCurDrive
        mov ah, 19h
        call syscall
        add al, "A"
        putc al
        putz eol

        ;; Display Current Date
        putz mDate
        call PrintSysDate
        putz eol

        ;; Display Current Time
        putz mTime
        call PrintSysTime
        putz eol

        ;; Check if Video
        putz mIsVideo
        call IsVideo
        test al,al
        putyesno
        putz eol

        ;; Get Video Rows from BDA
        putz mVideoRows
        GetBDAByte 84h
        inc al
        mov dl, al
        call putdec8
        putz eol

        exit 0

section .data
header:    db "System Information Utility",crlf
           db "Copyright (C) Simon Kissane 2014",crlf
           db crlf
           db 0
mVidModeDec:
           db "Current video mode is ",0
mVidModeHex:
           db " (", 0
mVidModeEnd:
           db " hex)",crlf,0
mColsPerLine:
           db "Columns per a line: ",0
mActivePage:
           db "Active display page: ",0
mCursorInfo:
           db "--- Cursor Info ---", crlf, 0
mStartScan:
           db "Start scan line: ",0
mEndScan:
           db "End scan line: ",0
mRow:
           db "Row: ",0
mCol:
           db "Col: ",0
mCmdLineBytes:
           db "Command Line Bytes: ",0
mStdout:
           db crlf,"--- Standard Out Device Info ---",crlf,0
mIsDev:    db "Is Device: ", 0
mIsEOF:    db "Is EOF: ", 0
mIsRaw:    db "Is Raw Mode: ", 0
mIsSpecial: db "Is Special: ", 0
mIsClock: db "Is Clock: ",0
mIsNull: db "Is Null: ",0
mIsConIn: db "Is Console Input: ",0
mIsConOut: db "Is Console Output: ",0

mCmdLineStart: db "Command Line: ",'"',0
mCmdLineEnd:   db '"',crlf,0

mDosVersion: db "DOS Version: ",0
mDosVerSep: db ".",0
mCurDrive: db "Current Drive: ",0
mDate: db "Date: ",0
mTime: db "Time: ",0
mIsVideo: db "Is Video: ",0
mVideoRows: db "Video Rows: ",0
