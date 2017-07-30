# Cube Rotation

## Assignment Notes
#### Done
- Implemented 'q' and 'Q' to exit the program
- Implemented shaders - source within character arrays in the shaders.h file
- Created 8 cubes at (+-10,+-10,+-10) with initial camera position displaying all cubes
- Key 'x' cycles the color of the cubes ('c' overlapped with crosshair toggle) 1-9 also cycles the colors
- Implemented movement and rotation with the specified keys (up, down, left, right, i, j, k, m)
- key 'r' resets the camera position, rotation, and horizontal fov
- key 'n' and 'w' make the horizontal field of view narrower and wider
- key 'c' toggles a crosshair in the middle of the screen

#### Extra Credit
- Each of the cubes are drawn from the same vertex array using a triangle strip
- Each cube rotates individually at a constant speed

#### Other
- Draws ground plane below cubes
- 'SPACE' enables control of the camera via the mouse to allow for rotations about the y and x axis based on where the mouse is moved. 
Also allows smoother movement via the 'i','j','k','m','up', and 'down' keys

## Controls:
```
Q             Quit

X, 1-9        Change the color of the cubes
C             Toggles the crosshair in the middle of the screen

I             Move camera forward
M             Move camera backward
J             Move camera left
K             Move camera right

UP            Move camera up
DOWN          Move camera down
LEFT          Rotate camera left
RIGHT         Rotate camera right

N             Makes the horizontal field of view narrower
W             Makes the horizontal field of view wider

R             Resets the camera to its initial settings

SPACE         Toggle mouse control of the camera

Mouse move    Rotate the camera's view in the direction of the mouse's movement (when mouse control is enabled)
```
