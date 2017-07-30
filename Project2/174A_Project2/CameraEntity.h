#ifndef H_CAMERA
#define H_CAMERA

#include "vec.h"
#include "WorldEntity.h"

// Camera object
class CameraEntity : public WorldEntity{
	GLfloat m_fov; //Field of Vision
	GLfloat m_aspectRatio; //AspectRatio
	GLfloat m_nearClip; //Near Clipplane
	GLfloat m_farClip; //Far Clipplane

public:
	CameraEntity():m_nearClip(1),m_farClip(500),m_fov(50),m_aspectRatio(1){}
	//set or add to field of vision
	void setFov(GLfloat fov){
		m_fov=fov;
	}
	void addFov(GLfloat fov){
		m_fov=m_fov+fov;
	}
	//move forward amount c (relative to current angle)
	void forward(float c){
		vec3 r=getRotation();
		vec4 v=RotateY(r.y)*RotateX(r.x)*RotateZ(r.z)*vec4(0,0,-1,0);
	
		float n=sqrt(v.y*v.y+v.x*v.x+v.z*v.z);
		v=v/n;
		v=c*v;

		translate(v.x,v.y,v.z);
	}
	//move backward amount c
	void backward(float c){
		forward(-c);
	}
	//move left (relative to current angle) amoutnt c
	void left(float c){
		vec3 r=getRotation();
		vec4 v=RotateY(r.y)*RotateX(r.x)*RotateZ(r.z)*vec4(-1,0,0,0);
	
		float n=sqrt(v.y*v.y+v.x*v.x+v.z*v.z);
		v=v/n;
		v=c*v;

		translate(v.x,v.y,v.z);
	}
	//move right amount c
	void right(float c){
		left(-c);
	}
	//get the perspective matrix for this camera
	mat4 getPerspMatrix() const{
		return Perspective(m_fov,m_aspectRatio,m_nearClip,m_farClip);
	}
	//get the transformation matrix for the camera (reversed translates and rotations)
	mat4 getTransMatrix() const{
		vec3 r=getRotation();
		mat4 tMatrix=RotateX(-r.x)*RotateY(-r.y)*RotateZ(-r.z)*Translate(-getPosition().x,-getPosition().y,-getPosition().z);

		return tMatrix;
	}
	//set or add to aspect ratio
	void setAspectRatio(float c){
		m_aspectRatio=c;

		if(m_aspectRatio<.5){
			m_aspectRatio=.5;
		}else if(m_aspectRatio>5){
			m_aspectRatio=5;
		}

	}
	void addToAspectRatio(float c){
		setAspectRatio(m_aspectRatio+c);
	}

};

#endif