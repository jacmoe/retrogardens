
; RAY.ASM
;
; Assembly language functions for drawing a raycast maze.
;
; Written by Christopher Lampton for
; GARDENS OF IMAGINATION (Waite Group Press, 1994)

		.MODEL  huge
    .386
		.CODE
    PUBLIC  _drawwall,_drawfloorrow,_drawceilrow

SCREEN_WIDTH	EQU	320
SCREEN_HEIGHT	EQU 200

xincrement dd 0
yincrement dd 0
map        dd 0
texture    dd 0
textureptr dd 0
rownum     dw 0
colnum     dw 0
width      dw 0
botptr     dd 0
topptr     dd 0
lightindex dd 0


COLUMNLOOPOLD MACRO        ;(16 bytes) instr for each pixel in column
; GS:[SI]=current screen location
; BP=256
; EDX=current bitmap pixel (16:16)
; ECX=stepping increment (16:16)
; FS:[DI]=bitmap pixel
; BL=pixel color

    shld edi,edx,16       ;mov integer portion of Bitmap index into bx
    mov  bl,fs:[di]       ;bl = pixel color from Bitmap
    mov  gs:[si],bl       ;mov pixel color into ScreenBuffer
    add  edx,ecx          ;increment Bitmap index
    sub  si,bp            ;increment ScreenBuffer index
ENDM

COLUMNLOOP MACRO        ;(20 bytes) instr for each pixel in column
; AL=pixel color
; EBX=light level
; EAX
    shld edi,edx,16       ;mov integer portion of Bitmap index into bx
    mov  al,fs:[di]       ;al = pixel color from Bitmap
    mov  al,es:[ebx + eax]  ;light source the pixel
    mov  gs:[si],al       ;mov pixel color into ScreenBuffer
    sub  edx,ecx          ;increment Bitmap index
    sub  si,bp            ;increment ScreenBuffer index
ENDM

FLOORLOOP MACRO REP
    LOCAL SKIPPIXEL
    lgs  bx,[botptr]
    mov  al,gs:[bx]
    mov  bx,[rownum]
    cmp  al,bl                  ;check if floor pix past a drawn col
    ja   SKIPPIXEL

                                     ;GRID CALCULATIONS
    shld edi,edx,10           ; edi=int(GridZ)/64
    shld ebx,ecx,10           ; edi=int (GridY)/64 * 16
    and  ebx,15
    shl  ebx,4
                                     ;negative values are inverted.
    and  edi,15
    add  bx,di                 ;bx now = GridIndex into the Grid
                                     ; =  ((int(GridX) mod 16) * 16)
                                     ;   + (int(GridZ) mod 16)

    mov  al,es:[ebp + ebx]     ;set al to Floor index into TextureList
    lgs  bx,[texture]          ;set ebx to start of TextureList
    mov  edi,gs:[ebx + (eax * 4)] ;set Texture to ptr to bitmap to get
    mov  [textureptr],edi      ;floor color for that pixel from

                                     ;BITMAP CALCULATIONS
    shld edi,ecx,16            ;mov integer part of GlobalZ32 into di
    shld ebx,edx,16            ;repeat for BitMapX into bx
    and  edi,63
    and  ebx,63
    imul di,320
    add  di,bx
    lgs  bx,[textureptr]          ;load bitmap into gs:[bx]
    mov  al,gs:[bx + di]       ;move pixel color into al

                                     ;LIGHT SOURCING
    lgs  bx,[LightIndex]       ;mov pointer to start of correct palette
    mov  al,gs:[ebx + eax]        ;move new color into al

;    mov  fs:[si + rep],al         ;mov color into ScreenBuffer
    db   64h,88h,44h,rep

SKIPPIXEL:
    add  dword ptr [botptr],1
    add  ecx,[yincrement]
    add  edx,[xincrement]
ENDM

CEILINGLOOP MACRO REP
    LOCAL SKIPPIXEL
    lgs  bx,[topptr]
    mov  al,gs:[bx]
    mov  bx,[rownum]
    cmp  al,bl                  ;check if floor pix past a drawn col
    jb   SKIPPIXEL

                                     ;GRID CALCULATIONS
    shld edi,edx,16           ; edi=int(GridZ)
    shld ebx,ecx,16           ; edi=int (GridY) mod 16 * 16
    and  ebx,15
    shl  ebx,4
                                     ;negative values are inverted.
    and  edi,15
    add  bx,di                 ;bx now = GridIndex into the Grid
                                     ; =  ((int(GridX) mod 16) * 16)
                                     ;   + (int(GridZ) mod 16)

    mov  al,es:[ebp + ebx]     ;set al to Floor index into TextureList
    lgs  bx,[texture]          ;set ebx to start of TextureList
    mov  edi,gs:[ebx + (eax * 4)] ;set Texture to ptr to bitmap to get
    mov  [textureptr],edi      ;floor color for that pixel from

                                     ;BITMAP CALCULATIONS
    shld edi,ecx,16            ;mov integer part of GlobalZ32 into di
    shld ebx,edx,16            ;repeat for BitMapX into bx
    and  edi,63
    and  ebx,63
    imul di,320
    add  di,bx
    lgs  bx,[textureptr]          ;load bitmap into gs:[bx]
    mov  al,gs:[bx + di]       ;move pixel color into al

                                     ;LIGHT SOURCING
    lgs  bx,[LightIndex]       ;mov pointer to start of correct palette
    mov  al,gs:[ebx + eax]        ;move new color into al

