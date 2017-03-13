
; SCREEN.ASM
; Assembler functions for graphics and video control
;
; Copyright 1993 by Christopher Lampton and The Waite Group Press

		.MODEL	large
		.CODE

    	PUBLIC	_cls,_setgmode,_setpalette,_putwindow
      PUBLIC  _clrwin, _blitscreen, _transput, _ctransput
		  PUBLIC  _putimage,_fadepalin, _fadepalout
      PUBLIC  _detectvga

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

_setgmode	PROC
		ARG		mode:WORD
		push	bp
		mov		bp,sp
		mov		ax,[mode]
		mov		ah,0
		int		10h
		pop		bp
		ret
_setgmode	ENDP

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
;    with upper left corner at xpos,ypos, width xsize and
;    height ysize

_putwindow	PROC
		ARG	xpos:WORD,ypos:WORD,xsize:WORD,ysize:WORD,\
        buf_off:WORD,buf_seg:WORD
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
		mov		dx,0a000h		 ; Get screen segment in ES
		mov		es,dx
		mov		dx,buf_seg	 ; Get screen buffer segment in DS
		mov		ds,dx
		mov		dx,ysize     ; Get line count into DX
		cld
ploop1:
		mov		cx,xsize     ; Get pixel width of window into CX
		shr		cx,1
		push  di           ; Save screen and buffer addresses
		push	si
		rep		movsw        ; Move one line of window to screen
		pop		si           ; Restore screen and buffer addresses
		pop		di
		add		si,320       ; ...and advance them to next line
		add		di,320
		dec		dx           ; Count off one line
		jnz		ploop1       ; If more lines in window, loop back
                       ;   and draw them
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
		mov		cx,w	         ; Get width of window in CX
		shr		cx,1
		mov		bx,h	         ; Get height of window in BX
		mov		ax,y1	         ; Get offset for upper left corner
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
		les     di, dbuffer				 ; Point ES:DI at destination buffer
		lds     si, sbuffer				 ; Point DS:SI at source buffer
		mov			bx,0
		mov			ah,0                 ; Be sure AH is clean
		cld
looptop:
		mov			al,BYTE PTR [ds:si]	 ; Get runlength byte
		and			al,128				 ; Repeat run or random run?
		jz			randrun				 ; If random run, skip ahead
		mov			al,BYTE PTR [ds:si]	 ; Else get runlength byte again
		and			al,127				 ; Remove high bit of count
		inc			si					 ; Point to repeat value
		mov			ah,0
        add			bx,ax				 ; Count pixels in BX
		inc			si					 ; Point to next runlength byte
		mov			ah,0
		add			di,ax                ; Move destination pointer past zero pixels
		jmp			endloop
randrun:                        	     ; Handle random run
		mov			al,BYTE PTR [ds:si]	 ; Get runlength in AL
		mov			ah,0
		add			bx,ax       		 ; Count pixels in BX
		mov			cl,al         		 ; Get repeat count in CX
		mov			ch,0
		inc			si                   ; Point to first byte of run
		shr			cx,1				 ; Divide runlength by 2
		jz			randrun2			 ; If zero, skip ahead
		rep			movsw				 ; Otherwise, move run of pixels
randrun2:
		jnc			endloop 			 ; Jump ahead if even
		movsb							 ; Else move the odd byte
endloop:
		cmp			bx,64000             ; Have we done all 64000?
		jb  		looptop				 ; If not, go to top of loop
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

; extern "C" void fadepalin(int start, int count, const byte* palette)
;
; This function fades the vga palette from 0 to the values in palette
; using a 64 pass incremental RGB fade.
;
; note: start and count are in the range 0..255, and denote the first
; palette register to process, and the number of subsequent registers
; to process. The palette argument points to an array of 768 bytes of
; DAC palette data. Only the low six bits of each byte are significant.
; Palette registers outside the specified range are not affected.

_fadepalin      PROC
        ARG start: WORD, count: WORD, palette: DWORD
        push    bp
        mov     bp, sp
        push    es                ; we use just about everything
        push    ds
        push    di
        push    si
        push    dx
        push    cx
        push    bx
        jmp     go

        pal     db 768 dup (20)   ; working palette area

