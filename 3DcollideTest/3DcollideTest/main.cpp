#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <random>

#include "DrawableEntity.h"
#include "vec.h"
#include "mat.h"
#include "shaders.h"
#include "CameraEntity.h"
#include "planet.h"

#include "windows\glew\include\GL\glew.h"
#if defined(__APPLE__)
#include <GLUT/glut.h>
#elif defined(_WIN32)
#include "windows/glut/glut.h"
#else
#include <GL/glut.h>
#endif

#include "gameobject.h"


#define BUFFER_OFFSET( bytes ) ((GLvoid *)(bytes))
//using namespace std;

///////////////////////////
/* Application Variables */
///////////////////////////

//Drawable Object declaration
HitBox quads[7];

//Camera
CameraEntity aCamera;

//Save the program and variable locations
GLuint aProgram;
GLuint aColor_loc;
GLuint aModelTransMatrix_loc;
GLuint aCameraTransMatrix_loc;

GLuint alightColor_loc;
GLuint aAmbLightColor_loc;
GLuint lightPos_loc;
GLuint aShine_loc;
GLuint camPos_loc;

int mouseX=0;
int mouseY=0;
bool mouseControllable=true;

bool trackPlanet=false;
bool displayPaths=false;
//velocity variable for moving the camera
const float VEL=.5;

/* Pressed Keys */
bool key_FWD=false;
bool key_BKWD=false;
bool key_LEFT=false;
bool key_RIGHT=false;
bool key_UP=false;
bool key_DOWN=false;

//GLOBALS//
vec3 grav=vec3(0,-1,0);

GameObject mainChar;

///////////////////////////
/* Application Functions */
///////////////////////////
void setModelTransMatrix(mat4 m){ //set the transformation matrix to be used for the drawn object
	glUniformMatrix4fv(aModelTransMatrix_loc,1,GL_FALSE,m);
}
void setCameraTransMatrix(mat4 m){
	glUniformMatrix4fv(aCameraTransMatrix_loc,1,GL_FALSE,m);
}
void setViewDirection(vec3 v){
	glUniform4f(camPos_loc,(GLfloat)v.x,(GLfloat)v.y,(GLfloat)v.z,0);
}

void setLightPosition(float x, float y, float z){
	glUniform4f(lightPos_loc,(GLfloat)x,(GLfloat)y,(GLfloat)z,1.0);
}
void setDiffuseColor(float r, float g, float b){ //set the color that the object will be drawn with
	glUniform4f(aColor_loc,(GLfloat)r,(GLfloat)g,(GLfloat)b,1.0);
}
void setLightColor(float r, float g, float b){
	glUniform4f(alightColor_loc,(GLfloat)r,(GLfloat)g,(GLfloat)b,1.0);
}
void setAmbientColor(float r, float g, float b){
	glUniform4f(aAmbLightColor_loc,(GLfloat)r,(GLfloat)g,(GLfloat)b,1.0);
}
void setShininess(float s){
	glUniform1f(aShine_loc,(GLfloat)s);
}

