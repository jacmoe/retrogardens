
; SCREEN.ASM
;
; A set of assembly language functions for accessing the mode 13h
; video display.
;
; Copyright 1993 by Christopher Lampton and The Waite Group Press

		.MODEL	large
		.CODE
		PUBLIC	_cls,_setmode,_setpalette,_putwindow, _clrwin, _blitscreen
						PUBLIC  _transput, _ctransput, _putimage

SCREEN_WIDTH	EQU	320
SCREEN_HEIGHT	EQU 200

; cls(char far *screen_adr)
;   Clear video memory or offscreen buffer at
;   SCREEN_ADR to zeroes

_cls			PROC
		ARG		screen:DWORD
		push	bp              ; Save BP
		mov		bp,sp           ; Set up stack pointer
		push	di              ; Save DI register
    les   di,screen       ; Point ES:DI at screen
		mov		cx,SCREEN_WIDTH/2 * SCREEN_HEIGHT ; Count pixels
		mov		ax,0            ; Store zero values...
		rep		stosw           ; ...in all of video memory
		pop		di              ; Restore DI
		pop		bp              ; Restore BP
		ret
_cls			ENDP

; setmode(int mode)
;   Set VGA adapter to BIOS mode MODE

_setmode	PROC
		ARG		mode:WORD
		push	bp              ; Save BP
		mov		bp,sp           ; Set up stack pointer
		mov		ax,mode         ; AX = video mode
		mov		ah,0            ; AH = function number
		int		10h             ; Call video BIOS
		pop		bp              ; Restore BP
		ret
_setmode	ENDP

; setpalette(char far *color_regs,int firstreg,int numregs)
;   Set VGA color registers, beginning with FIRSTREG and
;	  continuing for NUMREGS to the color values in COLOR_REGS

_setpalette	PROC
    ARG   regs:DWORD
    push  bp              ; Save BP
    mov   bp,sp           ; Set up stack pointer
    les   dx,regs         ; Point ES:SX at palette registers
    mov   ah,10h          ; Specify BIOS function 10h
    mov   al,12h          ; ...subfunction 12h
    mov   bx,0            ; Start with first register
    mov   cx,100h         ; Set all 256 (100h) registers
    int   10h             ; Call video BIOS
    pop   bp              ; Restore BP
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

;  blitscreen( void far* buffer )
;    move a full 320 x 200 frame to vide ram from buffer

_blitscreen	PROC
		ARG	buffer:DWORD
		push	bp
		mov		bp,sp
        push    es
		push	ds
		push	di
		push	si
        mov     ax, 0a000h
        mov     es, ax
        xor     di, di
        lds     si, buffer
        mov     cx, 32000
        cld
        rep     movsw
		pop		si
		pop		di
		pop		ds
        pop     es
		pop		bp
		ret
_blitscreen 	ENDP

; void putimage( int x1, int y1, int xdim, int ydim, void far* sbuf,
;                void far* dbuf);
; Copies the contents of the linear buffer sbuf to a rectangular area of
; the buffer at dbuf. NOTE: the buffer at dbuf is assumed to be a 64000
; byte offscreen frame buffer 320 x 200 in dimension

_putimage       PROC
        ARG x1:WORD, y1:WORD, xdim:WORD, ydim:WORD, sbuf:DWORD, dbuf:DWORD
        push    bp
        mov     bp, sp

	    push	ax
	    push	bx
        push	cx
	    push	ds
        push	si
	    push	es
	    push	di

	    cmp 	xdim, 00
	    je      pf_02
	    cmp 	ydim, 00
	    je		pf_02

        les     di, dbuf
        lds     si, sbuf
	    mov		ax, y1
	    mov	    bx, 320
	    mul		bx
	    add		di, ax
	    add		di, x1
	    mov		cx, ydim
	    cld
pf_01:
				push	cx
        push	di
	    mov	    cx, xdim
	    shr		cx, 1
	    rep		movsw
        rcl		cx, 1
	    rep		movsb
	    pop		di
	    add		di, 320
	    pop		cx
	    loop	pf_01
pf_02:
	    pop		di
	    pop		es
	    pop		si
	    pop		ds
	    pop		cx
	    pop		bx
	    pop		ax
        pop     bp
        ret
_putimage       ENDP

;  void transput( int x1, int y1, int x2, int y2, void far* sbuffer,
;                 void far* dbuffer );
; NOTE: assumes dbuffer is 64000 byte 320 x 200 image buffer, and sbuffer
; is a linear buffer holding the image data to be displayed transparently

_transput	    PROC
		ARG	x1:WORD, y1:WORD, x2:WORD, y2:WORD, sbuffer:DWORD, dbuffer:DWORD
		push	bp
		mov		bp,sp
        push    bx
        push    cx
				push    es
		push	ds
		push	di
		push	si

        les     di, dbuffer
        lds     si, sbuffer
        mov     ax, 320
        mov     bx, y1
        mul     bx
        add     ax, x1
        add     di, ax
        mov     ax, y2
        sub     ax, y1
        inc     ax
        mov     cx, ax
        mov     bx, x2
        sub     bx, x1
        inc     bx
nextln:
        push    cx
        mov     cx, bx
        push    di
drawln:
        cmp     byte ptr [ds:si], 0
        je      skip
        mov     al, byte ptr [ds:si]
        mov     byte ptr [es:di], al
skip:
        inc     si
        inc     di
        loop    drawln
        pop     di
        add     di, 320
        pop     cx
        loop    nextln

		pop		si
		pop		di
		pop		ds
        pop     es
        pop     cx
        pop     bx
		pop		bp
		ret
_transput   	ENDP

;  void ctransput( void far* sbuffer, void far* dbuffer);
; NOTE: assumes a 64000 byte 320 x 200 image

_ctransput	    PROC
		ARG	sbuffer:DWORD, dbuffer:DWORD
		push		bp
		mov			bp,sp
		push    bx
		push    cx
		push    es
		push		ds
		push		di
		push		si
		les     di, dbuffer						; Point ES:DI at destination buffer
		lds     si, sbuffer						; Point DS:SI at source buffer
		mov			bx,0
		mov			ah,0                  ; Be sure AH is clean
		cld
looptop:
		mov			al,BYTE PTR [ds:si]		; Get runlength byte
		and			al,128								; Repeat run or random run?
		jz			randrun								; If random run, skip ahead
		mov			al,BYTE PTR [ds:si]		; Else get runlength byte again
		and			al,127								; Remove high bit of count
		inc			si										; Point to repeat value
		mov			ah,0
		add			bx,ax									; Count pixels in BX
		inc			si									  ; Point to next runlength byte
		mov			ah,0
		add			di,ax                 ; Move destination pointer past zero pixels
		jmp			endloop
randrun:                        	; Handle random run
		mov			al,BYTE PTR [ds:si]		; Get runlength in AL
		mov			ah,0
		add			bx,ax       					; Count pixels in BX
		mov			cl,al         				; Get repeat count in CX
		mov			ch,0
		inc			si                    ; Point to first byte of run
		shr			cx,1									; Divide runlength by 2
		jz			randrun2							; If zero, skip ahead
		rep			movsw								  ; Otherwise, move run of pixels
randrun2:
		jnc			endloop 							; Jump ahead if even
		movsb													; Else move the odd byte
endloop:
		cmp			bx,64000              ; Have we done all 64000?
		jb  		looptop								; If not, go to top of loop
done:
		pop			si
		pop			di
		pop			ds
		pop     es
		pop     cx
		pop     bx
		pop			bp
		ret
_ctransput   	ENDP

		END
