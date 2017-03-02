
; IO.ASM
;   Low level input/output support routines for event manager
;
; Copyright 1993 by Christopher Lampton and The Waite Group Press

  .MODEL	large
  .CODE
  PUBLIC	_readstick,_readjbutton
  PUBLIC	_initmouse,_readmbutton
  PUBLIC	_rempointer,_relpos
  PUBLIC	_scankey

TIMER			equ 0040h
GAMEPORT 	equ	0201h

_readstick  	PROC
; Read current position of joystick on axis specified by BMASK
  ARG  bmask:WORD
  push bp
  mov	 bp,sp
  cli      						; Turn off interrupts, which could
                      ;  effect timing
  mov	 ah,byte ptr bmask	; Get bitmask into ah.
  mov	 al,0
  mov	 dx,GAMEPORT    ; Point DX at joystick port
  mov	 cx,0						; Prepare to loop 65,536 times
  out  dx,al					; Set joystick bits to 1
loop2:
  in	 al,dx					; Read joystick bits
  test al,ah					; Is requested bit (in bitmask)
                      ;  still 1?
  loopne loop2				; If so (and maximum count isn't
                      ;  done), try again
  sti									; Count is finished, so reenable
                      ;  interrupts
  mov	 ax,0
  sub	 ax,cx          ; Subtract CX from zero, to get count
  pop	 bp
  ret
_readstick		ENDP

_readjbutton	PROC
; Read joystick button specified by BMASK
  ARG  bmask:WORD
  push bp
  mov  bp,sp
  mov  dx,GAMEPORT		; Point DX at joystick port
  mov  ah,0						; Zero high byte of return value
  out  dx,al          ; Request data from port
  in   al,dx					; Get value from joystick
  not  al							; Flip button bits
  mov  bx,bmask				; Mask out all but requested buttons
  and  al,bl					; And leave result in AX
  pop  bp
  ret
_readjbutton	ENDP

_initmouse	PROC
; Call mouse driver initialization routine
  mov  ax,0		        ; Request function zero (initialize)
  int  33h						; Call mouse driver
  ret
_initmouse	ENDP

_disppointer	PROC
; Display mouse pointer
  mov  ax,1						; Request function 1 (display
                      ;  pointer)
  int	 33h						; Call mouse driver
  ret
_disppointer	ENDP

_rempointer	PROC
; Remove mouse pointer
  mov  ax,2						; Request function 2 (remove
                      ;  pointer)
  int	 33h						; Call mouse driver
  ret
_rempointer	ENDP

_readmbutton	PROC
; Read mouse button
  mov	 ax,3   				; Request function 3 (read buttons)
  int	 33h						; Call mouse driver
  mov	 ax,bx					; Put result in function return
                      ;  register
  ret
_readmbutton	ENDP

_relpos   PROC
; Get changes in mouse position relative to last call
  ARG   x:DWORD,y:DWORD
  push  bp
  mov   bp,sp
  mov   ax,000bh      ; Request function 0bh
                      ;  (relative mouse position)
  int   33h           ; Call mouse driver
  les   bx,x          ; Point es:bx at x parameter
  mov   [es:bx],cx    ; ...and store relative position
  les   bx,y          ; Point es:bx at y parameter
  mov   [es:bx],dx    ; ...and store relative position
  pop   bp
  ret
_relpos   ENDP

_scankey  PROC
; Get scan code of last key pressed
  mov   ah,1					; Specify function 1 (key status)
  int   16h						; Call BIOS keyboard interrupt
  jz    nokey					; If no key pressed, skip ahead
  mov   ah,0					;	Else specific function 0 (get
                      ;  last key code)
  int   16h						; Call BIOS keyboard interrupt
  mov   al,ah					; Get scan code into low byte of AX
  mov   ah,0					; Zero high byte of AX
  ret             		; Return value to C program
nokey:								; If no key pressed
  mov   ax,0					; Return value of zero
  ret
_scankey  ENDP

	END
