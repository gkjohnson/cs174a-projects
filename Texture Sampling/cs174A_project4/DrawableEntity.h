#ifndef H_DRAWABLE_OBJECT
#define H_DRAWABLE_OBJECT
#include <iostream>

#include "windows\glew\include\GL\glew.h"

#include "vec.h"
#include "WorldEntity.h"
#include "CameraEntity.h"
#define BUFFER_OFFSET( bytes ) ((GLvoid *)(bytes))

class DrawableEntity : public WorldEntity{ //Struct for a drawable object to keep track of Vertex Array and Buffer pointers
private:
	GLenum m_type;
	GLuint m_program;
	GLuint m_vArray;
	GLuint m_buffer;
	GLuint m_num_points;

	bool m_initialized;
	bool m_has_points;
	bool m_has_program;

	void checkInitialized();
public:
	DrawableEntity();
	void setDrawType(GLenum mode); //Sets the draw mode for the object - GL_LINES, GL_TRIANGLES, GL_POINTS
	void setProgram(GLuint newProgram); //Sets the shader program that the object uses to render
	bool loadPoints(vec3*& points, vec3*& normals, vec3*& uvs, int num); //Loads the passed points in the GPU memory and stores the name of the buffer and vertex array
	bool draw(); //draw the vertex array
	void setNumPoints(int x); //set the number of vertices to draw
};


#endif