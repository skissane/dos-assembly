;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; LIBZUTL.INC - General Purpose Assembly Library
;
; Assumptions:
; 1) Only supports .COM files - no .EXE support
; 2) User must provide "start" label as entry
;    point
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
cmdLineBytes    equ 80h
cmdLineStart    equ 81h

; Since we are a COM, we stick data in code sect
%define csect section .text
%define dsect section .text

csect
org 100h
        call VideoModeSave
        jmp start

%define crlf 13,10

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: exit
; Purpose: Exit to DOS
; Arguments:
;   %1 = Return code
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro exit 1
        sti
        call VideoModeRestore
        mov ax, [customExit]
        test ax, ax
        jz %%noCustomExit
        call ax
%%noCustomExit:
        mov ah,4ch
        mov al,%1
        int 21h
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: putc
; Purpose: Prints a single character
; Arguments:
;   %1 = character to print
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro putc 1
        mov ah,02h
        mov dl,%1
        int 21h
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: putdec
; Purpose: Prints a single decimal digit
; Arguments:
;   %1 = decimal digit to print
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro putdec 1
        mov dl,%1
        add dl,30h
        putc dl
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: puthex
; Purpose: Prints a single hexidecimal digit
; Arguments:
;   %1 = hex digit to print
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro puthex 1
        mov dl,%1
        cmp dl,10
        jge %%high
        add dl,30h
        putc dl
        jmp %%end
%%high:
        add dl,("A" - 10)
        putc dl
%%end:
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: BiosGetVideoMode
; Purpose: Gets BIOS video mode
; Arguments: None
; Output Registers:
;    AL = Video Mode
;    AH = Columns per line
;    BH = Active display page
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro BiosGetVideoMode 0
        mov ah,0fh
        int 10h
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: BiosSetVideoMode
; Purpose: Sets BIOS video mode
; Arguments:
; %1 = new video mode
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro BiosSetVideoMode 1
        mov al,%1
        mov ah,0
        int 10h
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: BiosSetVideoPage
; Purpose: Sets BIOS video page
; Arguments:
; %1 = new video page
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro BiosSetVideoPage 1
        mov al,%1
        mov ah,5
        int 10h
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: BiosEnterVideoMode
; Purpose: Enters a video mode, but only if we
;          are not already in it
; Arguments:
; %1 = new video mode
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro BiosEnterVideoMode 1
        BiosGetVideoMode
        cmp al, %1
        je %%end
        BiosSetVideoMode %1
%%end:
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: BiosEnterVideoPage
; Purpose: Enters a video page, but only if we
;          are not already in it
; Arguments:
; %1 = new video page
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro BiosEnterVideoPage 1
        BiosGetVideoMode
        cmp bh, %1
        je %%end
        BiosSetVideoPage %1
%%end:
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: BiosGetCursor
; Purpose: Get BIOS cursor position
; Arguments: None
; Input Registers:
;    BH = Page Number
; Output Registers:
;    CH = Start Scan Line
;    CL = End Scan Line
;    DH = Row
;    DL = Column
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro BiosGetCursor 0
        mov ah,03h
        int 10h
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: putz
; Purpose: Prints a 0-terminated string to console
; Arguments:
;   %1 = Address of string to print
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro putz 1
        mov bx,%1
%%loop:
        mov dl,[bx]
        test dl,dl
        jz %%end
        push bx
        mov ah,02h
        int 21h
        pop bx
        add bx, 1
        jmp %%loop
%%end:
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: putyesno
; Purpose: Prints Yes or No
;          Depending on state of ZF (Zero Flag)
;          ZF set = No
;          ZF clear = Yes
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro putyesno 0
        jz %%no
        putz mYes
        jmp %%end
%%no:   putz mNo
%%end:
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: peek
; Purpose: Like POP, but leaves the POP value on
;          stack.
; Argument:
;  %1 = Register to peek
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro peek 1
        pop %1
        push %1
%endmacro

