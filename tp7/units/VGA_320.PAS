(******************************************************************************************************)
(*												      *)
(*	Dr. William T. Verts (C) April 26, 1996							      *)
(*												      *)
(*	Simple unit to support VGA 320x200 256 color mode					      *)
(*												      *)
(******************************************************************************************************)
{$R-}
{$I-}
{$N-}
{$E-}

Unit VGA_320 ;

Interface

Uses	DOS ;

Const	Zero		= 0						;
	GetMaxX		= 319						;
	GetMaxY		= 199						;
	GetMidX		= GetMaxX Div 2					;
	GetMidY		= GetMaxY Div 2					;

Type	VGA_Linear	= Array [0..63999] Of Byte			;
	VGA_Array	= Array [Zero..GetMaxY, Zero..GetMaxX] Of Byte	;

	Color_Type	= Record
				Red		: Byte			;
				Green		: Byte			;
				Blue		: Byte			;
			  End						;

	Color_Table	= Array [Byte] Of Color_Type			;

Var	Screen_Linear	: VGA_Linear Absolute $A000:$0000		;
	Screen_Array	: VGA_Array  Absolute $A000:$0000		;

Procedure InitGraph		;
Procedure CloseGraph		;
Procedure Get_Palette		(Var P:Color_Table) ;
Procedure Put_Palette		(    P:Color_Table) ;
Procedure ClearDevice		;
Procedure Fill_Screen		(Color:Byte) ;

Procedure PutPixel		(X,Y:LongInt ; Color:Byte) ;
Function  GetPixel		(X,Y:LongInt) : Byte ;
Procedure Horizontal_Line	(X1,X2,Y:LongInt ; Color:Byte) ;
Procedure Vertical_Line		(X,Y1,Y2:LongInt ; Color:Byte) ;
Procedure Line			(X1,Y1,X2,Y2:LongInt ; Color:Byte) ;
Procedure Rectangle		(X1,Y1,X2,Y2:LongInt ; Color:Byte) ;
Procedure Filled_Rectangle	(X1,Y1,X2,Y2:LongInt ; Color:Byte) ;
Procedure Circle		(Xc,Yc,R:LongInt ; Color:Byte) ;
Procedure Filled_Circle		(Xc,Yc,R:LongInt ; Color:Byte) ;

(******************************************************************************************************)
(******************************************************************************************************)
(******************************************************************************************************)

Implementation

Procedure Lswap (Var N1,N2:LongInt) ;
    Var	Temp	: LongInt ;
Begin	(***)
	Temp	:= N1 ;
	N1	:= N2 ;
	N2	:= Temp ;
End ;

(******************************************************************************************************)
(*	Tell the video hardware to select 320x200 256-color graphics mode, then clear screen	      *)
(*	In assembly language, this can be done by MOV AX, 13H followed by INT 10H		      *)

Procedure InitGraph ;

    Var	R	: Registers ;

Begin	(***)
	FillChar (R, SizeOf(R), Zero) ;
	R.AX	:= $0013 ;
	Intr	($10, R) ;
	ClearDevice ;
End ;

(*----------------------------------------------------------------------------------------------------*)
(*	Tell the video hardware to select 80x25 Text mode (16 color)				      *)
(*	In assembly language, this can be done by MOV AX, 03H followed by INT 10H		      *)

Procedure CloseGraph ;

    Var	R	: Registers ;

Begin	(***)
	ClearDevice ;
	FillChar (R, SizeOf(R), Zero) ;
	R.AX	:= $0003 ;
	Intr	($10, R) ;
End ;

(*----------------------------------------------------------------------------------------------------*)
(*	Fetch the current palette from the VGA card.  Since VGA palettes use only 6 bits per color,   *)
(*	it is necessary to scale each of the entries by shifting them to the left by 2 bits	      *)
(*	(multiplying them by 4) to turn them into true RGB byte values				      *)

