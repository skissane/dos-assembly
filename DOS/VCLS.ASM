%include "libzutl.inc"

start:
        call BiosClearScreen
        BiosHomeCursor
        putz mScreenCleared
        exit 0
mScreenCleared:
        db "Screen cleared", crlf,0