;    mov  fs:[si + rep],al         ;mov color into ScreenBuffer
    db   64h,88h,44h,rep

SKIPPIXEL:
    add  dword ptr [topptr],1
    add  ecx,[yincrement]
    add  edx,[xincrement]
ENDM

_drawwall	PROC
		ARG		screenptr:DWORD,bitmapptr:DWORD,height:WORD,increment:DWORD,litelevel:DWORD
    push	bp              ; Save BP
		mov		bp,sp           ; Set up stack pointer
    mov   bx,height
    mov   ax,200
    sub   ax,bx
    mov   ecx,increment
    lgs   si,screenptr
    lfs   di,bitmapptr
    mov   ebx,0
    les   bx,litelevel
    mov   dx,di
    shl   edx,16
    imul  ax,21                   ; Multiply by 21 bytes
    mov   di,offset walloop       ;add the starting point of the
    add   di,ax                   ;unrolled loop.
    mov   bp,320
    xor   eax,eax
    jmp   di                      ;jump into unrolled loop.
walloop:
    REPT 200
      COLUMNLOOP
    ENDM
		pop		bp              ; Restore BP
		ret
_drawwall			ENDP

_drawfloorrow  PROC
		ARG		row:WORD,screenptr:DWORD,texturelist:DWORD,floormap:DWORD,litelevel:DWORD,bots:DWORD,xinc:DWORD,yinc:DWORD,x:DWORD,y:DWORD,w:WORD
    push	bp              ; Save BP
		mov		bp,sp            ;Set up stack pointer
    mov   bx,w
    mov   [width],bx
    mov   ebx,litelevel
    mov   [lightindex],ebx
    mov   bx,row
    mov   [rownum],bx
    mov   [colnum],0
    mov   ecx,y
    mov   edx,x
    lfs   si,screenptr
    mov   ebx,xinc
    mov   [xincrement],ebx
    mov   ebx,yinc
    mov   [yincrement],ebx
    mov   ebx,texturelist
    mov   [texture],ebx
    mov   ebx,bots
    mov   [botptr],ebx
    les   bp,floormap
    mov   ax,[width]
    and   ax,7
    sub   si,ax
    imul  ax,floor1-floor         ; Multiply by 50 bytes
    mov   di,offset floor         ;add the starting point of the
    add   di,ax                   ;unrolled loop.
    xor   eax,eax           ;top 3 bytes must remain clear in loop
    jmp   di

floor:
    FLOORLOOP 0
floor1:
    FLOORLOOP 1
    FLOORLOOP 2
    FLOORLOOP 3
    FLOORLOOP 4
    FLOORLOOP 5
    FLOORLOOP 6
    FLOORLOOP 7
    add  si,8                   ;move one pixel right in ScreenBuffer
    add  [colnum],8
    mov  bx,[colnum]
    cmp  bx,[width]
    jb   floor

    pop   bp
    ret
_drawfloorrow  ENDP

_drawceilrow  PROC
		ARG		row:WORD,screenptr:DWORD,texturelist:DWORD,ceilmap:DWORD,litelevel:DWORD,bots:DWORD,xinc:DWORD,yinc:DWORD,x:DWORD,y:DWORD,w:WORD
    push	bp              ; Save BP
		mov		bp,sp            ;Set up stack pointer
    mov   bx,w
    mov   [width],bx
    mov   ebx,litelevel
    mov   [lightindex],ebx
    mov   bx,row
    mov   [rownum],bx
    mov   [colnum],0
    mov   ecx,y
    mov   edx,x
    lfs   si,screenptr
    mov   ebx,xinc
    mov   [xincrement],ebx
    mov   ebx,yinc
    mov   [yincrement],ebx
    mov   ebx,texturelist
    mov   [texture],ebx
    mov   ebx,bots
    mov   [topptr],ebx
    les   bp,ceilmap
    mov   ax,[width]
    and   ax,7
    sub   si,ax
    imul  ax,ceiling2-ceiling     ; Multiply by 50 bytes
    mov   di,offset ceiling       ;add the starting point of the
    add   di,ax                   ;unrolled loop.
    xor   eax,eax           ;top 3 bytes must remain clear in loop
    jmp   di

ceiling:
    CEILINGLOOP 0
ceiling2:
    CEILINGLOOP 1
    CEILINGLOOP 2
    CEILINGLOOP 3
    CEILINGLOOP 4
    CEILINGLOOP 5
    CEILINGLOOP 6
    CEILINGLOOP 7
    add  si,8                   ;move one pixel right in ScreenBuffer
    add  [colnum],8
    mov  bx,[colnum]
    cmp  bx,[width]
    jb   ceiling

    pop   bp
    ret
_drawceilrow  ENDP

		END
