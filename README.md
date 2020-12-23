**Update** [Retronew][retronew] is an attempt which follows the original code more closely, and without any external dependencies.
------------------------------------------------------------------------------------------------------------------------------------------------------

# retrogardens
16bit retro programming in DosBox with Borland C++ 3.1 Turbo Pascal 7 and TASM

![palette][pal]

## Introduction

--------------------------------

## Prerequisites

DosBox, Borland C++ 3.1 and Turbo Pascal 7.0.

You don't need both BCPP3.1 and TP7, so if you don't want to use Pascal, don't download and install TP7, or vice versa.

I installed TASM 5.5 also, although I am not sure if you really need to do that.  
However, below instructions assume that the Turbo Assember is installed into `C:\TASM`


Also, besure to check out Fabien Sanglard's excellent article [Let's Compile Like Its 1992][1992compile] about how to use DosBox and Borland C++ 3.1 to compile Id Software's Wolfenstein 3D, if you want a walkthrough with lots of picures and well written text.


### Where to get it
#### BC3.1 and TP7
If you don't already have BC3.1, TASM and TP7 lying around, then you can grab them from [WinWorld][winworld] - go to the *Library* section.

#### DosBox
Download it from [DosBox.com][dosbox]

--------------------------------

### Dosbox
While you can read (and you probably should) the [DosBox setup and installation wiki page][dosboxwiki] , here is a list of things that I did to get up and running.

Firstly, I simply installed DosBox.

#### C drive
First, we need a place for our development tools to be installed.

In other words, we need a `C:\` drive.

I simply created a directory creatively entitled `DosBox` in my home directory.


#### General configuration

You shouldn't really have to change anything, other than outlined below.

The configuration file for DosBox is `~/.dosbox/dosbox-0.74.conf` on Linux.

Look it up if you are on other platforms.

#### Autoexec.bat
Open up the dosbox configuration file and navigate to the end, where you will find the *autoexec* section.

```
[autoexec]
# Lines in this section will be run at startup.
# You can put your MOUNT lines here.
mount c ~/DosBox
path borlandc/bin;tasm/bin;tp7/tp/bin
c:
```
We mount the C drive, set up the path and change to the C drive.


#### Floppy disks
Since we are going to program like it's 1995, we need to deal with floppy disks.

So, since our machine does not have a floppy disk station, we use floppy disk images.  
And a directory that we mount as floppy in DosBox.

So, create a directory for the floppy drive, and add a line in the `autoexec` section of the DosBox configuration file, like so:
```
mount a -t floppy ~/DOS/box_disks/floppy
```

Now, what's in the floppy disk directory will be available from the A drive inside of DosBox.

**Insert and Change floppy disks**  
Simply open a floppy image ( `disk01.img` ) in a disk image viewer, select all files and copy them to the `floppy` directory.

When the installation program asks for the next disk, simply delete the files in `floppy` and copy the files of the next disk into it.

Very low tech. But it works! :p

#### CD ROMs
If you need to mount a CD drive, and you have an iso that you want to mount, then you can do that, like so:
```
imgmount d ~/DOS/WatcomCPP10.6/watcom10.6.iso -t iso
```
When you are done with it, simply remove the line, or comment it out (in case you want to mount another iso later).

I have no idea how to swap CDs - but this is DOS and 1995  and I don't recall ever having to swap CDs when installing. :)

#### Cpu cycles
A CPU speed of 50% works well.  
Press `Ctrl+F10` to grab the input, and press `CTRL+F11` to decrease CPU cycles.  
Click `CTRL+F10` to release the mouse again.

See the [DosBox - Special Keys][dosboxspecialkeys] wiki page for a list of key combos that you can use with it.

#### DosBox Window

If you want the DosBox to be larger, you can set the size in the `[sdl]` section.

My settings are:
```
windowresolution=1024x768
output=opengl
```

You can also set autolock for the mouse to be off:
```
autolock=true

