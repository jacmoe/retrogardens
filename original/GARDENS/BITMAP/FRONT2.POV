// MAZE.POV

#include "colors.inc"
#include "textures.inc"
#include "stones.inc"
#include "maze.inc"

#declare ZTRANS=4
#declare MazeCube=
  object {
    box { <-1 0 -1><1 2 1> }
    texture {
      WallTexture
      scale WallTextureScale
      ambient AmbientLevel
    }
  }

// A camera pointed down an aisle of the maze
camera {
  location <0 1 -2>
  look_at<0 1 3>
}

object {MazeCube translate <-4 0 ZTRANS>}
object {MazeCube translate <-2 0 ZTRANS>}
object {MazeCube translate <0 0 ZTRANS>}
object {MazeCube translate <2 0 ZTRANS>}
object {MazeCube translate <4 0 ZTRANS>}

// The floor
object {
  plane {<0 1 0> 0}
  texture {
    FloorTexture
    ambient AmbientLevel
    phong 1
    scale FloorTextureScale
    reflection FloorReflection
  }
}

// The ceiling
object {
  plane {<0 1 0> 2}
  texture {
    CeilingTexture
    scale CeilingTextureScale
    ambient 1
  }
}

object {
  light_source { <0 1 -2> color White}
}