csect

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: putzdec8
; Purpose: Print 8-bit unsigned number in decimal
;          with initial zero padding
; Input Registers:
;   DL = number to print
; Output Registers:
;   None
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
putzdec8:
        cmp dl,100
        jae putzdec8z1
        push dx
        putc "0"
        pop dx
putzdec8z1:
        cmp dl,10
        jae putdec8
        push dx
        putc "0"
        pop dx
        jmp putdec8

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: putzhex8
; Purpose: Print 8-bit unsigned number in hex
;          with initial zero padding
; Input Registers:
;   DL = number to print
; Output Registers:
;   None
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
putzhex8:
        cmp dl,16
        jae puthex8
        push dx
        putc "0"
        pop dx
        jmp puthex8

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: putdec8
; Purpose: Print 8-bit unsigned number in decimal
; Input Registers:
;   DL = number to print
; Output Registers:
;   None
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
putdec8:
        cmp dl,100
        jb putdec8z1
        xor ax,ax
        mov al,dl
        mov bl,100
        idiv bl
        mov dl,al
        xchg ah,al
        xor ah,ah
        push ax
        putdec dl
        pop dx
        cmp dl,10
        jae putdec8z1
        push dx
        putdec 0
        pop dx
putdec8z1:
        cmp dl,10
        jb putdec8z2
        xor ax,ax
        mov al,dl
        mov bl,10
        idiv bl
        mov dl,al
        xchg ah,al
        xor ah,ah
        push ax
        putdec dl
        pop dx
putdec8z2:
        putdec dl
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: puthex8
; Purpose: Print 8-bit unsigned number in hex
; Input Registers:
;   DL = number to print
; Output Registers:
;   None
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
puthex8:
        cmp dl,16
        jb puthex8z1
        xor ax,ax
        mov al,dl
        mov bl,16
        idiv bl
        mov dl,al
        xchg ah,al
        xor ah,ah
        push ax
        puthex dl
        pop dx
puthex8z1:
        puthex dl
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: KeepVideoMode
; Purpose: When run, will disable restoring of
;      video mode/page at exit
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro KeepVideoMode 0
        xor ah, ah
        mov [restoreVideoOnExit], ah
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: syscall
; Purpose: Performs Int 21h call
;          Aborts program with error if fails
; Input:
;   AH = Int 21h subfunction
;   other registers as per that subfunction
; Output:
;   per that subfunction
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
syscall:
        push ax ; Save AH for error message
        int 21h
        jc syscall.err
        add sp,2 ; Success, discard AH
        ret
syscall.err:
        sti
        pop bx   ; Stash saved AH in BX
        push ax  ; Save output AX from Int 21h
        push bx  ; Resave stashed input AX

        ; Reset DS to CS in case some other
        ; DS used as argument to syscall
        push cs
        pop ds

        ; Restore video before we print error
        ; Otherwise, the restore may wipe away
        ; the error message
        call VideoModeRestore

        ; Now we have restored video, turn
        ; off video auto-restoration, otherwise
        ; we will restore again when we finally
        ; exit
        KeepVideoMode

        ; Display error message
        putz mSysCallError
        pop ax   ; Restore stashed input AX
        mov dl,ah
        call putzhex8
        putz mSysCallError2
        pop ax
        mov dl,al
        call putdec8
        putz eol
        putz mAborted

        ; Exit the program
        exit 1

dsect
mSysCallError:
        db crlf,"Int 21,AH=",0
mSysCallError2:
        db " failed with error #",0
mAborted:
        db "Program Aborted",crlf,0
csect

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: DosGetDevInfo
; Purpose: Get device information
; Input:
;   BX = File handle
; Output:
;   DX = Device Information Word
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
DosGetDevInfo:
        mov ax,4400h
        call syscall
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: PrintSysDate
; Purpose: Print current date in ISO-8601 format
; Input: None
; Output: None
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PrintSysDate:
        mov ah,2Ah
        call syscall
        push dx                 ;MonthDay
        sub cx,1900
        mov ax,cx
        mov bl,100
        div bl
        push ax                 ;YearQR,MonthDay
        add al,19
        mov dl,al
        call putdec8
        pop ax                  ;MonthDay
        mov dl,ah
        cmp dl,10
        jge not2000s
        push dx                 ;DecadeYear,MonthDay
        putc "0"
        pop dx                  ;MonthDay
