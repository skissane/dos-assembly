%include "libzutl.inc"

start:
        putz mPressAnyKey
        BiosGetKey
        putz eol
        exit 0
mPressAnyKey:
        db "Press any key to continue...", 0
