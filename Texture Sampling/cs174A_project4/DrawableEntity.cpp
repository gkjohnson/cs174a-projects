#include "DrawableEntity.h"
#include <iostream>


void DrawableEntity::checkInitialized(){ // Checks if the entity has been initialized properly
	m_initialized=m_has_points&&m_has_program;
}
DrawableEntity::DrawableEntity():m_type(GL_POINTS), m_initialized(false), m_has_points(false), m_has_program(false){};
void DrawableEntity::setDrawType(GLenum mode){ //Sets the draw mode for the object - GL_LINES, GL_TRIANGLES, GL_POINTS
	this->m_type=mode;
}
void DrawableEntity::setProgram(GLuint newProgram){ //Sets the shader program that the object uses to render
	this->m_program=newProgram;
	this->m_has_program=true;
	checkInitialized(); //checks if the object has been initialized
}
bool DrawableEntity::loadPoints(vec3*& points, vec3*& normals, vec3*& uvs, int num){ //Loads the passed points in the GPU memory and stores the name of the buffer and vertex array
	if(!this->m_has_program)return false;

	glGenVertexArrays(1, &(this->m_vArray));
	glBindVertexArray(this->m_vArray);

	glGenBuffers(1, &(this->m_buffer));
	glBindBuffer(GL_ARRAY_BUFFER, this->m_buffer);
	//glBufferData(GL_ARRAY_BUFFER, num*sizeof(vec3), points, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, num*sizeof(vec3)*3, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, num*sizeof(vec3), points);
    glBufferSubData(GL_ARRAY_BUFFER, num*sizeof(vec3), num*sizeof(vec3), normals);
	glBufferSubData(GL_ARRAY_BUFFER, 2*num*sizeof(vec3), num*sizeof(vec3), uvs);

    // setup vertex arrays
    GLuint vPosition = glGetAttribLocation(this->m_program, "vPosition");
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vPosition);
    GLuint vNormal = glGetAttribLocation(this->m_program, "vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(num*sizeof(vec3)));
	GLuint vUV = glGetAttribLocation(this->m_program, "vUV");
    glEnableVertexAttribArray(vUV);
    glVertexAttribPointer(vUV, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(2*num*sizeof(vec3)));

	this->m_num_points=num;

	this->m_has_points=true;
	checkInitialized(); //checks if the object has been initialized
	return true; //returns success
}
bool DrawableEntity::draw(){ //draws the points to the screen
	if(!m_initialized)return false; //returns failure if the object has not been properly set up

	glUseProgram(this->m_program);
	glBindVertexArray(this->m_vArray);
	glDrawArrays( this->m_type, 0, m_num_points );

	return true;
}
void DrawableEntity::setNumPoints(int x){ //sets the number of points to be drawn
	m_num_points=x;
}