Procedure Get_Palette (Var P:Color_Table) ;

    Var	R	: Registers ;
	I	: Byte ;

Begin	(***)
	(*------------------------------------------------------------------*)

	FillChar (R, SizeOf(R), Zero) ;		  (* Initialize Record      *)

	With R Do
		Begin	(***)
			AX	:= $1017	; (* Command to get Palette *)
			BX	:= 0		; (* Starting Palette Entry *)
			CX	:= 256		; (* Number of Entries      *)
			ES	:= Seg(P)	; (* Address of Palette     *)
			DX	:= Ofs(P)	; (* Address of Palette     *)
		End ;

	Intr	($10, R) ;			  (* Do the command         *)

	(*------------------------------------------------------------------*)
	(* Turn 6 bit VGA palette entries into 8 bit RGB bytes              *)

	For I := 0 To 255 Do
		With P[I] Do
			Begin	(***)
				Red	:= Red   SHL 2 ;
				Green	:= Green SHL 2 ;
				Blue	:= Blue  SHL 2 ;
			End ;
End ;

(*----------------------------------------------------------------------------------------------------*)
(*	Make P the current palette on the VGA card.  Since VGA palettes use only 6 bits per color,    *)
(*	it is necessary to scale each of the RGB byte entries by shifting them to the right by 2 bits *)
(*	(dividing them by 4) to turn them into true VGA 6 bit values.  This loses the lower two bits  *)
(*	of precision (any two colors that differ by only the lower two bits of each byte will be      *)
(*	mapped onto the same VGA color).							      *)

Procedure Put_Palette (P:Color_Table) ;

    Var	R	: Registers ;
	I	: Byte ;

Begin	(***)
	(*------------------------------------------------------------------*)
	(* Turn 8 bit RGB values into 6 bit VGA palette entries             *)

	For I := 0 To 255 Do
		With P[I] Do
			Begin	(***)
				Red	:= Red   SHR 2 ;
				Green	:= Green SHR 2 ;
				Blue	:= Blue  SHR 2 ;
			End ;

	(*------------------------------------------------------------------*)

	FillChar (R, SizeOf(R), Zero) ;		  (* Initialize Record      *)

	With R Do
		Begin	(***)
			AX	:= $1012	; (* Command to put Palette *)
			BX	:= 0		; (* Starting Palette Entry *)
			CX	:= 256		; (* Number of Entries      *)
			ES	:= Seg(P)	; (* Address of Palette     *)
			DX	:= Ofs(P)	; (* Address of Palette     *)
		End ;

	Intr	($10, R) ;			  (* Do the command         *)
End ;

(*----------------------------------------------------------------------------------------------------*)
(*	Set all pixels in the 64000 byte array to zero (generally entry zero is black).		      *)

Procedure ClearDevice ;
Begin	(***)
	Fill_Screen (Zero) ;
End ;

(*----------------------------------------------------------------------------------------------------*)
(*	Use the linear array definition to flood all 64000 bytes with the same color.  Although a FOR *)
(*	loop can be used (For I := 0 To 63999 Do Screen_Linear[I] := Color ;) the FillChar procedure  *)
(*	is much faster (visually almost instantaneous).						      *)

Procedure Fill_Screen (Color:Byte) ;
Begin	(***)
	FillChar (Screen_Linear, SizeOf(Screen_Linear), Color) ;
End ;

(*----------------------------------------------------------------------------------------------------*)

Procedure PutPixel (X,Y:LongInt ; Color:Byte) ;
Begin	(***)
	If (X >= Zero   ) And
	   (X <= GetMaxX) And
	   (Y >= Zero   ) And
	   (Y <= GetMaxY) Then Screen_Array [Y, X] := Color ;
End ;

(*----------------------------------------------------------------------------------------------------*)

