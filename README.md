# retrogardens
16bit retro programming in DosBox with Borland C++ 3.1 Turbo Pascal 7 and TASM

![palette][pal]

## Introduction


## Prerequisites

DosBox, Borland C++ 3.1 and Turbo Pascal 7.0.

### Dosbox

#### Cpu cycles
A CPU speed of 50% works well.  
Click on the DosBox window with your mouse to grab the input, and press CTRL+F11 to decrease CPU cycles.  
Click CTRL+F10 to release the mouse again.


### Borland C++ 3.1

#### General advise
It is a good idea to not use the mouse when using the Borland C++ IDE.

Use the ALT key to quickly issue commands - for example use `ALT+P ENTER` to open a project, and `ALT+R Enter` to run your project.  
**Achtung!** Do not use `CTRL+F9` to run your project. You will disappoint yourself, because that is the DosBox terminate key combo.

#### Project settings


#### ASM and Turbo Assember

##### TASM.EXE
If Borland complains that it can't find `TASM.EXE`, then go into `Options  - Transfer` and find `~Turbo Assember` and Edit it.  
Enter `C:\TASM\BIN\TASM.EXE` in `Program Path` and then hit the `Modify` button.

##### TASM2MSG.EXE
If Borland complains that it can't find `TASM2MSG.EXE`, find it in `C:\BORLANDC\BIN` and place a copy in `C:\BORLANDC` and a copy next to the `.ASM` file that you want to link.


### Turbo Pascal 7.0

Because TP7 needs to be patched to run with faster CPUs, it is a good idea to use version 7.0, and not 7.1.

#### Divide by Zero CRT runtime bug
Turbo Pascal works, but will produce executables that crashes with a *divide by zero* error at runtime.  
It will do that on faster CPUs, unless we patch it.

Download the  [Crt patch for TP7][tppatch] - get the compile time patches - and extract.  
I used the first patch, and checked my TP7 version to figure out which patch to apply (it was the first one).

Copy the files to the `bin` directory and run `PATCH PATCH700` (or PATCH701, depending on the date of your TP7).





[tppatch]: http://www.ipnet6.org/tppatch.html "Crt patch for TP7"

[pal]: https://github.com/jacmoe/retrogardens/raw/master/screenshots/pal.jpg "Palette"