not2000s:
        call putdec8
        putc "-"
        peek dx                 ;MonthDay
        cmp dh,10
        jge notBeforeOct
        putc "0"
        peek dx                  ;Empty stack
notBeforeOct:
        mov dl,dh
        call putdec8
        putc "-"
        peek dx                  ;Empty stack
        cmp dl,10
        jge notBefore10th
        putc "0"
notBefore10th:
        pop dx                  ;Empty stack
        call putdec8
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: PrintSysTime
; Purpose: Print current time in ISO-8601 format
; Input: None
; Output: None
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PrintSysTime:
        mov ah,2Ch
        call syscall
        push dx                 ;Seconds
        push cx                 ;HourMin,Seconds
        cmp ch,10
        jge notHours1Digit
        putc "0"
        peek cx                 ;HourMin,Seconds
notHours1Digit:
        mov dl, ch
        call putdec8
        putc ":"
        peek cx                 ;HourMin,Seconds
        cmp cl,10
        jge notMinutes1Digit
        putc "0"
notMinutes1Digit:
        pop cx                  ;Seconds
        mov dl, cl
        call putdec8
        putc ":"
        peek dx                 ;Seconds
        cmp dh,10
        jge notSeconds1Digit
        putc "0"
 notSeconds1Digit:
        pop dx                  ;Empty
        mov dl, dh
        call putdec8
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: CopyCmdLine
; Purpose: Copy command line from PSP to ASCIIZ
;          buffer
; Input: None
; Output: None
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CopyCmdLine:
        xor cx, cx
        mov cl, [cmdLineBytes]
        mov si, cmdLineStart
        mov di, bufCmdLine
clMoveLoop:
        test cx, cx
        jz clMoveEnd
        xor ax, ax
        mov al, [si]
        cmp al, 13
        jne clNotCr
        mov al, 0
clNotCr:
        mov [di], al
        inc si
        inc di
        dec cx
        jmp clMoveLoop
clMoveEnd:
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: IsVideo
; Purpose: Checks that STDOUT goes to Console
; Input: None
; Output:
;   AL=1 if STDOUT goes to console
;   AL=0 if STDOUT goes elsewhere
;            eg: file, serial port
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
IsVideo:
        mov bx, 1
        call DosGetDevInfo
        xor ax, ax
        test dl, 80h ; Is Device Flag true?
        jz IsVideoNo
        test dl, 10h ; Is Special Flag true?
        jz IsVideoNo
        test dl, 02h ; Is Console Output Flag true?
        jz IsVideoNo
        test dl, 01h ; Is Console Input Flag true?
        jz IsVideoNo
        mov ax, 1
IsVideoNo:
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: VideoModeSave
; Purpose: Save video mode at startup
; Input: None
; Output: None
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
VideoModeSave:
        BiosGetVideoMode
        mov [savedVideoMode], al
        mov [savedVideoPage], bh
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: VideoModeRestore
; Purpose: Restore video mode at exit
; Input: None
; Output: None
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
VideoModeRestore:
        mov ah, [restoreVideoOnExit]
        test ah, ah
        jz VideoPageSame
        BiosGetVideoMode
        mov ah, [savedVideoMode]
        cmp al, ah
        je VideoModeSame
        BiosSetVideoMode ah
VideoModeSame:
        BiosGetVideoMode
        mov bl, [savedVideoPage]
        cmp bl, bh
        je VideoPageSame
        BiosSetVideoPage bl
VideoPageSame:
        ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: skipws
; Purpose: Skip whitespace in string
; Input:
;    BX = Pointer to ASCIIZ string
; Output:
;    BX advanced to first non-whitespace character
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
skipws:
        mov ah, [bx]
        test ah, ah
        jz skipwsExit
        cmp ah, 32
        je skipwsMatch
        cmp ah, 9
        je skipwsMatch
        jmp skipwsExit
