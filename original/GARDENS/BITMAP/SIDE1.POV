// MAZE.POV

#include "colors.inc"
#include "textures.inc"
#include "stones.inc"
#include "maze.inc"

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

object {MazeCube translate <-2 0 0>}
object {MazeCube translate <-2 0 2>}
object {MazeCube translate <-2 0 4>}
object {MazeCube translate <-2 0 6>}
object {MazeCube translate <-2 0 8>}
object {MazeCube translate <2 0 0>}
object {MazeCube translate <2 0 2>}
object {MazeCube translate <2 0 4>}
object {MazeCube translate <2 0 6>}
object {MazeCube translate <2 0 8>}

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
