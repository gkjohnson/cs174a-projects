#include "vec.h"
#include "box.h"

float grav2=-.05;

class GameObject:public HitBox{

	vec3 m_acc;
	vec3 m_vel;

	bool m_jump;

public:
	GameObject():m_acc(vec3(0,0,0)),m_vel(vec3(0,0,0)),m_jump(false){}

	void update(){
		this->translate(m_vel);
		m_vel=m_vel+m_acc;
		setAcc(vec3(0,grav2,0));

		m_jump=false;

	}
	void jump(){
		if(m_jump){
			setVel(vec3(m_vel.x,1,m_vel.z));
			m_jump=false;
		}

	}
	void setAcc(vec3 newAcc){
		m_acc=newAcc;
	}
	void accelerate(vec3 addAcc){
		m_acc=m_acc+addAcc;
	}
	vec3 getAcc(){
		return m_acc;
	}
	void setVel(vec3 newAcc){
		m_vel=newAcc;
	}
	void vel(vec3 newAcc){
		m_vel=m_vel+newAcc;
	} 
	vec3 getVel(){
		return m_vel;
	}

	void onCollide(HitBox b){

		vec3 dP12=b.getCorner1()-getCorner2();
		vec3 dP21=b.getCorner2()-getCorner1();

		vec3 newPos=this->getPosition();
		vec3 newVel=this->getVel();

		vec3 p=this->getPosition();

		bool inXPlus=(p.x-getDim().x/2)<b.getCorner1().x&&(p.x+getDim().x/2)>b.getCorner1().x;
		bool inXMin=(p.x+getDim().x/2)>b.getCorner2().x&&(p.x-getDim().x/2)<b.getCorner2().x;
		bool inYPlus=(p.y-getDim().y/2)<b.getCorner1().y&&(p.y+getDim().y/2)>b.getCorner1().y;
		bool inYMin=(p.y+getDim().y/2)>b.getCorner2().y&&(p.y-getDim().y/2)<b.getCorner2().y;
		bool inZPlus=(p.z-getDim().z/2)<b.getCorner1().z&&(p.z+getDim().z/2)>b.getCorner1().z;
		bool inZMin=(p.z+getDim().z/2)>b.getCorner2().z&&(p.z-getDim().z/2)<b.getCorner2().z;

		bool inX=(p.x-getDim().x/4)<b.getCorner1().x&&(p.x+getDim().x/4)>b.getCorner2().x;
		bool inY=(p.y-getDim().y/4)<b.getCorner1().y&&(p.y+getDim().y/4)>b.getCorner2().y;
		bool inZ=(p.z-getDim().z/4)<b.getCorner1().z&&(p.z+getDim().z/4)>b.getCorner2().z;

		inX=(p.x+abs(newVel.x))<b.getCorner1().x&&(p.x-abs(newVel.x))>b.getCorner2().x;
		inY=(p.y+abs(newVel.y))<b.getCorner1().y&&(p.y-abs(newVel.y))>b.getCorner2().y;
		inZ=(p.z+abs(newVel.z))<b.getCorner1().z&&(p.z-abs(newVel.z))>b.getCorner2().z;

		 inX=(p.x-2*getDim().x/5+abs(newVel.x))<b.getCorner1().x&&(p.x+2*getDim().x/5-abs(newVel.x))>b.getCorner2().x;
		 inY=(p.y-2*getDim().y/5+abs(newVel.y))<b.getCorner1().y&&(p.y+2*getDim().y/5-abs(newVel.y))>b.getCorner2().y;
		 inZ=(p.z-2*getDim().z/5+abs(newVel.z))<b.getCorner1().z&&(p.z+2*getDim().z/5-abs(newVel.z))>b.getCorner2().z;


		if(inX&&inY){

			if(inZPlus&&!inZMin){
				newPos.z=b.getCorner1().z+getDim().z/2;
			}else if(inZMin&&!inZPlus){
				newPos.z=b.getCorner2().z-getDim().z/2;
			}else if((dP12.z>0&&dP21.z<0)||
				(dP12.z<0&&dP21.z>0)){
					if(getVel().z>0){
						newPos.z=b.getCorner2().z-this->getDim().z/2;
					}else if(getVel().z<0){
						newPos.z=b.getCorner1().z+this->getDim().z/2;
					}
					newVel.z=0;

			}
		}

		if(inZ&&inY){
			if(inXPlus&&!inXMin){
				newPos.x=b.getCorner1().x+getDim().x/2;
			}else if(inXMin&&!inXPlus){
				newPos.x=b.getCorner2().x-getDim().x/2;
			}else if((dP12.x>0&&dP21.x<0)||
				(dP12.x<0&&dP21.x>0)){
					if(getVel().x>0){
						newPos.x=b.getCorner2().x-this->getDim().x/2;
					}else if(getVel().x<0){
						newPos.x=b.getCorner1().x+this->getDim().x/2;
					}
					newVel.x=0;
			}
		}
		setPosition(newPos);
		dP12=b.getCorner1()-getCorner2();
		dP21=b.getCorner2()-getCorner1();

		newPos=this->getPosition();
		p=this->getPosition();

		if(inX&&inZ){
			if((dP12.y>0&&dP21.y<0)||
				(dP12.y<0&&dP21.y>0)){
					if(getVel().y>0){
						newPos.y=b.getCorner2().y-this->getDim().y/2;
					}else if(getVel().y<0){
						newPos.y=b.getCorner1().y+this->getDim().y/2;
						
					}
					newVel.y=-.2*newVel.y;
					if(abs(newVel.y)<.2){
						newVel.y=0;
					}
					m_jump=true;
			}
		}

		setPosition(newPos);
		setVel(newVel);
	}

	vec3 getDir(){
		vec4 dir=RotateY(this->getRotation().y)*vec4(0,0,-1,0);

		return vec3(dir.x,dir.y,dir.z);
	}

	void move(vec3 v, float speed){
		vec3 newVel=speed*normalize(v);
		newVel.y=m_vel.y;
		m_vel=newVel;
	}


};