#include "DrawableEntity.h"
#include "mat.h"

int generateSphere(vec3*& points, vec3*& normals, int h_div, int v_div, bool smooth){
	points=new vec3[100000];
	normals=new vec3[100000];
	float radius=1.0;
	h_div+=2;
	v_div+=0;

	float v_angle_delta=M_PI/v_div; //change in the vertical angle
	float h_angle_delta=2*M_PI/h_div; //change in the horizontal angle

	int total_points=0;
	for(int i=0; i<v_div; i++){
		for(int j=0; j<h_div;j++){
			vec3 startVec= vec3(0,1,0);

			vec4 v1=RotateY(j*h_angle_delta)*RotateX(i*v_angle_delta)*startVec;
			vec4 v2=RotateY((j+1)*h_angle_delta)*RotateX(i*v_angle_delta)*startVec;
			vec4 v3=RotateY((j+1)*h_angle_delta)*RotateX((i+1)*v_angle_delta)*startVec;
			vec4 v4=RotateY((j)*h_angle_delta)*RotateX((i+1)*v_angle_delta)*startVec;

			points[total_points]=vec3(v1.x,v1.y,v1.z);
			points[total_points+1]=vec3(v2.x,v2.y,v2.z);
			points[total_points+2]=vec3(v4.x,v4.y,v4.z);

			points[total_points+3]=vec3(v3.x,v3.y,v3.z);
			points[total_points+4]=vec3(v4.x,v4.y,v4.z);
			points[total_points+5]=vec3(v2.x,v2.y,v2.z);

			if(smooth){ //smooth shading
				normals[total_points]=points[total_points++];
				normals[total_points]=points[total_points++];
				normals[total_points]=points[total_points++];

				normals[total_points]=points[total_points++];
				normals[total_points]=points[total_points++];
				normals[total_points]=points[total_points++];
			}else if(!smooth){ //flat shading
				vec3 n=cross(normalize(points[total_points+1]-points[total_points]),normalize(points[total_points+1]-points[total_points+2]));
				n=normalize(n);
				normals[total_points++]=n;
				normals[total_points++]=n;
				normals[total_points++]=n;

				n=cross(normalize(points[total_points+1]-points[total_points]),normalize(points[total_points+1]-points[total_points+2]));
				n=normalize(n);
				normals[total_points++]=n;
				normals[total_points++]=n;
				normals[total_points++]=n;
			}

		}
	}
	return total_points;
}