void animate(){ //this function is called 60 times per second before the frame is redrawn
	
	//MOUSE CONTROL//
	if(mouseControllable){
		//find the mouse's distance from the center of the screen
		int xDelta=mouseX-glutGet(GLUT_WINDOW_WIDTH)/2;
		int yDelta=mouseY-glutGet(GLUT_WINDOW_HEIGHT)/2;

		const int BOX_SZ=0; //half the width/height of the dead zone in the middle of the screen

		glutSetCursor(GLUT_CURSOR_NONE); //hide the cursor 

		if(abs(xDelta)>BOX_SZ){
			mainChar.rotate(0,-2*M_PI/360*xDelta/10,0);
		}
		if(abs(yDelta)>BOX_SZ){
			aCamera.rotate(-2*M_PI/360*yDelta/10,0,0);
			if(aCamera.getRotation().x>M_PI/2&&aCamera.getRotation().x<3*M_PI/2){
				
				if(yDelta>0){	
					aCamera.setRotation(3*M_PI/2,0,0);
				}else if(yDelta<0){
					aCamera.setRotation(M_PI/2,0,0);
				}
			
			}

		std::cout<<aCamera.getRotation().x<<std::endl;

		}

		//if(key_FWD)aCamera.forward(VEL);
		//if(key_BKWD)aCamera.backward(VEL);
		//if(key_RIGHT)aCamera.right(VEL);
		//if(key_LEFT)aCamera.left(VEL);
		//if(key_UP)aCamera.translate(0,VEL,0);
		//if(key_DOWN)aCamera.translate(0,-VEL,0);


		//if(key_DOWN)aCamera.translate(0,-VEL,0);


		//reset the cursor to the center of the window
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH)/2,glutGet(GLUT_WINDOW_HEIGHT)/2);
	}


	//if(key_FWD)mainChar.setVel(vec3(mainChar.getVel().x,mainChar.getVel().y,-VEL));
	//if(key_BKWD)mainChar.setVel(vec3(mainChar.getVel().x,mainChar.getVel().y,VEL));
	//if(key_RIGHT)mainChar.setVel(vec3(VEL,mainChar.getVel().y,mainChar.getVel().z));
	//if(key_LEFT)mainChar.setVel(vec3(-VEL,mainChar.getVel().y,mainChar.getVel().z));

	vec4 leftDir=RotateY(M_PI/2)*vec4(mainChar.getDir(),0);


		mainChar.setVel(vec3(0,mainChar.getVel().y,0));
	if(key_FWD)mainChar.move(mainChar.getDir(),VEL);
	if(key_BKWD)mainChar.move(mainChar.getDir(),-VEL);
	if(key_RIGHT)mainChar.move(vec3(leftDir.x,leftDir.y,leftDir.z),-VEL);
	if(key_LEFT)mainChar.move(-vec3(leftDir.x,leftDir.y,leftDir.z),-VEL);

	if(key_UP)mainChar.jump();
	mainChar.update();

	for(int i=0;i<7;i++){
		if(quads[i].didCollide(mainChar)){
			mainChar.onCollide(quads[i]);
		}
	}

	aCamera.setPosition(mainChar.getPosition()+vec3(0,15,9));
	aCamera.setPosition(mainChar.getPosition()+-9*mainChar.getDir());
	aCamera.translate(0,10,0);

	aCamera.setRotation(aCamera.getRotation().x,mainChar.getRotation().y,mainChar.getRotation().z);

}

////////////////////
/* GLUT Callbacks */
////////////////////
void callbackDisplay()
{
	//draw to the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear the draw buffer so that no ghosting occurs

	//set the camera information
	setViewDirection(aCamera.getPosition());
	setCameraTransMatrix(aCamera.getPerspMatrix()*aCamera.getTransMatrix());
	
	//Set the light and material properties
	setAmbientColor(.1,.1,.1);
	setLightColor(1,1,1);
	setLightPosition(10,10,10);
	setShininess(100);
	setDiffuseColor(1,.75,.75);
	
	//draw both squares
	for(int i=0;i<7;i++){
	setModelTransMatrix(quads[i].getTransMatrix());
	quads[i].draw();	
	}

	setDiffuseColor(.75,1,1);

	setModelTransMatrix(mainChar.getTransMatrix());
	mainChar.draw();




	glutSwapBuffers();
}
void callbackReshape(int width, int height){}// Called when the window is resized.
void callbackKeyboard(unsigned char key, int x, int y){// Called when a key is pressed. x, y is the current mouse position.

	switch(key){
	case 'Q':
	case 'q':
		exit(0);
		break;
	case 'W':
	case 'w':
		key_FWD=true;
		break;
	case 'A':
	case 'a':
		key_LEFT=true;
		break;
	case 'D':
	case 'd':
		key_RIGHT=true;
		break;
	case 'S':
	case 's':
		key_BKWD=true;
		break;
	case 'R':
	case 'r':
		aCamera.setRotation (0,0,0);
		aCamera.setPosition(0,0,2); //set the camera's initial position
		aCamera.setAspectRatio(1);
		aCamera.setFov(70);
		break;
	case 'F':
	case 'f':
		mouseControllable=!mouseControllable;
		break;
	case ' ':
		key_UP=true;
		break;
	}
}
void callbackKeyboardUp(unsigned char key, int x, int y){// Called when a key is released. x, y is the current mouse position.
	switch(key){
	case 'W':
	case 'w':
		key_FWD=false;
		break;
	case 'A':
	case 'a':
		key_LEFT=false;
		break;
	case 'D':
	case 'd':
		key_RIGHT=false;
		break;
	case 'S':
	case 's':
		key_BKWD=false;
		break;
	case ' ':
		key_UP=false;
		break;
	}
}
void callbackSpecialFunc(int key, int x, int y){// Called when a "special" non alphanumeric key is pressed
	switch(key){
	case GLUT_KEY_UP:
		//key_UP=true;
		break;
	case GLUT_KEY_DOWN:
		//key_DOWN=true;
		break;
	}
}
void callbackSpecialUpFunc(int key, int x, int y){// called when a "special" non alphanumeric key is released
	switch(key){
	case GLUT_KEY_UP:
		//key_UP=false;
		break;
	case GLUT_KEY_DOWN:
		//key_DOWN=false;
		break;
	}
}
void callbackMouse(int button, int state, int x, int y){}// Called when a mouse button is pressed or released
void callbackMotion(int x, int y){// Called when the mouse is moved with a button pressed
	mouseX=x;
	mouseY=y;
}
void callbackPassiveMotion(int x, int y){// Called when the mouse is moved with no buttons pressed
	mouseX=x;
	mouseY=y;
}
void callbackIdle(){}// Called when the system is idle. Can be called many times per frame.
void callbackTimer(int) // Called when the timer expires
{
	//redraw and animate 60 times per second
	animate();
	glutTimerFunc(1000/60, callbackTimer, 0);
	glutPostRedisplay();
}



