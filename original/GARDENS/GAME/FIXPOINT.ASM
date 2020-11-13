
; FIXPOINT.ASM
;
; Copyright (c) 1993
; Kevin Gliner
;
; Adapted by Christopher Lampton for
; GARDENS OF IMAGINATION (Waite Group Press, 1994)

    .MODEL huge
    .DATA
    .CODE
    .386
    PUBLIC _fixmul,_fixmuldiv,_fixdiv,_fix

_fixmul       PROC
              ARG arg1:DWORD, arg2:DWORD

		          push	bp
		          mov		bp,sp
							mov		eax,arg1
							imul	arg2
;	  					shr		eax,16          ;to put result in eax instead, do
                                    ;a shrd edx,eax,16
              shrd  eax,edx,16
 		          pop		bp
							ret

_fixmul       ENDP

_fixmuldiv    PROC
              ARG arg1:DWORD, arg2:DWORD, arg3:DWORD

		          push	bp
		          mov		bp,sp
							mov		eax, arg1
 							imul	arg2
							idiv	arg3               ;eax now holds result
              shld  edx,eax,16
		          pop		bp
							ret

_fixmuldiv    ENDP

_fixdiv       PROC
              ARG numer:DWORD, denom:DWORD

              push	bp
		          mov		bp,sp
							mov		eax, numer
              mov   edx,eax
              sar   edx,16
              shl   eax,16
							idiv	denom            ;eax now holds result
              shld  edx,eax,16
		          pop		bp
							ret

_fixdiv       ENDP

_fix          PROC
              ARG arg1:WORD

          		push	bp
          		mov		bp,sp
							mov		ax, arg1
              sal   eax,16
							mov		edx, eax
							shr		edx, 16
		          pop		bp
  						ret
_fix          ENDP

              END
