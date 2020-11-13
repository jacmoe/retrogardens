
; IO.ASM
;   Low level input/output support routines for event manager
;
; Copyright 1993 by Christopher Lampton and The Waite Group Press

  .MODEL	large
  .CODE
  PUBLIC	_readstick,_readjbutton
  PUBLIC	_initmouse,_readmbutton
  PUBLIC	_rempointer,_relpos
  PUBLIC	_initkey,_remkey

TIMER			equ 0040h
GAMEPORT 	equ	0201h
KBPORT    equ 60h

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
  mov	 ax,0
  sub	 ax,cx          ; Subtract CX from zero, to get count
  push ax             ; Save count
  mov  dx,GAMEPORT
  mov  cx,65535
  mov  bl,3
loop3:
  in   al,dx
  test al,bl
  loopne loop3
  pop  ax             ; Restore count
  sti									; Count is finished, so reenable
                      ;  interrupts
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

intofs dw 0
intseg dw 0
bufseg   dw 0
bufofs   dw 0

_initkey  PROC
; Initialize keyboard interrupt handler
  ARG   buffer:DWORD    ; Pointer to scan code buffer
  push  bp              ; Save BP register
  mov   bp,sp
  push  es              ; Save ES and DS registers
  push  ds
  mov   ah,35h          ; Call INT 21h Function 35h
  mov   al,09h          ; ...to get current address of
  int   21h             ; ...interrupt 09h
  mov   intseg,es       ; Save segment
  mov   intofs,bx       ; ...and offset
  mov   dx,seg _newkey  ; Put the segment of new
  mov   ds,dx           ; ...interrupt handler in DS
  mov   dx,offset _newkey ; Plus offset in DX
  mov   ah,25h          ; Call INT 21h Function 25h
  mov   al,09h          ; ...to install new handler
  int   21h
  les   dx,buffer       ; Save address of new scan code buffer
  mov   bufseg,es
  mov   bufofs,dx
  pop   ds              ; Restore registers
  pop   es
  pop   bp
  ret
_initkey  ENDP

_remkey PROC
  push  ds              ; Save DS register
  mov   dx,intseg       ; Get segment and offset of
  mov   ds,dx           ; ...old interrupt handler
  mov   dx,intofs
  mov   ah,25h          ; Call INT 21h Function 25h
  mov   al,09h          ; ...to restore old handler
  int   21h
  pop   ds              ; Restore DS register
  ret
_remkey ENDP

_newkey  PROC FAR
  push  ax              ; Save all registers used
  push  di
  push  bp
  push  es
  mov   ax,bufseg       ; Point ES:DI at scan code buffer
  mov   es,ax
  mov   di,bufofs
  in    al,60h          ; Get the latest scan code
  mov   ah,0            ; Zero high byte of AX
  cmp   al,128          ; Was code a "make" or a "break"?
  jnb   break           ; If break, skip ahead
  mov   bp,ax           ; Else use code as index into buffer
  mov   [byte ptr es:di+bp],1  ; And flag key as pressed
  jmp   endkey
break:
  and   al,127          ; If break code, remove high bit
  mov   bp,ax           ; Use code as index into buffer
  mov   [byte ptr es:di+bp],0 ; Flag key as released
endkey:
  in   al,61h           ; Clear the keyboard interrupt
  mov  ah,al
  or   al,80h
  out  61h,al
  mov  al,20h           ; Allow additional interrupts
  out  20h,al
  pop  es               ; Restore registers
  pop  bp
  pop  di
  pop  ax
  iret
_newkey  ENDP

	END