go:
        ; setup for and make bios call to load the current dac palette
        ; into the table at pal

        push    cs                ; get code segment into es
        pop     es
        mov     dx, offset pal    ; es:dx points to start of pal
        push    dx                ; save offset of pal
        xor     bx, bx            ; need to get the current palette into
        mov     cx, 100h          ; pal using bios int 10h, 10h, 17h
        mov     ax, 1017h         ; set up and generate the interrupt
        int     10h               ; read the palette registers into pal
        pop     di                ; offset of pal, was in dx!

        ; get the palette pointer, then calculate the offset to the
        ; first byte processed, based on start, and the number of
        ; bytes to process based on count. Use the offset to adjust
        ; the string pointers in si and di, and leave the byte
        ; count in ax

        lds     si, palette       ; get address of target palette
        mov     ax, start         ; get offset of first palette byte to
        mov     bx, 3             ; be processed
        mul     bx
        add     si, ax            ; adjust di and si point first byte in the
        add     di, ax            ; target and temporary palettes
        mov     ax, count         ; find the number of bytes to be processed
        mov     bx, 3
        mul     bx                ; leave it in ax

        ; clear the bytes in the triplets which will be operated on by
        ; the fade. All other registers are unaffected

        push    di                ; save the starting offset into pal
        push    ax                ; save the number of bytes to process
        mov     cx, ax            ; set up a loop counter
        xor     ax, ax            ; clear ax
        rep     stosb             ; fill relevant range of pal with 0's
        pop     ax                ; restore the number of bytes to process
        pop     di                ; restore the starting offset into pal
        mov     cx, 64            ; 64 passes through fade loop

        ; the fade loop will execute 64 times. On each pass the inner
        ; loop adjusts the working palette, then waits for a blanking
        ; interval, and loads the working palette into the DAC

fade_loop:
        push    cx                ; save the fade loop counter
        push    di                ; save offset of first byte processed in
        push    si                ; temp and target palettes
        mov     bl, cl            ; outer loop count into bl
        mov     cx, ax            ; load number of bytes to process into cx

        ; inner loop makes one pass through the palette for each pass
        ; through the outer loop. Each byte is incremented if it's
        ; target value is one greater than the outer loop count. Using
        ; this logic ensures that all bytes arrive at their target values
        ; on the same pass through the outer loop

pal_cmp_loop:
        cmp     bl, ds:[si]       ; start incrementing when palette value
        jns     no_add            ; is one greater than loop count
        inc     BYTE PTR es:[di]
no_add:
        inc     si                ; point to the next byte in both palettes
        inc     di
        loop    pal_cmp_loop      ; do the next byte

        ; setup for palette load. As much as possible was moved above the
        ; blanking interval wait, in order to maximize the amount of the
        ; blanking interval remaining in which to do the palette loading

        mov     bx, sp
        mov     di, ss:[bx+02]      ; restore offset into pal without popping
        mov     cx, count           ; number of triplets to process
        push    ax                  ; need to use ax for port i/o

        ; monitor bit 1 of CRT controller's input status 1 register to
        ; sense a vertical blanking interval. Wait for any current vbi
        ; to end, then wait for the next full one to begin.

	    mov dx, 03DAh               ; CRT controller input status 1 register
vbi_1:
		in al, dx                   ; watch vertical blanking bit
		test al,08h                 ; wait for it to clear to make sure
		jnz vbi_1                   ; we're not in a blanking interval
vbi_2:
		in al, dx                   ; now wait for the start of the
		test al,08h                 ; next blanking interval
		jz vbi_2

        ; load the relevant triplets from pal into the VGA DAC palette

        mov     ah, BYTE PTR start  ; get first register to process into ah
        mov     dx, 03c8h           ; DAC palette index register
pal_load_loop:
        mov     al, ah              ; get next palette number to write
        out     dx, al              ; write the register number to the dac
        inc     dx                  ; address dac data register
        mov     al, BYTE PTR [es:di] ; get first byte of triplet
        out     dx, al               ; write it to the dac data register
        inc     di                   ; point to second byte
        mov     al, BYTE PTR [es:di] ; get second byte of triplet
        out     dx, al               ; write it to the dac data register
        inc     di                   ; point to third byte
        mov     al, BYTE PTR [es:di] ; get third byte of triplet
        out     dx, al               ; write it to the dac data register
        inc     di                   ; point to first byte of next triplet
        dec     dx                   ; address the dac index register
        inc     ah                   ; point to next palette register
        loop    pal_load_loop        ; process next triplet

        ; clean-up for the next pass through the fade loop

        pop     ax                ; restore ax
        pop     si                ; restore the offset into palette
        pop     di                ; restore the offset into pal
        pop     cx                ; restore the fade loop counter
        loop    fade_loop         ; do the next pass through the fade loop

        pop     bx
        pop     cx
        pop     dx
        pop     si
        pop     di
        pop     ds
        pop     es
        pop     bp
        ret
_fadepalin      ENDP

