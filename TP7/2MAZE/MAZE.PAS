(*************************************************************************)
(* Draws a maze  *)
(*************************************************************************)
{$R-}
{$I+}
{$N-}
{$E-}

program maze;

uses
    Crt, Dos, VGA_320;

type
    xy = record
       x : integer;
       y : integer;
    end;

const
     Bell = ^G;
     Map : array [0..15, 0..15] of integer =
     (
        (1,1,1,1,  1,1,1,1,  1,1,1,1,  1,1,1,1),
        (1,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1),
        (1,0,1,0,  1,1,1,0,  0,0,0,0,  1,1,0,1),
        (1,0,1,0,  1,0,1,0,  1,1,1,0,  1,1,0,1),

        (1,0,1,0,  1,0,1,0,  0,0,1,0,  0,1,0,1),
        (1,0,1,1,  1,0,1,0,  1,0,1,1,  1,1,0,1),
        (1,0,0,0,  0,0,0,0,  1,0,0,0,  0,0,0,1),
        (1,0,1,1,  1,0,1,0,  1,1,1,0,  1,1,0,1),

        (1,0,1,0,  0,0,1,1,  0,0,1,0,  1,1,0,1),
        (1,0,1,0,  1,0,1,1,  1,1,1,0,  1,1,0,1),
        (1,0,1,0,  1,0,0,0,  0,0,0,0,  0,0,0,1),
        (1,0,1,1,  1,0,1,0,  1,0,1,1,  1,1,0,1),

        (1,0,0,0,  1,0,1,0,  1,0,0,0,  0,1,0,1),
        (1,0,1,1,  1,0,1,1,  1,1,1,1,  1,1,0,1),
        (1,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1),
        (1,1,1,1,  1,1,1,1,  1,1,1,1,  1,1,1,1)
    );

    increment : array[0..3] of xy = (
        (x : -1; y : 0),
        (x : 0; y : 1),
        (x : 1; y : 0),
        (x : 0; y : -1));
    left : array[0..3] of xy = (
        (x : 0; y : -1),
        (x : -1; y : 0),
        (x : 0; y : 1),
        (x : 1; y : 0));
    right : array[0..3] of xy = (
        (x : 0; y : 1),
        (x : 1; y : 0),
        (x : 0; y : -1),
        (x : -1; y : 0));
    pos : xy = (x : 1; y : 3);

    direction : integer = 3;
    visibility : integer = 4;

(* Wait for the user to strike a key *)
procedure Continue;
begin
     while KeyPressed do Readkey;
     write(Bell);
     while not KeyPressed do ;
     while KeyPressed do ReadKey;
end;

procedure Draw_Box;
begin
     Line(82, 19, 294, 19, White);
     Line(294, 19, 294, 119, White);
     Line(294, 119, 82, 119, White);
     Line(82, 119, 82, 19, White);
end;

procedure Draw_Maze;
var
   oldleft, oldright : integer;
   dist : integer;

   block, lblock, rblock : xy;

