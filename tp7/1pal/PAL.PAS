(*************************************************************************)
(* Draws a palette  *)
(*************************************************************************)
{$R-}
{$I+}
{$N-}
{$E-}

program pal;

uses
    Crt, Dos, VGA_320;

const
     Bell = ^G;

Var
   Save_Palette : Color_Table;
   I : longint;
   X1, Y1 : longint;
   X2, Y2 : longint;
   Width : integer;
   Height : integer;
   C : byte;
   R, G, B : byte;
   P_Color : Color_Table;


(* Wait for the user to strike a key *)
procedure Continue;
begin
     while KeyPressed do Readkey;
     write(Bell);
     while not KeyPressed do ;
     while KeyPressed do ReadKey;
end;


begin
     InitGraph;
     Get_Palette(Save_Palette);

     (* Build a color palette with 216 entries *)
     FillChar(P_Color, SizeOf(P_Color), Zero);

     for R := 0 to 5 do
     begin
         for G := 0 to 5 do
         begin
             for B := 0 to 5 do
             begin
                 with P_color[((R * 6) + G) * 6 + B] do
                 begin
                      Red := R * 51;
                      Green := G * 51;
                      Blue := B * 51;
                 end;
             end;
         end;
     end;

     Put_Palette(P_Color);
     Width := Succ(GetMaxX) div 16;
     Height := Succ(GetMaxY) div 16;

     for C := 0 to 255 do
     begin
          X1 := (C mod 16) * Width;
          X2 := X1 + Width - 1;
          Y1 := (C div 16) * Height;
          Y2 := Y1 + Height - 1;
          Filled_Rectangle(X1, Y1, X2, Y2, C);
     end;
     Continue;

     Put_Palette(Save_Palette);
     CloseGraph;

end.

