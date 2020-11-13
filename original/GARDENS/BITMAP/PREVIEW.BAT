copy %1.maz maze.inc
povray +itestmaze.pov +otestmaze.tga -w188 -h120 -p
pl makemap testmaze
pl maketga testmaze
tgashow testmaze.tga
