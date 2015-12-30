## DOS Assembly Programs

These are a few programs I wrote in real mode x86 assembly for MS-DOS. They
don't really serve any practical purpose, I just wanted to have a go at writing
some programs in assembly so here they are.

I run DOS inside VirtualBox on Mac OS, so I have some shell scripts to build a
disk image etc. They probably run under Linux (or Cygwin) too, but haven't
tried them.

### Instructions
Assuming you have mtools installed, just run `./build.sh`. This will create a
1.44MB floppy image called `Build/zutl.img`. Now open that disk image in
MS-DOS, and run `ASMALL.BAT` (assumes nasm is in your path). This will assemble
all the programs.

Note that `build` wipes out your disk image, so if you edit the files on the
DOS side, you will lose all your changes if you run this. You can use
`fromFloppy.sh` to copy the changes from DOS back to Unix.

### The Programs
* `APMINFO` - checks if APM BIOS is present, if so prints the APM version
* `DUMPARGS` - prints out its own command line and the environment vars
* `INTDUMP` - dumps the interrupt vector table in hex format
* `PCTYPE` - prints PC type stored in ROM BIOS. Meaningless value except on
             1980s vintage IBM hardware.
* `POWEROFF` - uses APM to turn off the computer power
* `RS232MON` - shows RS-232 status flags in real-time. (I'm not sure whether
               values it displays are actually correct.)
* `RUNSHELL` - runs a subshell (i.e. child COMMAND.COM instance)
* `VCLS` - clears the screen
* `VMODE` - displays or changes video mode
* `VPAGE` - displays or changes video page
* `VPAUSE` - equivalent to the MS-DOS `PAUSE` command
* `ZUTL` - displays various system information
