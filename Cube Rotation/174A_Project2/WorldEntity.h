#ifndef H_WORLDOBJECT
#define H_WORLDOBJECT
#include <iostream>
#include "windows\glew\include\GL\glew.h"
#if defined(__APPLE__)
#include <GLUT/glut.h>
#elif defined(_WIN32)
#include "windows/glut/glut.h"
#else
#include <GL/glut.h>
#endif

#include "vec.h"
#include "mat.h"

//Made for objects that reside in the world
class WorldEntity{
private:
	vec3 m_pos;
	vec3 m_rot;
	vec3 m_scale;
public:
	WorldEntity();

	void setPosition(GLfloat newX, GLfloat newY, GLfloat newZ); //sets the position the object
	void setPosition(vec3 v);
	void translate(GLfloat addX, GLfloat addY, GLfloat addZ); //adds to the position of the object
	void translate(vec3 addV);

	void setRotation(GLfloat rotX, GLfloat rotY, GLfloat rotZ); //sets the rotation of hte object
	void rotate(GLfloat addRotX, GLfloat addRotY, GLfloat addRotZ); //adds to the rotation of the object

	void setScale(GLfloat scaX, GLfloat scaY, GLfloat scaZ); //sets the scale of the object
	void scale(GLfloat addScaX, GLfloat addScaY, GLfloat addScaZ); //scales the object

	vec3 getPosition() const;
	vec3 getRotation() const;
	vec3 getScale() const;
	mat4 getTransMatrix() const;
};

mat4 quaternion(vec3 axis, GLfloat theta);



#endif