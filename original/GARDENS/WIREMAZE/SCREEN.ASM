
		.MODEL	large
		.CODE
;		.386
		PUBLIC	_cls,_setmode,_setpalette,_putwindow,_clrwin;

SCREEN_WIDTH	EQU	320
SCREEN_HEIGHT	EQU 200

; cls(char far *screen_adr)
;   Clear video buffer at offset, segment

_cls			PROC
		ARG		scr_off:WORD, scr_seg:WORD
		push	bp
		mov		bp,sp
		push	di
		mov		di,scr_seg
		mov		es,di
		mov		di,scr_off
		mov		cx,SCREEN_WIDTH/2 * SCREEN_HEIGHT
		mov		ax,0
		rep		stosw
		pop		di
		pop		bp
		ret
_cls			ENDP

_setmode	PROC
		ARG		mode:WORD
		push	bp
		mov		bp,sp
		mov		ax,[mode]
		mov		ah,0
		int		10h
		pop		bp
		ret
_setmode	ENDP

; setpalette(char far *color_regs,int firstreg,int numregs)
;   Set VGA color registers, beginning with FIRSTREG and
;	  continuing for NUMREGS to the color values in COLOR_REGS

_setpalette	PROC
		ARG   regoff:WORD,regseg:WORD,firstreg:WORD,numregs:WORD
		push	bp
		mov		bp,sp
		mov		dx,es
		push	dx
		mov		dx,regseg
		mov		es,dx
		mov		ah,10h
		mov		al,12h
		mov		bx,0
		mov		cx,100h
		mov		dx,regoff
		int		10h
		pop		dx
		mov		es,dx
		pop		bp
		ret
_setpalette	ENDP

;  putwindow(xpos,ypos,xsize,ysize,offset,segment)
;    Move rectangular area of screen buffer at offset, segment
;    with upper left corner at xpos,ypos, width xsize and height ysize

_putwindow	PROC
		ARG	xpos:WORD,ypos:WORD,xsize:WORD,ysize:WORD,buf_off:WORD,buf_seg:WORD
		push	bp
		mov		bp,sp
		push	ds
		push	di
		push	si
		mov		ax,ypos
		mov		dx,320
		mul		dx
		add		ax,x1
		mov   di,ax
		add		ax,buf_off
		mov		si,ax
		mov		dx,0a000h			 ; Get screen segment in ES
		mov		es,dx
		mov		dx,buf_seg		 ; Get screen buffer segment in DS
		mov		ds,dx
		mov		dx,ysize       ; Get line count into DX
		cld
ploop1:
		mov		cx,xsize       ; Get pixel width of window into CX
		shr		cx,1
		push  di             ; Save screen and buffer addresses
		push	si
		rep		movsw          ; Move one line of window to screen
		pop		si             ; Restore screen and buffer addresses
		pop		di
		add		si,320         ; ...and advance them to next line
		add		di,320
		dec		dx             ; Count off one line
		jnz		ploop1         ; If more lines in window, loop back and draw them
		pop		si
		pop		di
		pop		ds
		pop		bp
		ret
_putwindow	ENDP

; clrwin(x1,y1,w,h,offset,segment)
;   Clear rectangular window in screen buffer at offset, segment
;   with upper left corner at x1,y1, width w and height h

_clrwin		PROC
		ARG		x1:WORD, y1:WORD, w:WORD, h:WORD, scr_off:WORD, scr_seg:WORD
		push	bp
		mov		bp,sp
		push	di
		mov		cx,w	; Get width of window in CX
		shr		cx,1
		mov		bx,h	; Get height of window in BX
		mov		ax,y1	; Get offset for upper left corner
		mov		dx,320
		mul 	dx
		add		ax,x1
		add		ax,scr_off
		mov		di,ax
		mov		ax,scr_seg
		mov		es,ax
		mov		ax,0
cwloop:
		push	cx
		push	di
		rep		stosw
		pop		di
		pop		cx
		add		di,320
		dec		bx
		jnz		cwloop
		pop		di
		pop		bp
		ret
_clrwin		ENDP

		END