////////////////////
/* Initialization */
////////////////////

//initialize the window
void initGlut(int& argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 50);
	glutInitWindowSize(600, 600);
	glutCreateWindow("G Test");
}

//bind callback functions to glut
void initCallbacks()
{
	glutDisplayFunc(callbackDisplay);
	glutReshapeFunc(callbackReshape);
	glutKeyboardFunc(callbackKeyboard);
	glutKeyboardUpFunc(callbackKeyboardUp);
	glutSpecialFunc(callbackSpecialFunc);
	glutSpecialUpFunc(callbackSpecialUpFunc);
	glutMouseFunc(callbackMouse);
	glutMotionFunc(callbackMotion);
	glutPassiveMotionFunc(callbackPassiveMotion);
	glutIdleFunc(callbackIdle);
	glutTimerFunc(1000/30, callbackTimer, 0);
}

//Initialize the application
GLuint initShader(const char* vShaderSource, const char* fShaderSource){
	GLuint program=glCreateProgram();

	/* compile and attach Vertex Shader */
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER); //get handle to new Shader
	glShaderSource(vShader, 1, (const GLchar**)&vShaderSource, NULL); //copy source to GPU
	glCompileShader(vShader); //compile the shader source
	
	//verify that the shader was compiled properly
	GLint compiled; 
	glGetShaderiv( vShader, GL_COMPILE_STATUS, &compiled );
	if(!compiled){
		std::cout<<"Error compiling Vertex Shader";

        char buffer[512];
        glGetShaderInfoLog(vShader, 512, NULL, buffer);
        std::cout << buffer << std::endl;

		exit(EXIT_FAILURE);
	}
	glAttachShader( program, vShader );

	/* compile and attach Fragment Shader */
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER); 
	glShaderSource(fShader, 1, (const GLchar**)&fShaderSource, NULL);
	glCompileShader(fShader);

	//verify that the shader was compiled properly
	glGetShaderiv( fShader, GL_COMPILE_STATUS, &compiled );
	if(!compiled){
		std::cout<<"Error compiling Fragment Shader";

        char buffer[512];
        glGetShaderInfoLog(fShader, 512, NULL, buffer);
        std::cout << buffer << std::endl;

        exit(EXIT_FAILURE);
	}
	glAttachShader( program, fShader );

	/* Link the new program */
	glLinkProgram(program);
	GLint linked;
	glGetProgramiv( program, GL_LINK_STATUS, &linked ); //verify that program was linked successfully
	if ( !linked ) {
		std::cout<<"Error linking program";
		exit(EXIT_FAILURE);
	}
	glUseProgram(program); //use the program that was just created when rendering

	return program; //returns the program with the passed shaders
}

