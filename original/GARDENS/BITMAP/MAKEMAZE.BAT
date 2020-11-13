copy %1.maz maze.inc
povray +ifront1.pov +ofront1.tga -w188 -h120 -p
povray +ifront2.pov +ofront2.tga -w188 -h120 -p
povray +ifront3.pov +ofront3.tga -w188 -h120 -p
povray +ifront4.pov +ofront4.tga -w188 -h120 -p
povray +ifront5.pov +ofront5.tga -w188 -h120 -p
povray +iside1.pov +oside1.tga -w188 -h120 -p
povray +iside2.pov +oside2.tga -w188 -h120 -p
povray +iside3.pov +oside3.tga -w188 -h120 -p
povray +iside4.pov +oside4.tga -w188 -h120 -p
pl makemap side1
pl maketga front1
pl maketga front2
pl maketga front3
pl maketga front4
pl maketga front5
pl maketga side1
pl maketga side2
pl maketga side3
pl maketga side4