begin

     for dist := 0 to (visibility - 1) do begin

         block.x := pos.x + dist * increment[direction].x;
         block.y := pos.y + dist * increment[direction].y;

         lblock.x := block.x + left[direction].x;
         lblock.y := block.y + left[direction].y;

         rblock.x := block.x + right[direction].x;
         rblock.y := block.y + right[direction].y;

         case (dist) of

         { Draw current square }
         0 : begin
           { Is wall open to the left ? }
           { If not, draw wall }
           if (Map[block.x, block.y] <> 0) then begin
              Line(82, 19, 135, 44, White);
              Line(135, 44, 135, 93, White);
              Line(135, 93, 82, 118, White);
           end
           else begin { Else draw opening }
              Line(82, 44, 135, 44, White);
              Line(135, 44, 135, 93, White);
              Line(135, 93, 82, 93, White);
           end;

           { Is wall open to the right ? }
           { If not, draw wall }
           if Map[rblock.x, rblock.y] <> 0 then begin
              Line(294, 19, 242, 44, White);
              Line(242, 44, 242, 93, White);
              Line(294, 118, 242, 93, White);
           end
           else begin { Else draw opening }
              Line(294, 44, 242, 44, White);
              Line(242, 44, 242, 93, White);
              Line(242, 93, 294, 93, White);
           end;
         end; { Case 0 }

         { Repeat for the next square }
         1 : begin

           { Can we see the next square ? }
           { If not, draw wall }
           if (Map[block.x, block.y] <> 0) then begin
              Line(135, 44, 135, 93, White);
              Line(242, 44, 242, 93, White);
              Line(135, 44, 242, 44, White);
              Line(135, 93, 242, 93, White);
           end
           else begin { Else draw sides of the next square }
             { Left side }
             if Map[lblock.x, lblock.y] <> 0 then begin
               Line(135, 44, 162, 57, White);
               Line(162, 57, 162, 80, White);
               Line(162, 80, 135, 93, White);
             end
             else begin
               Line(135, 57, 162, 57, White);
               Line(162, 57, 162, 80, White);
               Line(162, 80, 135, 80, White);
             end;
             { Right side }
             if Map[rblock.x, rblock.y] <> 0 then begin
               Line(242, 44, 215, 57, White);
               Line(215, 57, 215, 80, White);
               Line(215, 80, 242, 93, White);
             end
             else begin
               Line(242, 57, 215, 57, White);
               Line(215, 57, 215, 80, White);
               Line(215, 80, 242, 80, White);
             end;
           end; { Draw sides }
         end; { Case 1 }

         { Do it again }
         2 : begin
           if (Map[block.x, block.y] <> 0) then begin
             Line(162, 57, 162, 80, White);
             Line(215, 57, 215, 80, White);
             Line(162, 57, 215, 57, White);
             Line(162, 80, 215, 80, White);
           end
           else begin
             if (Map[lblock.x, lblock.y]) <> 0 then begin
               Line(162, 57, 175, 63, White);
               Line(175, 63, 175, 74, White);
               Line(175, 74, 162, 80, White);
             end
             else begin
               Line(162, 63, 175, 63, White);
               Line(175, 63, 175, 74, White);
               Line(175, 74, 162, 74, White);
             end;
             if (Map[rblock.x, rblock.y] <> 0) then begin
               Line(215, 57, 202, 63, White);
               Line(202, 63, 202, 74, White);
               Line(202, 74, 215, 80, White);
             end
             else begin
               Line(215, 63, 202, 63, White);
               Line(202, 63, 202, 74, White);
               Line(202, 74, 215, 74, White);
             end;
           end;
         end; { Case 2 }

         { And again }
         3 :
         begin
           if (Map[block.x, block.y] <> 0) then begin
             Line(175, 63, 175, 74, White);
             Line(202, 63, 202, 74, White);
             Line(175, 63, 202, 63, White);
             Line(175, 74, 202, 74, White);
           end
           else begin
             if (Map[lblock.x, lblock.y] <> 0) then begin
               Line(175, 63, 182, 66, White);
               Line(182, 66, 182, 70, White);
               Line(182, 70, 175, 74, White);
             end
             else begin
               Line(175, 66, 182, 66, White);
               Line(182, 66, 182, 70, White);
               Line(182, 70, 175, 70, White);
             end;
             if (Map[rblock.x, rblock.y] <> 0) then begin
               Line(202, 63, 195, 66, White);
               Line(195, 66, 195, 70, White);
               Line(195, 70, 202, 74, White);
             end
             else begin
               Line(202, 66, 195, 66, White);
               Line(195, 66, 195, 70, White);
               Line(195, 70, 202, 70, White);
             end;
           end;
         end; { Case 3 }

       end; { Switch }

       if (Map[block.x, block.y] <> 0) then break;

     end; { For tiles in Map }

end;


begin
     InitGraph;

     Draw_Box;

     Draw_Maze;

     Continue;

     CloseGraph;
end.