skipwsMatch:
        inc bx
        jmp skipws
skipwsExit:
        ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; CPU Flag definitions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%define _CF  0 ; Carry Flag
%define _PF  2 ; Parity Flag
%define _AF  4 ; Adjust Flag
%define _ZF  6 ; Zero Flag
%define _SF  7 ; Sign Flag
%define _TF  8 ; Trap Flag
%define _IF  9 ; Interrupt Flag
%define _DF 10 ; Direction Flag
%define _OF 11 ; Overflow Flag

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: SetFlag
; Purpose: Sets a flag
; Arguments:
; %1 = Flag bit to set
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro SetFlag 1
        push ax
        pushf
        pop ax
        or ax, (1 << %1)
        push ax
        popf
        pop ax
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: ClearFlag
; Purpose: Clears a flag
; Arguments:
; %1 = Flag bit to clear
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro ClearFlag 1
        push ax
        pushf
        pop ax
        and ax, ~(1 << %1)
        push ax
        popf
        pop ax
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: IsDigit
; Purpose: Test if character is decimal digit
; Arguments:
; %1 = 8-bit Input Register containing character
; Output:
;    ZF is set if character is decimal digit
;       otherwise ZF is cleared
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro IsDigit 1
        test %1, %1
        jz %%notDigit
        cmp %1, "0"
        jb %%notDigit
        cmp %1, "9"
        ja %%notDigit
        SetFlag _ZF
        jmp %%end
%%notDigit:
        ClearFlag _ZF
%%end:
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: parsedec8
; Purpose: Parse unsigned 8-bit decimal
; Input:
;    BX = Pointer to ASCIIZ string
; Output:
;    BX = First non-digit
;    AH = value
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
parsedec8:
        call skipws
        xor ax, ax
parsedec8loop:
        mov ch, [bx]
        test ch, ch
        jz parsedec8exit
        cmp ch, "0"
        jb parsedec8exit
        cmp ch, "9"
        ja parsedec8exit
        sub ch, "0"
        mov cl, 10
        xchg al, ah
        mul cl
        xchg al, ah
        add ah, ch
        inc bx
        jmp parsedec8loop