Function GetPixel (X,Y:LongInt) : Byte ;
Begin	(***)
	If (X < Zero) Or
	   (X > GetMaxX) Or
	   (Y < Zero) Or
	   (Y > GetMaxY) Then GetPixel := Zero
			 Else GetPixel := Screen_Array[Y, X] ;
End ;

(*----------------------------------------------------------------------------------------------------*)
(*	This routine first clips the line coordinates to the screen.  The last line could have been   *)
(*	For X := X1 To X2 Do Screen_Array[Y, X] := Color ; but FillChar was used to increase speed.   *)

Procedure Horizontal_Line (X1,X2,Y:LongInt ; Color:Byte) ;
Begin	(***)
	If Y < Zero Then Exit ;
	If Y > GetMaxY Then Exit ;
	If X1 > X2 Then Lswap (X1, X2) ;
	If X1 < Zero Then X1 := Zero ;
	If X2 > GetMaxX Then X2 := GetMaxX ;
	If X1 <= X2 Then FillChar (Screen_Array[Y, X1], X2 - X1 + 1, Color) ;
End ;

(*----------------------------------------------------------------------------------------------------*)
(*	Calling a vertical line routine is faster than calling the general purpose line routine for   *)
(*	these cases because it can take advantage of the layout of the video hardware.		      *)

Procedure Vertical_Line (X,Y1,Y2:LongInt ; Color:Byte) ;

    Var	Y	: Integer ;

Begin	(***)
	If X < Zero Then Exit ;
	If X > GetMaxX Then Exit ;
	If Y1 > Y2 Then Lswap (Y1, Y2) ;
	If Y1 < Zero Then Y1 := Zero ;
	If Y2 > GetMaxY Then Y2 := GetMaxY ;
	For Y := Y1 To Y2 Do Screen_Array[Y, X] := Color ;
End ;