void createVertexArrays(){

	//Set up the cube objects (all objects use the same vertex array)
	vec3* points=0;
	vec3* normals=0;
	vec3* uv=0;
	//create the points for the plane
	points=new vec3[24];
	points[0]=vec3(-1,0,1);
	points[1]=vec3(1,0,1);
	points[2]=vec3(1,0,-1);
	points[3]=vec3(-1,-0,-1);
	//create the normals for the plane
	normals=new vec3[24];
	normals[0]=vec3(0,1,0);
	normals[1]=vec3(0,1,0);
	normals[2]=vec3(0,1,0);
	normals[3]=vec3(0,1,0);



	//top square
	points[0]=vec3(.5,.5,.5);
	points[1]=vec3(-.5,.5,.5);
	points[2]=vec3(-.5,.5,-.5);
	points[3]=vec3(.5,.5,-.5);
	//bottom squard
	points[4]=vec3(.5,-.5,.5);
	points[5]=vec3(-.5,-.5,.5);
	points[6]=vec3(-.5,-.5,-.5);
	points[7]=vec3(.5,-.5,-.5);
	//right square
	points[8]=vec3(.5,.5,.5);
	points[9]=vec3(.5,-.5,.5);
	points[10]=vec3(.5,-.5,-.5);
	points[11]=vec3(.5,.5,-.5);
	//left square
	points[12]=vec3(-.5,.5,.5);
	points[13]=vec3(-.5,-.5,.5);
	points[14]=vec3(-.5,-.5,-.5);
	points[15]=vec3(-.5,.5,-.5);
	//front square
	points[16]=vec3(.5,.5,-.5);
	points[17]=vec3(-.5,.5,-.5);
	points[18]=vec3(-.5,-.5,-.5);
	points[19]=vec3(.5,-.5,-.5);
	//back square
	points[20]=vec3(.5,.5,.5);
	points[21]=vec3(-.5,.5,.5);
	points[22]=vec3(-.5,-.5,.5);
	points[23]=vec3(.5,-.5,.5);

	normals[0]=normals[1]=normals[2]=normals[3]=vec3(0,1,0);
	normals[4]=normals[5]=normals[6]=normals[7]=vec3(0,-1,0);
	normals[8]=normals[9]=normals[10]=normals[11]=vec3(1,0,0);
	normals[12]=normals[13]=normals[14]=normals[15]=vec3(-1,0,0);
	normals[16]=normals[17]=normals[18]=normals[19]=vec3(0,0,-1);
	normals[20]=normals[21]=normals[22]=normals[23]=vec3(0,0,1);

	//create the quads
	quads[0].setProgram(aProgram);
	quads[0].loadPoints(points,normals,points,4);
	quads[0].setDrawType(GL_QUADS);
	quads[0].scale(10,10,10);

	mainChar.setProgram(aProgram);
	mainChar.loadPoints(points,points,points,24);
	mainChar.setDrawType(GL_QUADS);
	mainChar.scale(1,1,1);
	mainChar.translate(0,0,0);
	mainChar.setBox(1,1,1);

	quads[1].setProgram(aProgram);
	quads[1].loadPoints(points,normals,points,24);
	quads[1].setDrawType(GL_QUADS);
	quads[1].setScale(30,20,30);
	quads[1].translate(0,-10,0);
	quads[1].setBox(30,20,30);

	quads[0]=quads[1];
	quads[0].setScale(30,5,2);
	quads[0].setPosition(0,2.5,-15);
	quads[0].setBox(30,5,2);
	
	quads[2]=quads[0];
	quads[2].setPosition(0,2.5,15);

	quads[3]=quads[0];
	quads[3].setScale(2,5,30);
	quads[3].setPosition(-15,2.5,0);
	quads[3].setBox(2,5,30);

	quads[4]=quads[3];
	quads[4].setPosition(15,2.5,0);

	quads[5]=quads[0];
	quads[5].setScale(10,1,10);
	quads[5].setPosition(-25,10,0);
	quads[5].setBox(10,2.5,10);

	quads[6]=quads[5];
	quads[6].translate(-15,5,5);

	delete normals;
	delete points; //deletes both uvs and points
}
//initialize the variables used for the app
void initApp(){
	aProgram=initShader(VERTEX_SHADER_SOURCE,FRAGMENT_SHADER_SOURCE); //create the shader program

	//create uniform varibles

	aColor_loc = glGetUniformLocation(aProgram, "diffuseColor");
	aModelTransMatrix_loc = glGetUniformLocation(aProgram, "model_trans");
	aCameraTransMatrix_loc=glGetUniformLocation(aProgram, "camera_trans");

	alightColor_loc=glGetUniformLocation(aProgram, "lightColor");
	aAmbLightColor_loc=glGetUniformLocation(aProgram, "ambientColor");
	lightPos_loc=glGetUniformLocation(aProgram, "lightPos");
	camPos_loc=glGetUniformLocation(aProgram, "camPos");
	aShine_loc=glGetUniformLocation(aProgram, "shininess");

	aCamera.setRotation (-M_PI/4,0,0);
	aCamera.setPosition(0,1,15); //set the camera's initial position
	glClearColor( .05, .075, .1, 1 ); // dark background

	mouseX=glutGet(GLUT_WINDOW_WIDTH)/2;
	mouseY=glutGet(GLUT_WINDOW_HEIGHT)/2;
	glutWarpPointer(mouseX,mouseY);

	createVertexArrays(); //create the vertex arrays

	//Set openGL to use a depth buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

int main(int argc, char** argv)
{
	//initialize glut and glew
	initGlut(argc, argv);
	glewInit();
	initApp();

	initCallbacks();

	//start glut main loop
	glutMainLoop();
	return 0;
}