parsedec8exit:
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: GetBDAByte
; Purpose: Get byte from BDA
; Arguments:
;       %1 = offset in BDA (segment 40h)
; Output:
;       AL = value at that byte
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro GetBDAByte 1
        push ds
        mov ax, 40h
        push ax
        pop ds
        xor ax, ax
        mov al, [%1]
        pop ds
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: BDAGetColumns
; Purpose: Get screen columns from BDA
; Arguments: None
; Output:
;       AL = columns of screen
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro BDAGetColumns 0
        GetBDAByte 4Ah
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: BDAGetRows
; Purpose: Get screen rows from BDA
; Arguments: None
; Output:
;       AL = columns of screen
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro BDAGetRows 0
        GetBDAByte 84h
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: BiosClearScreen
; Purpose: Calls BIOS to clear screen
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BiosClearScreen:
        BDAGetColumns
        push ax
        BDAGetRows
        mov dh, al ; CH = row of lower right corner of rectangle
        pop ax
        mov dl, al ; CL = col of lower right corner of rectangle
        ;dec dl
        mov bh, 7 ; BH = video attribute (7 = white on black)
        mov al, 0 ; AL = number of lines to scroll (0 = blank rectangle)
        mov cx, 0 ; DH,DL: row,col of upper left; (0,0) = origin
        mov ah, 6 ; Subfunction 6: scroll up / clear rectangle
        int 10h
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: BiosSetCursorPos
; Purpose: Set BIOS cursor position
; Arguments: None
; Input Registers:
;    DH = Row
;    DL = Col
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro BiosSetCursorPos 0
        push dx
        BiosGetVideoMode ; Sets BH=active page
                         ; which int 10,2 requires
        pop dx
        mov ah, 2
        int 10h
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: BiosHomeCursor
; Purpose: Move cursor to origin (0,0)
; Arguments: None
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro BiosHomeCursor 0
        xor dx, dx
        BiosSetCursorPos
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: BiosGetKey
; Purpose: Read a key from keyboard
; Inputs: None
; Outputs:
; - AL = ASCII key
; - AH = Scan Code
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro BiosGetKey 0
        mov ah, 0
        int 16h
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: GetInterruptVector
; Purpose: Gets an interrupt vector
; Arguments:
; %1 = interrupt vector
; Outputs:
; - ES = segment of interrupt vector
; - BX = offset of interrupt vector
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro GetInterruptVector 1
        mov al, %1
        mov ah, 35h
        call syscall
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: SaveInterruptVector
; Purpose: Saves an interrupt vector
; Arguments:
; %1 = interrupt vector
; %2 = save memory location
; Outputs: None
;
; Note: ChainInterruptVector does a CALL FAR,
; which requires the memory address to be in
; little endian format, OFFSET then SEGMENT.
; If we were only saving/restoring, not chaining,
; then the order would not matter.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro SaveInterruptVector 2
        GetInterruptVector %1
        push es
        push bx
        mov bx, word %2
        pop word [bx]   ; offset of IV
        pop word [bx+2] ; segment of IV
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: RestoreInterruptVector
; Purpose: Restores an interrupt vector
; Arguments:
; %1 = interrupt vector
; %2 = save memory location
; Outputs: None
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro RestoreInterruptVector 2
        push ds
        mov bx, %2
        mov dx, [bx]
        push word [bx+2]
        pop ds
        mov al, %1
        mov ah, 25h
        cli
        call syscall
        sti
        pop ds
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: InstallInterrupt
; Purpose: Installs interrupt vector in our own
;          code segment
; Arguments:
; %1 = interrupt vector
; %2 = our handler
; Outputs: None
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro InstallInterrupt 2
        mov ah, 25h
        mov al, %1
        mov dx, %2
        cli
        call syscall
        sti
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: ChainInterruptVector
; Purpose: Chains an interrupt vector
; Arguments:
; %1 = save memory location
; Outputs: None
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro ChainInterruptVector 1
        pushf
        cs call far [%1]
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: Int10Handler
; Purpose: Int 10h to set InBIOS flag then chain
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Int10Handler:
        cs inc byte [InBios]
        ChainInterruptVector savedInt10
        cs dec byte [InBios]
        iret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: Int28Handler
; Purpose: Int 28h to set InIdle flag then chain
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Int28Handler:
        cs inc byte [InIdle]
        ChainInterruptVector savedInt28
        cs dec byte [InIdle]
        iret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: HookInt10
