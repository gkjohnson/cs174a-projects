#include "DrawableEntity.h"
#include <iostream>
class HitBox:public DrawableEntity{

	vec3 m_p1;
	vec3 m_p2;

public:

	HitBox(float x, float y, float z){
		m_p1=vec3(x/2,y/2,z/2);
		m_p2=-m_p1;
	}
	HitBox():m_p1(vec3(.5,.5,.5)),m_p2(-m_p1){
	}

	void setBox(float x, float y, float z){
		m_p1=vec3(x/2,y/2,z/2);
		m_p2=-m_p1;
	}

	vec3 getCorner1(){
		return m_p1+this->getPosition();
	}
	vec3 getCorner2(){
		return m_p2+this->getPosition();
	}

	bool didCollide(float x, float y, float z){

		vec3 temp1=m_p1+this->getPosition();
		vec3 temp2=m_p2+this->getPosition();


		if(x<=temp1.x&&x>=temp2.x){
			if(y<=temp1.y&&y>=temp2.y){
				if(z<=temp1.z&&z>=temp2.z){
					return true;
				}
			}
		}
			return false;
	}

	bool didCollide(HitBox b){
		vec3 dP12=b.getCorner1()-getCorner2();
		vec3 dP21=b.getCorner2()-getCorner1();

		if((dP12.x>0&&dP21.x<0)||
			(dP12.x<0&&dP21.x>0))
		{
			if((dP12.y>0&&dP21.y<0)||
				(dP12.y<0&&dP21.y>0))
			{
				if((dP12.z>0&&dP21.z<0)||
					(dP12.z<0&&dP21.z>0))
				{
					return true;
				}
			}
		}

		return false;
	}

	void onCollide(HitBox b){

	}

	vec3 getDim(){
		return 2*m_p1;
	}

};