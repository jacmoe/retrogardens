
		.MODEL	large
		.DATA
		yincrement	dd	0
		.DATA?
		wwidth	dw	?
		lightindex	dd	?
		rownum	dw	?
		colnum	dw	?
		xincrement	dd	?
		texture		dd	?
		botptr		dd	?
		textureptr	dd	?

		.CODE
		.386
		PUBLIC	_fixmul,_fixdiv,_drawwall
		PUBLIC	_drawfloorrow

_fixmul		PROC
	ARG arg1:DWORD, arg2:DWORD
	push	bp			; set up BP register
	mov		bp, sp
	mov		eax, arg1	; get first argument into EAX
	imul	arg2		; multiply it by second argument
	shrd	eax,edx,16	; shift high and low bytes into DX:AX
	pop		bp
	ret
_fixmul		ENDP

_fixdiv		PROC
	ARG numer:DWORD, denom:DWORD
	push	bp			; set up BP register
	mov		bp,sp
	mov		eax,numer	; put dividend into EAX
	mov		edx,eax		; copy it into EDX
	sar		edx,16		; shift high 16 bits of EDX back into EAX
	shl		eax,16		; shift low 16 bits of EAX into high 16 bits
	idiv	denom		; divide by divisor
	shld	edx,eax,16	; get result
	pop		bp
	ret
_fixdiv		ENDP

COLUMNLOOP	MACRO
	shld	edi,edx,16			; move integral portion of bitmap
								; pointer into DI
	mov		al,es:[ebx + eax]	; get lightsourced color
	mov		gs:[si],al			; copy pixel color to screen column
	sub		edx,ecx				; add increment to bitmap pointer
	sub		si,bp				; point to next pixel in wall column
ENDM


_drawwall	PROC
	ARG	screenptr:DWORD, bitmapptr:DWORD, height:WORD, increment:DWORD, litelevel:DWORD
	push	bp					; save BP
	mov		bp,sp				; set up stack pointer
	mov		bx,height			; get height in BX
	mov		ax,200				; calculate number of pixels to skip
	sub		ax,bx				; leave result in AX
	mov		ecx,increment		; get increment in ECX
	lgs		si,screenptr		; get screen index in GS:SI
	lfs		di,bitmapptr		; get pointer to bitmap in FS:DI
	mov		ebx,0				; clear out EBX
	les		bx,litelevel		; get lightsource table addr in BX
	mov		dx,di				; copy increment in DX
	shl		edx,16				; reverse the bytes
	imul	ax,21				; calculate jump address
	mov		di,offset walloop	; add start of loop....
	add		di,ax				; ...to offset in loop
	mov		bp,320				; store constant in BP
	xor		eax,eax				; clear out EAX
	jmp		di					; jump unto unrolled loop
walloop:
	REPT	200					; repeat macro 200 times
		COLUMNLOOP
	ENDM
	pop		bp					; restore BP
	ret
_drawwall	ENDP

FLOORLOOP	MACRO	REP
	LOCAL	SKIPPIXEL
	lgs		bx,[botptr]			; get pointer to BOTS array
	mov		al,gs:[bx]			; get current bottom position
	mov		bx,[rownum]			; get current row number
	cmp		al,bl				; compare the two
	ja		SKIPPIXEL			; jump if floor pixel behind the wall
	shld	edi,edx,10			; (int)x / 64
	shld	ebx,ecx,10			; (int)y / 64
	and		ebx,15				; clear out junk in EBX
	shl		ebx,4				; multiply y * 16
	and		edi,15				; clear out junk in EDI
	add		bx,di				; BX = (int)y / 64 * 16 + (int)x / 64
	mov		al,es:[ebp + ebx]	; get tile number in AL
	lgs		bx,[texture]		; point GS:BX at texture list
	mov		edi,gs:[ebx + (eax * 4)]	; get pointer to texture map
	mov		[textureptr],edi	; save texturemap pointer
	shld	edi,ecx,16			; calculate (int)y % 64 * 320 + x % 64
	shld	ebx,edx,16
	and		edi,63
	and		ebx,63
	imul	di,320
	add		di,bx
	lgs		bx,[textureptr]		; get pointer to texture
	mov		al,gs:[bx + di]		; get pixel color
	lgs		bx,[lightIndex]		; point to lightsource table
	mov		al,gs:[ebx + eax]	; get lightsourced color
	mov		fs:[si + rep],al	; put it on screen
SKIPPIXEL:
	add		dword ptr [botptr],1	; advance bottom pointer
	add		ecx,[yincrement]	; add increments to get
	add		edx,[xincrement]	; next pixel coordinate
ENDM

_drawfloorrow	PROC
	ARG row:WORD,screenptr:DWORD,texturelist:DWORD,floormap:DWORD,litelevel:DWORD,bots:DWORD,xinc:DWORD,yinc:DWORD,x:DWORD,y:DWORD,w:WORD
	push	bp				; save BP
	mov		bp,sp			; set up stack pointer
	mov		bx,w			; move parameters into memory variables
	mov		[wwidth],bx
	mov		ebx,litelevel
	mov		[lightindex],ebx
	mov		bx,row
	mov		[rownum],bx
	mov		[colnum],0
	mov		ecx,y
	mov		edx,x
	lfs		si,screenptr
	mov		ebx,xinc
	mov		[xincrement],ebx
	mov		ebx,texturelist
	mov		[texture],ebx
	mov		ebx,bots
	mov		[botptr],ebx
	les		bp,floormap
	xor		eax,eax			; clear the EAX register

floor:
	FLOORLOOP 0			; unroll FLOORLOOP 8 times
	FLOORLOOP 1
	FLOORLOOP 2
	FLOORLOOP 3
	FLOORLOOP 4
	FLOORLOOP 5
	FLOORLOOP 6
	FLOORLOOP 7
	add		si,8			; advance screen pointers
	add		[colnum],8		; increase column count
	mov		bx,[colnum]		; have we covered entire viewport?
	cmp		bx,[wwidth]
	jb		floor			; if not, do it again

	pop		bp				; else return to caller
	ret
_drawfloorrow	ENDP

END