; Purpose: Hooks int 10h to set InBIOS flag
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro HookInt10 0
        SaveInterruptVector 10h, savedInt10
        InstallInterrupt 10h, Int10Handler
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: HookInt28
; Purpose: Hooks int 28h to set InIdle flag
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro HookInt28 0
        SaveInterruptVector 28h, savedInt28
        InstallInterrupt 28h, Int28Handler
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: RestoreInt10
; Purpose: Restores saved int 10h
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro RestoreInt10 0
        RestoreInterruptVector 10h, savedInt10
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: RestoreInt28
; Purpose: Restores saved int 28h
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro RestoreInt28 0
        RestoreInterruptVector 28h, savedInt28
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: SetCustomExit
; Purpose: Sets the custom exit routine
; Arguments:
; %1 = custom exit routine
; Outputs: None
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro SetCustomExit 1
        mov [customExit], word %1
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: SaveInDOSAddr
; Purpose: Saves address of InDOS flag.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro SaveInDOSAddr 0
        mov ah, 34h
        call syscall
        push es
        push bx
        mov bx, InDOS
        pop word [bx]
        pop word [bx+2]
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: SaveCritErrAddr
; Purpose: Saves address of Critical Error flag.
;
; The "Critical Error Flag" (ErrorMode) is the
; first byte of the "Swappable Data Area" (SDA),
; so named because it can be swapped to enable
; DOS to be called re-entrantly. (This is used
; for DOS-based multitasking / task switching,
; such as "European MS-DOS 4.0", and also by
; network redirectors.) However, we don't care
; about any of that, we just want to check if
; the ErrorMode flag is non-zero, which means
; DOS is currently handling a critical error,
; and hence it might not be safe to call into
; DOS APIs right now.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro SaveCritErrAddr 0
        push ds
        mov ax, 5d06h
        mov ah, 34h
        call syscall
        mov [CritErrAddr], di
        mov [CritErrAddr+2], ds
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: GetInDOSFlag
; Purpose: Clear ZF if InDOS set, else set ZF
;      Assumes SaveInDOSAddr previously called
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro GetInDOSFlag 0
        mov es, [InDOS+2]
        mov bx, [InDOS]
        es mov ah, [bx]
        test ah, ah
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: GetCritErrFlag
; Purpose: Checks if critical error flag is set
;      Assumes SaveCritErrAddr previously called
;      Clear ZF if flag set, else set ZF
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro GetCritErrFlag 0
        mov es, [CritErrAddr+2]
        mov bx, [CritErrAddr]
        es mov ah, [bx]
        test ah, ah
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: IntCanWork
; Purpose: Determines if interrupt can call BIOS
;   and MS-DOS. Criteria:
;      1) InBIOS flag is zero AND
;      2) either InDOS is zero AND CritErr is zero
;           OR Int28 active
; Output: ZF set if we can do work,
;         ZF clear if system busy
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro IntCanWork 0
        cmp byte [InBios], 0
        jnz %%cant
        GetInDOSFlag
        jnz %%indos
        GetCritErrFlag
        jnz %%indos
        jmp %%can
%%indos:
        cmp byte [InIdle], 0
        jnz %%cant
%%can:
        SetFlag _ZF
        jmp %%end
%%cant:
        ClearFlag _ZF
%%end:
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: InterruptPrologue
; Purpose: Prologue for hardware interrupts
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro InterruptPrologue 0
        ; Save general purpose registers
        pusha

        ; Save orig DS register, then set DS=CS
        push ds
        push cs
        pop ds

        ; Save orig ES register, then set ES=CS
        push es
        push cs
        pop es
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro: InterruptEpilogue
; Purpose: Epilogue for hardware interrupts
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro InterruptEpilogue 0
        ; Restore ES register
        pop es

        ; Restore DS register
        pop ds

        ; Restore general purpose registers
        popa
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: putzhex16
; Purpose: Print 16-bit unsigned number in hex
;          with initial zero padding
; Input Registers:
;   DX = number to print
; Output Registers:
;   None
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
putzhex16:
        push dx
        mov dl, dh
        call putzhex8
        pop dx
        call putzhex8
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: ASCIIZLength
; Purpose: Find length of ASCIIZ string
; Input Registers:
;   BX = pointer to string (clobbered)
; Output Registers:
;   CX = length of string
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ASCIIZLength:
        mov cx, 0
.loop:
        cmp byte [bx], 0
        jz .end
        inc cx
        inc bx
        jmp .loop
.end:
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine: MemoryCopy
; Purpose: Copy memory between locations
; Input Registers:
;   CX = Bytes to copy
;   SI = Copy from
;   DI = Copy to
; Output Registers:
;   None (clobbers input + AX)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
MemoryCopy:
        jcxz .end
        mov al, [si]
        mov [di], al
        inc si
        inc di
        dec cx
        jmp MemoryCopy
.end:
        ret

; ***** DATA SECTION *****
dsect
eol:       db crlf
           db 0
mYes:
           db "Yes", 0
mNo:
           db "No", 0
bufCmdLine: times 129 db 0
savedVideoMode: db 0
savedVideoPage: db 0
restoreVideoOnExit: db 1
customExit: dw 0
InBios: db 0
InIdle: db 0
InDOS: dd 0
CritErrAddr: dd 0
savedInt10: dd 0
savedInt28: dd 0
csect
; **** END OF FILE ****
