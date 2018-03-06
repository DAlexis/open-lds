#!/usr/bin/gnuplot

set terminal wxt
#set pm3d map
#set pm3d interpolate 0,0
set view 0,0
splot 'output-detection-func-graph.txt' w pm3d, 'output-solutions.txt' title "Solutions", 'output-DF-positions.txt' title "Stations"

pause -1
