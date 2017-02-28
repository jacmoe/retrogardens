# retrogardens
16bit retro programming in DosBox with Borland C++ 3.1 Turbo Pascal 7 and TASM

![palette][pal]


## Prerequisites

### Dosbox



### Borland C++ 3.1

#### ASM and Turbo Assember

##### TASM.EXE
If Borland complains that it can't find `TASM.EXE`, then go into `Options  - Transfer` and find `~Turbo Assember` and Edit it.  
Enter `C:\TASM\BIN\TASM.EXE` in `Program Path` and then hit the `Modify` button.

##### TASM2MSG.EXE
If Borland complains that it can't find `TASM2MSG.EXE`, find it in `C:\BORLANDC\BIN` and place a copy in `C:\BORLANDC` and a copy next to the `.ASM` file that you want to link.


### Turbo Pascal 7.0

[Crt patch for TP7][tppatch]






[tppatch]: http://www.ipnet6.org/tppatch.html "Crt patch for TP7"

[pal]: https://github.com/jacmoe/retrogardens/raw/master/screenshots/pal.jpg "Palette"
