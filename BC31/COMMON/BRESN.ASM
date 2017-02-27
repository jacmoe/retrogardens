
; BRESNHAM.ASM
;
; A function for drawing a line between two
; arbitrary points in a specified color to
; an offscreen buffer.
;
; Copyright 1993 by Christopher Lampton and
;  The Waite Group Press

		.MODEL  large
		.CODE
		PUBLIC  _linedraw

_linedraw   PROC
		ARG   x1:WORD,y1:WORD,x2:WORD,y2:WORD,color:WORD,\
            scr_off:WORD,scr_seg:WORD
		LOCAL y_unit:WORD,x_unit:WORD,xdiff:WORD,ydiff:WORD,\
            error_term:WORD=AUTO_SIZE
		push  bp
		mov   bp,sp
		sub   sp,AUTO_SIZE
		mov   ax,scr_seg    ; Get screen segment in ax
		mov   es,ax
		mov   ax,y1         ; Get y1 in ax...
		mov   dx,320        ; Multiply by 320
		mul   dx
		add   ax,x1         ; And add x1 to get pixel offset
		add   ax,scr_off    ; Add screen offset
		mov   bx,ax         ; Move offset to BX
init_line:
		mov   dx,color      ; Put pixel color in dx
		mov   error_term,0  ; Initialize error term
		mov   ax,y2         ; Determine sign of y2-y1
		sub   ax,y1
		jns   ypos          ; If positive, jump
		mov   y_unit,-320   ; Else handle negative case
		neg   ax            ; Get absolute value of YDIFF
		mov   ydiff,ax      ; And store it in memory
		jmp   next
ypos:
		mov   y_unit,320    ; Handle positive case
		mov   ydiff,ax      ; Store YDIFF in memory
next:
		mov   ax,x2         ; Determine sign of x2-x1
		sub   ax,x1
		jns   xpos          ; If positive, jump
		mov   x_unit,-1     ; Else handle negative case
		neg   ax            ; Get absolute value of XDIFF
		mov   xdiff,ax      ; And store it in memory
		jmp   next2
xpos:
		mov   x_unit,1      ; Handle positive case
		mov   xdiff,ax      ; Store XDIFF in memory
next2:
		cmp   ax,ydiff      ; Compare XDIFF (in AX) and YDIFF
		jc    yline         ; IF XDIFF<YDIFF then count
                        ;   in Y dimension
		jmp   xline         ; Else count in X dimension
;
xline:
; Line with slope less than 1
		mov   cx,xdiff      ; Get line length in cx for count
		inc   cx
xline1:
		mov   es:[bx],dl    ; Draw next point in line
		add   bx,x_unit     ; Point offset to next pixel
                        ;   in x direction
		mov   ax,error_term ; Check to see if move required
                        ;   in Y direction?
		add   ax,ydiff
		mov   error_term,ax
		sub   ax,xdiff
		jc    xline2        ; If not, continue
		mov   error_term,ax
		add   bx,y_unit     ; Else, move up or down one pixel
xline2:
		loop  xline1        ; Loop until count (in CX) complete
		jmp   linedone
;
yline:
; Line with slope >= 1
		mov   cx,ydiff      ; Get line length in cx
		inc   cx
yline1:
		mov   es:[bx],dl    ; Draw next point in line
		add   bx,y_unit     ; Point offset to next pixel
                        ;   in Y direction
		mov   ax,error_term ; Check to see if move require
                        ;   in X direciton
		add   ax,xdiff
		mov   error_term,ax
		sub   ax,ydiff
		jc    yline2        ; If not, continue
		mov   error_term,ax
		add   bx,x_unit     ; Else, move left or right
                        ;   one pixel
yline2:
		loop  yline1        ; Loop until count (in CX)
                        ;   complete
linedone:
; Line is finished, clean up the stack
		mov   sp,bp         ; Finished!
		pop   bp
		ret
_linedraw   ENDP

	END