(*----------------------------------------------------------------------------------------------------*)
(*	A variation on Bresenham's classic line algorithm.					      *)

Procedure Line (X1,Y1,X2,Y2:LongInt ; Color:Byte) ;

    Var	DeltaX	: LongInt ;
	DeltaY	: LongInt ;
	X, Y, S	: LongInt ;
	XX, YY	: LongInt ;
	SwapX	: Boolean ;
	SwapY	: Boolean ;
	SwapXY	: Boolean ;

Begin	(***)
	If Y1 = Y2 Then Horizontal_Line (X1, X2, Y1, Color) Else
	If X1 = X2 Then Vertical_Line   (X1, Y1, Y2, Color) Else
		Begin	(***)
			DeltaX	:= X2 - X1 ;
			DeltaY	:= Y2 - Y1 ;
			SwapX	:= (DeltaX < Zero) ;
			DeltaX	:= Abs(DeltaX) ;
			SwapY	:= (DeltaY < Zero) ;
			DeltaY	:= Abs(DeltaY) ;
			SwapXY	:= (DeltaY > DeltaX) ;
			If SwapXY Then Lswap(DeltaX, DeltaY) ;

			PutPixel (X1, Y1, Color) ;
			Y	:= Zero ;
			S	:= -(DeltaY Div 2) ;

			For X := 1 To DeltaX Do
				Begin	(***)
					Inc	(S, DeltaY) ;
					If S > Zero Then
						Begin	(***)
							Dec	(S, DeltaX) ;
							Inc	(Y) ;
						End ;

					XX	:= X ;
					YY	:= Y ;
					If SwapXY Then Lswap(XX, YY) ;
					If SwapX  Then XX := -XX ;
					If SwapY  Then YY := -YY ;
					PutPixel (X1+XX, Y1+YY, Color) ;
				End ;
		End ;
End ;

(*----------------------------------------------------------------------------------------------------*)
(*	This routine lets the line routines handle all of the clipping for simplicity's sake.	      *)

Procedure Rectangle (X1,Y1,X2,Y2:LongInt ; Color:Byte) ;
Begin	(***)
	Horizontal_Line (X1, X2, Y1, Color) ;
	Horizontal_Line (X1, X2, Y2, Color) ;
	Vertical_Line   (X1, Y1, Y2, Color) ;
	Vertical_Line   (X2, Y1, Y2, Color) ;
End ;

(*----------------------------------------------------------------------------------------------------*)
(*	This routine could have been made shorter by calling Horizontal_Line in the For Y loop, and   *)
(*	letting it perform all of the clipping, but by explicitly clipping and using FillChar to fill *)
(*	each raster line, we trade a little bit of complexity for greatly increased speed.	      *)

Procedure Filled_Rectangle (X1,Y1,X2,Y2:LongInt ; Color:Byte) ;

    Var	Y	: Integer ;
	Bytes	: Integer ;

Begin	(***)
	If X1 > X2 Then Lswap (X1, X2) ;
	If Y1 > Y2 Then Lswap (Y1, Y2) ;
	If X1 < Zero Then X1 := Zero ;
	If Y1 < Zero Then Y1 := Zero ;
	If X2 > GetMaxX Then X2 := GetMaxX ;
	If Y2 > GetMaxY Then Y2 := GetMaxY ;
	If X1 > X2 Then Exit ;
	If Y1 > Y2 Then Exit ;
	Bytes	:= (X2 - X1 + 1) ;
	For Y := Y1 To Y2 Do FillChar (Screen_Array[Y, X1], Bytes, Color) ;
End ;

(*----------------------------------------------------------------------------------------------------*)
(*	A variation on Bresenham's circle routine.  Note that the routine computes points only for a  *)
(*	single octant (octant #2, above the Y=0 axis and to the right of the X=0 axis), then exploits *)
(*	8-way symmetry to generate the corresponding points in the remaining 7 octants.		      *)

Procedure Circle (Xc,Yc,R:LongInt ; Color:Byte) ;

    Var	X, Y, S	: LongInt ;

Begin	(***)
	X	:= R ;
	Y	:= Zero ;
	S	:= -R ;

	While (X >= Y) Do
		Begin	(***)
			PutPixel (Xc - X, Yc - Y, Color) ;
			PutPixel (Xc + X, Yc - Y, Color) ;
			PutPixel (Xc - X, Yc + Y, Color) ;
			PutPixel (Xc + X, Yc + Y, Color) ;
			PutPixel (Xc - Y, Yc - X, Color) ;
			PutPixel (Xc + Y, Yc - X, Color) ;
			PutPixel (Xc - Y, Yc + X, Color) ;
			PutPixel (Xc + Y, Yc + X, Color) ;

			Inc	(S, Y + Y + 1) ;
			Inc	(Y) ;
			If S > Zero Then
				Begin	(***)
					Dec	(S, X + X - 2) ;
					Dec	(X) ;
				End ;
		End ;
End ;

(*----------------------------------------------------------------------------------------------------*)
(*	This is the same algorithm as above, except that instead of painting individual pixels, the   *)
(*	left-right pairs of pixels are used as endpoints for horizontal lines.			      *)

Procedure Filled_Circle (Xc,Yc,R:LongInt ; Color:Byte) ;

    Var	X, Y, S	: LongInt ;

Begin	(***)
	X	:= R ;
	Y	:= Zero ;
	S	:= -R ;

	While (X >= Y) Do
		Begin	(***)
			Horizontal_Line (Xc - X, Xc + X, Yc - Y, Color) ;
			Horizontal_Line (Xc - X, Xc + X, Yc + Y, Color) ;
			Horizontal_Line (Xc - Y, Xc + Y, Yc - X, Color) ;
			Horizontal_Line (Xc - Y, Xc + Y, Yc + X, Color) ;

			Inc	(S, Y + Y + 1) ;
			Inc	(Y) ;
			If S > Zero Then
				Begin	(***)
					Dec	(S, X + X - 2) ;
					Dec	(X) ;
				End ;
		End ;
End ;

(******************************************************************************************************)

End.