; extern "C" void fadepalout(int start, int count)
;
; This function fades the vga palette from the values in palette
; to 0 using a 64 pass incremental RGB fade. Similar in execution to
; _fadepalin above.
;
; note: start and count are in the range 0..255, and denote the first
; palette register to process, and the number of subsequent registers
; to process. The palette argument points to an array of 768 bytes of
; DAC palette data. Only the low six bits of each byte are significant.
; Palette registers outside the specified range are not affected.

_fadepalout     PROC
        ARG start: WORD, count: WORD
        push    bp
        mov     bp, sp
        push    es
        push    ds
        push    di
        push    si
        push    dx
        push    cx
        push    bx
        jmp     o_go
        opal    db 768 dup (20)   ; temporary palette area
o_go:
        push    cs                ; get code segment into es
        pop     es
        mov     dx, offset opal   ; es:dx points to start of opal
        push    dx                ; save offset of opal
        xor     bx, bx
        mov     cx, 100h
        mov     ax, 1017h         ; bios read dac registers function
        int     10h               ; read the palette registers into opal
        pop     di                ; offset of opal, was in dx!
        mov     ax, start         ; get offset of first palette byte to
        mov     bx, 3             ; be processed
        mul     bx
        add     di, ax            ; adjust offset into opal
        mov     ax, count         ; find the number of bytes to be processed
        mov     bx, 3
        mul     bx                ; leave it in ax
        mov     cx, 64            ; 64 passes through fade loop
o_fade_loop:
        push    cx                ; save the fade loop counter
        push    di                ; save offset of first byte processed in
        mov     bl, cl            ; we'll use the pass number as a threshold
        mov     cx, ax            ; load number of bytes to process into cx
o_pal_cmp_loop:
        cmp     bl, es:[di]       ; start decrementing when palette value
        jnz     o_no_dec          ; is equal loop count (it will stay equal
        dec     BYTE PTR es:[di]  ; to loop count for the rest of this pass)
o_no_dec:
        inc     di
        loop    o_pal_cmp_loop      ; do the next byte

        mov     bx, sp              ; need the stack pointer for a moment
        mov     di, ss:[bx]         ; restore offset into pal without popping
        mov     cx, count           ; number of triplets to process
        push    ax                  ; need to use ax for port i/o

	    mov dx, 03DAh               ; CRT controller input status 1 register
o_vbi_1:
		in al, dx                   ; watch vertical blanking bit
		test al,08h                 ; wait for it to clear to make sure
		jnz o_vbi_1                 ; we're not in a blanking interval
o_vbi_2:
		in al, dx                   ; now wait for the start of the
		test al,08h                 ; next blanking interval
		jz o_vbi_2

        mov     ah, BYTE PTR start  ; get first register to process into ah
        mov     dx, 03c8h           ; DAC palette index register
o_pal_load_loop:
        mov     al, ah              ; get next palette number to write
        out     dx, al              ; write the register number to the dac
        inc     dx                  ; address dac data register
        mov     al, BYTE PTR [es:di] ; get first byte of triplet
        out     dx, al               ; write it to the dac data register
        inc     di                   ; point to second byte
        mov     al, BYTE PTR [es:di] ; get second byte of triplet
        out     dx, al               ; write it to the dac data register
        inc     di                   ; point to third byte
        mov     al, BYTE PTR [es:di] ; get third byte of triplet
        out     dx, al               ; write it to the dac data register
        inc     di                   ; point to first byte of next triplet
        dec     dx                   ; address the dac index register
        inc     ah                   ; point to next palette register
        loop    o_pal_load_loop      ; process next triplet

        pop     ax                ; restore ax
        pop     di                ; restore the offset into pal
        pop     cx                ; restore the fade loop counter
        loop    o_fade_loop       ; do the next pass through the fade loop

        pop     bx
        pop     cx
        pop     dx
        pop     si
        pop     di
        pop     ds
        pop     es
        pop     bp
        ret
_fadepalout     ENDP

; extern "C" boolean detectvga()
;
; This function returns true if a VGA card is installed in the system, the
; vga card is the active adapter, and the system has an analog color monitor

_detectvga      PROC
        push    bx
        mov     ax, 1a00h         ; bios function 1ah, subfunction 00h
        int     10h
        cmp     al, 1ah           ; if al = 1ah VGA card installed
        jne     no_vga            ; else exit through no_vga
        cmp     bl, 08h           ; bl = 08h means VGA active and color mon.
        jne     no_vga            ; else exit through no_vga
        mov     ax, 1             ; set return value to boolean true
        jmp     vga_done          ; exit
no_vga:
        xor     ax, ax            ; set return value to boolean false
vga_done:
        pop     bx
        ret
_detectvga      ENDP

		END
