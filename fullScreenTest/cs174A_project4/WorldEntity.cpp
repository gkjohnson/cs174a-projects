#include "WorldEntity.h"

WorldEntity::WorldEntity():m_pos(vec3(0,0,0)),m_rot(vec3(0,0,0)),m_scale(vec3(1,1,1)){}

void WorldEntity::setPosition(GLfloat newX, GLfloat newY, GLfloat newZ){ //set the position of the entity
	m_pos=vec3(newX,newY,newZ);
}
void WorldEntity::setPosition(vec3 v){
	this->setPosition(v.x,v.y,v.z);
}
void WorldEntity::translate(GLfloat addX, GLfloat addY, GLfloat addZ){ //translate the position of the entity relative to its current position
	m_pos=m_pos+vec3(addX,addY,addZ);
}
void WorldEntity::translate(vec3 transV){
	this->translate(transV.x,transV.y,transV.z);
}

void WorldEntity::setRotation(GLfloat rotX, GLfloat rotY, GLfloat rotZ){ //set the rotation of the entity
	m_rot=vec3(rotX,rotY,rotZ);

	if(m_rot.x>2*M_PI){
		m_rot.x-=2*M_PI;
	}else if(m_rot.x<0){
		m_rot.x+=2*M_PI;
	}
	
	if(m_rot.y>2*M_PI){
		m_rot.y-=2*M_PI;
	}else if(m_rot.y<0){
		m_rot.y+=2*M_PI;
	}
	
	if(m_rot.z>2*M_PI){
		m_rot.z-=2*M_PI;
	}else if(m_rot.z<0){
		m_rot.z+=2*M_PI;
	}

}
void WorldEntity::rotate(GLfloat addRotX, GLfloat addRotY, GLfloat addRotZ){ //rotate the entity relative to its current rotation
	setRotation(m_rot.x+addRotX,m_rot.y+addRotY,m_rot.z+addRotZ);//m_rot+vec3(addRotX,addRotY,addRotZ);
}

void WorldEntity::setScale(GLfloat scaX, GLfloat scaY, GLfloat scaZ){ //sets the scale of the entity
	m_scale=vec3(scaX,scaY,scaZ);
}
void WorldEntity::scale(GLfloat scaX, GLfloat scaY, GLfloat scaZ){ //scales the object relative to its current scale.
	m_scale=vec3(m_scale.x*scaX,m_scale.y*scaY,m_scale.z*scaZ);
}

vec3 WorldEntity::getPosition() const{ //return the position vector
	return m_pos;
}
vec3 WorldEntity::getRotation() const{ //return the rotation vector
	return m_rot;
}
vec3 WorldEntity::getScale() const{ //return the scale vector
	return m_scale;
}
mat4 WorldEntity::getTransMatrix() const{ //returns the transformation matrix for the object

	mat4 s=10*identity();
	s[0][0]=m_scale.x;
	s[1][1]=m_scale.y;
	s[2][2]=m_scale.z;
	s[3][3]=1;

	mat4 tMatrix=Translate(m_pos.x,m_pos.y,m_pos.z)*RotateX(m_rot.x)*RotateY(m_rot.y)*RotateZ(m_rot.z)*s;

	return tMatrix;
}