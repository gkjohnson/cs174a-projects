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
	checkInitialized();
}
bool DrawableEntity::loadPoints(vec3*& points, int num){ //Loads the passed points in the GPU memory and stores the name of the buffer and vertex array
	glGenVertexArrays(1, &(this->m_vArray));
	glGenBuffers(1,&(this->m_buffer));
	glBindVertexArray(this->m_vArray);
	glBindBuffer( GL_ARRAY_BUFFER, this->m_buffer);

	glBufferData( GL_ARRAY_BUFFER, num*sizeof( vec3 ), points, GL_STATIC_DRAW );
	glEnableVertexAttribArray(0);

	this->m_num_points=num;

	this->m_has_points=true;
	checkInitialized();
	return true; //returns success
}
bool DrawableEntity::draw(){ //draws the points to the screen
	if(!m_initialized)return false; //returns failure if the object has not been properly set up

	glUseProgram(this->m_program);
	glBindVertexArray(this->m_vArray);
	glDrawArrays( this->m_type, 0, m_num_points );
	return true;
}
bool DrawableEntity::bindVertexShaderVariable(char* name, GLuint size, GLuint offset){ //binds a vertex shader variable that is used in the shader program passed to the object
	if(!m_initialized)return false; //returns failure if the object has not been properly set up
	GLuint loc = glGetAttribLocation( this->m_program, name );
	glEnableVertexAttribArray( loc );
	glVertexAttribPointer( loc,3, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(offset) );
	return true;
}

void DrawableEntity::setNumPoints(int x){ //sets the number of points to be drawn
	m_num_points=x;
}