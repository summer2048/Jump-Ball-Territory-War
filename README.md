# Final-Project Group 19
## How to run
We run it in Windows system, using mingw and 'make' command.

## Group Members
Mengfei Liang (Liangm17, 400227110)
Sean Gao (gaox47, 400194815)
Yuxuan Huang (400191170)



## List of Commands  
| Command  | Description |
|---|---|
| q | quit  |
| r | clear floor |
| R | restart game (switch game model) |
| p | pause marble movement |
| 4 arrow keys | rotate the scene  |
| mouse wheel up/down | move close/further to the scene |
| left click | pick objects (Cube, Cone, Cylinder), click others to unpick |
| left click & drag | move objects around the ground |
| o | add a random object to origin |
| d | delete picked object (Cone is the base so cannot be deleted) |
| +/- | increase/decrease game speed | 
| Y | restart a game (when previous game ends) |
| n | quit (when previous game ends) |
| laft mouse & right mouse | used to drag Saturn in free mode |
| 1-5 | change current drawing color |

## List of Features
- Lighting
- Texture
- Alpha blending (objects & marbles are transparent)
- Particle system (Marble movement & bounce)
- Picking (Pick & move cube, cone, cylinder, *saturn) (Use mouse ray to choose model)
- Non-geometric primitives (Score board, start game manu, player color board)
- Collision detection (Event trigger, marble changes colors of the floor)

## Reference
We learned how to generate terrain from this website:
- https://www.codeproject.com/Articles/14154/OpenGL-Terrain-Generation-An-Introduction

We learned how to print text using glut from this link:
- https://stackoverflow.com/questions/20866508/using-glut-to-simply-print-text

We learned how to load obj files from this website:
- https://www.raywenderlich.com/2604-how-to-export-blender-models-to-opengl-es-part-1-3#toc-anchor-001

Blue sky texture image is downloaded from:
- https://www.freeimages.com/photo/blue-sky-1160827

Saturn obj file is downloaded from :
- https://free3d.com/3d-model/saturn-v1--741827.html

Heart obj file is downloaded from:
- https://free3d.com/3d-model/heart-v1--539992.html