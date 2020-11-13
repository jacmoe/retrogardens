; -------+---------+---------+---------+---------+---------+---------+---------+
;  1991-1992 Betz Associates. Released to the Public Domain. This source code
;  is offered without warranty or guarantee of any kind, and the author(s)
;  expressly disclaim any responsibility for consequences of it's use in
;  products for commercial, shareware, or public domain release of any kind.
;
;  File Name: PALFADE.ASM
;  Creation:  August 2, 1992
;  Author:    Mark Betz (MB)
;
;  Machine:   IBM PC and Compatibles
;
;  Includes:  nothing
;
; -------+---------+---------+---------+---------+---------+---------+---------+
;  Change History
;  ------ -------
;
;       Date            Rev.	Author		Purpose
;       ----            ----    ------      -------
;       8-2-1992        1.0     MB          Initial release
;       9-20-1992       1.1     MB          Update release v 1.1
;
;  Description
;  -----------
;       Two assembler functions for fading the VGA palette in/out
;
; -------+---------+---------+---------+---------+---------+---------+---------+


		.MODEL	huge
		.CODE

		PUBLIC  _fadepalin, _fadepalout

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

        pal     db 768 dup (0)    ; working palette area

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

        END