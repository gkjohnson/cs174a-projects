Project 2:
Done:
- Implemented 'q' and 'Q' to exit the program
- Implemented shaders - source within character arrays in the shaders.h file
- Created 8 cubes at (+-10,+-10,+-10) with initial camera position displaying all cubes
- Key 'x' cycles the color of the cubes ('c' overlapped with crosshair toggle) 1-9 also cycles the colors
- Implemented movement and rotation with the specified keys (up, down, left, right, i, j, k, m)
- key 'r' resets the camera position, rotation, and horizontal fov
- key 'n' and 'w' make the horizontal field of view narrower and wider
- key 'c' toggles a crosshair in the middle of the screen

Extra Credit:
- Each of the cubes are drawn from the same vertex array using a triangle strip
- Each cube rotates individually at a constant speed

Other:
- Draws ground plane below cubes
- key 'SPACE' enables control of the camera via the mouse to allow for rotations about the y and x axis based on where the mouse is moved. 
Also allows smoother movement via the 'i','j','k','m','up', and 'down' keys

Controls:
'Q', 'q' : Quit

'X' 'x' '1'-'9' : Change the color of the cubes
'C' 'c' : toggles the crosshair in the middle of the screen

'I' 'i' : move camera forward
'M' 'm' : move camera backward
'J' 'j' : move camera left
'K' 'k' : move camera right

'UP' : move camera up
'DOWN' : move camera down
'LEFT' : rotate camera left
'RIGHT' : rotate camera right

'N' 'n' : makes the horizontal field of view narrower
'W' 'n' : makes the horizontal field of view wider

'R' 'r' : resets the camera to its initial settings

'SPACE' : Toggle mouse control of the camera

Mouse move : (when mouse control is enabled) rotate the camera's view in the direction of the mouse's movement