```
Then the mouse will not be grabbed automatically by the DosBox window.  
You can use `Ctrl+F10` to grab / ungrab it.

#### DosBox Oddities
If you move files around on your C directory from outside of DosBox, you need to restart it to make DOS see it.

--------------------------------

### Borland C++ 3.1

#### General advise
It is a good idea to minimize the use of the mouse when using the Borland C++ IDE.

Don't click with your mouse in the DosBox window, if you don't want DosBox to grab it. Or turn off `autolock` (see above).

Use the ALT key to quickly issue commands - for example use `ALT+P ENTER` to open a project, and `ALT+R Enter` to run your project.  
**Achtung!** Do not use `CTRL+F9` to run your project, because that is the DosBox terminate key combo.

Use `ALT+[number]` to quickly switch between windows.

#### Project settings
When a project is open, enter the Options menu - `ALT+O` - and make sure that the following is set:

**Application**  
Dos Standard

**Code Generation**  
Model : Large  

**Advanced Code Generation**  
Floating Point : 80287/387  
Instruction Set : 80386  

**Entry/Exit Code Generation**  
Prolog/Epilog Code Generation : Dos standard  

**C++ Options**  
Use C++ Compiler : C++ always  

**Linker**  
Output : Standard DOS EXE  

**Debugger**
Program Heap Size : 350 K bytes  

**Directories**  
Modify the Include / Library directories as needed.  
Consider adding `C:\GARD\BC31\[project_dir]` to the Source Directories.  

My *Include Directories* is:
```
C:\BORLANDC\INCLUDE;C:\GARD\BC31\COMMON
```
My *Library Directories* is:
```
C:\BORLANDC\LIB;C:\GARD\BC31\COMMON
```

Make sure that you save the settings before exiting: `ALT+O S K`


#### ASM and Turbo Assember

#### Link shenanigans

In the `COMMON` directory, there are a couple of files in assembly code that needs to be built.

You can either do this, by first removing the object files from the projejct ( `del` ), and add the `.ASM` files ( `Ins` button in the Project window ) and build them ( `ALT+C L` ) and then remove them from the project again, and add the object ( `.OBJ` ) files again.

Or you can simply use Turbo Assembler on the command line.  
Don't ask me how to do that :p


##### TASM.EXE
If Borland complains that it can't find `TASM.EXE`, then go into `Options  - Transfer` and find `~Turbo Assember` and Edit it.  
Enter `C:\TASM\BIN\TASM.EXE` in `Program Path` and then hit the `Modify` button.

##### TASM2MSG.EXE
If Borland complains that it can't find `TASM2MSG.EXE`, find it in `C:\BORLANDC\BIN` and place a copy in `C:\BORLANDC` and a copy next to the `.ASM` file that you want to link.

--------------------------------

### Turbo Pascal 7.0

Because TP7 needs to be patched to run with faster CPUs, it is a good idea to use version 7.0, and not 7.1.

#### Divide by Zero CRT runtime bug
Turbo Pascal works, but will produce executables that crashes with a *divide by zero* error at runtime.  
It will do that on faster CPUs, unless we patch it.

Download the  [Crt patch for TP7][tppatch] - get the compile time patches - and extract.  
I used the first patch, and checked my TP7 version to figure out which patch to apply (it was the first one).

Copy the files to the `bin` directory and run `PATCH PATCH700` (or PATCH701, depending on the date of your TP7).


--------------------------------

## Credits
* General ideas and code from Christopher Lampton's book [Gardens of Imagination][gardens] (1994 - Waite Group)
* VGA code from David Brackeen's [256-Color VGA Programming in C][vgainc] tutorial.
* Keyboard code from Al Stevens' [Raycaster article - Dr. Dobb's Journal][raycast]

Some code from the *dos/programming* section of the [x2ftp.oulu.fi mirror][x2ftp]

--------------------------------


[retronew]: https://github.com/jacmoe/retronew "Retronew"

[1992compile]: http://fabiensanglard.net/Compile_Like_Its_1992/index.php "Compile Like Its 1992"

[winworld]: https://winworldpc.com/ "WinWorld"

[dosbox]: https://www.dosbox.com/download.php?main=1 "DosBox - Downloads"

[dosboxwiki]: https://www.dosbox.com/wiki/Basic_Setup_and_Installation_of_DosBox "DosBox Wiki - basic setup"

[dosboxspecialkeys]: http://www.dosbox.com/wiki/Special_Keys "DosBox Wiki - Special Keys"

[tppatch]: http://www.ipnet6.org/tppatch.html "Crt patch for TP7"

[pal]: https://github.com/jacmoe/retrogardens/raw/master/screenshots/pal.jpg "Palette"

[gardens]: https://www.amazon.com/Gardens-Imagination-Programming-Maze-Games/dp/187873959X "Gardens of Imagination"

[raycast]: http://www.drdobbs.com/windows/windows-casting-rays-and-developer-days/windows/sourcecode/windows-casting-rays-and-developer-days/30300258 "Raycaster"

[vgainc]: http://www.brackeen.com/vga/index.html "256-Color VGA Programming in C"

[x2ftp]: http://ftp.lanet.lv/ftp/mirror/x2ftp/msdos/programming/ "x2ftp.oulu.fi mirror"
