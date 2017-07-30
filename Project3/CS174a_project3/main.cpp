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

#define BUFFER_OFFSET( bytes ) ((GLvoid *)(bytes))
//using namespace std;

///////////////////////////
/* Application Variables */
///////////////////////////

//Drawable Object declaration
DrawableEntity sun;
DrawableEntity planets[5];
float pCycles[5]={0,0,0,0,0};
float pRadii[5]={35,50,75,100,10};

DrawableEntity stars;

DrawableEntity dGroundPlane;
DrawableEntity pPaths[5];

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
GLuint aGouraud_loc;

int mouseX=0;
int mouseY=0;
bool mouseControllable=true;

bool trackPlanet=false;
bool displayPaths=false;
//velocity variable for moving the camera
const float VEL=1;

/* Pressed Keys */
bool key_FWD=false;
bool key_BKWD=false;
bool key_LEFT=false;
bool key_RIGHT=false;
bool key_UP=false;
bool key_DOWN=false;

///////////////////////////
/* Application Functions */
///////////////////////////
void setModelTransMatrix(mat4 m){ //set the transformation matrix to be used for the model of the drawn object
	glUniformMatrix4fv(aModelTransMatrix_loc,1,GL_FALSE,m);
}
void setCameraTransMatrix(mat4 m){ //set the camera transformation matrix to be used
	glUniformMatrix4fv(aCameraTransMatrix_loc,1,GL_FALSE,m);
}
void setViewDirection(vec3 v){ //set the angle at which the viewer is looking
	glUniform4f(camPos_loc,(GLfloat)v.x,(GLfloat)v.y,(GLfloat)v.z,0);
}

void setLightPosition(float x, float y, float z){ //set where the light is
	glUniform4f(lightPos_loc,(GLfloat)x,(GLfloat)y,(GLfloat)z,1.0);
}
void setDiffuseColor(float r, float g, float b){ //set the color that the object will be drawn with
	glUniform4f(aColor_loc,(GLfloat)r,(GLfloat)g,(GLfloat)b,1.0);
}
void setLightColor(float r, float g, float b){ //set the lights color
	glUniform4f(alightColor_loc,(GLfloat)r,(GLfloat)g,(GLfloat)b,1.0);
}
void setAmbientColor(float r, float g, float b){ //set the ambient light color
	glUniform4f(aAmbLightColor_loc,(GLfloat)r,(GLfloat)g,(GLfloat)b,1.0);
}
void setShininess(float s){ //set the shininess to be used
	glUniform1f(aShine_loc,(GLfloat)s);
}
void setGouraud(bool t){ //set whether to use gouraud shading or not
	glUniform1i(aGouraud_loc,(GLfloat)t);
}

void animate(){ //this function is called 60 times per second before the frame is redrawn
	//MOUSE CONTROL//
	//find the mouse's distance from the center of the screen
	if(mouseControllable){
		int xDelta=mouseX-glutGet(GLUT_WINDOW_WIDTH)/2;
		int yDelta=mouseY-glutGet(GLUT_WINDOW_HEIGHT)/2;

		const int BOX_SZ=0; //half the width/height of the dead zone in the middle of the screen

		glutSetCursor(GLUT_CURSOR_NONE); //hide the cursor 

		if(abs(xDelta)>BOX_SZ){
			aCamera.rotate(0,-2*M_PI/360*xDelta/10,0);
		}
		if(abs(yDelta)>BOX_SZ){
			aCamera.rotate(-2*M_PI/360*yDelta/10,0,0);
		}

		if(key_FWD)aCamera.forward(VEL);
		if(key_BKWD)aCamera.backward(VEL);
		if(key_RIGHT)aCamera.right(VEL);
		if(key_LEFT)aCamera.left(VEL);
		if(key_UP)aCamera.translate(0,VEL,0);
		if(key_DOWN)aCamera.translate(0,-VEL,0);

		//reset the cursor to the center of the window
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH)/2,glutGet(GLUT_WINDOW_HEIGHT)/2);
	}else{
		glutSetCursor(GLUT_CURSOR_INHERIT); //show the cursor 
	}


	//set the light position to where the sun is
	setLightPosition(sun.getPosition().x,sun.getPosition().y,sun.getPosition().z);

	//animate the first four planets around the sun
	for(int i=0;i<4;i++){
		vec4 dir=planets[i].getPosition()-sun.getPosition();
		float dist=sqrt(dot(dir,dir));

		planets[i].setPosition(pRadii[i]*cos(pCycles[i]),0,pRadii[i]*sin(pCycles[i]));
		planets[i].translate(sun.getPosition());

		pPaths[i].setPosition(sun.getPosition());

		pCycles[i]+=(float)(4-i)/1000;
	}

	//animate moon
	vec4 dir=planets[4].getPosition()-planets[3].getPosition();
	float dist=sqrt(dot(dir,dir));

	planets[4].setPosition(pRadii[4]*cos(pCycles[4]),0,pRadii[4]*sin(pCycles[4]));
	planets[4].translate(planets[3].getPosition());
	pPaths[4].setPosition(planets[3].getPosition());

	pCycles[4]+=(float)(-10)/1000;

	//attach camera to the moon
	if(trackPlanet){
		aCamera.setPosition(planets[4].getPosition());
		aCamera.translate(0,3,0);
	}

}

////////////////////
/* GLUT Callbacks */
////////////////////
void callbackDisplay()
{
	setGouraud(false);
	//draw to the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear the draw buffer so that no ghosting occurs
	
	//Set up Camera
	setViewDirection(aCamera.getPosition());
	setCameraTransMatrix(aCamera.getPerspMatrix()*aCamera.getTransMatrix());

	//Draw Sun
	setAmbientColor(1,.98,.95);
	setModelTransMatrix(sun.getTransMatrix());
	sun.draw();

	setModelTransMatrix(stars.getTransMatrix());
	stars.draw();

	//reset ambient and light color
	setAmbientColor(.15,.05,.1);
	setLightColor(1,.8,.6);

	//draw first planet
	setShininess(0);
	setDiffuseColor(1,.45,.15);
	setModelTransMatrix(planets[0].getTransMatrix());
	planets[0].draw();
	//draw second planet
	setShininess(1000);
	setDiffuseColor(.55,.6,.8);
	setModelTransMatrix(planets[1].getTransMatrix());
	planets[1].draw();
	setGouraud(true);
	//draw third planet
	setShininess(50);
	setDiffuseColor(.55,.75,.6);
	setModelTransMatrix(planets[2].getTransMatrix());
	planets[2].draw();
	setGouraud(false);
	//draw fourth planet
	setDiffuseColor(.85,.85,.95);
	setModelTransMatrix(planets[3].getTransMatrix());
	planets[3].draw();
	setGouraud(true);
	//draw moon
	setDiffuseColor(.65,.85,.95);
	planets[4].setDrawType(GL_TRIANGLES);
	planets[4].setScale(1,1,1);
	setModelTransMatrix(planets[4].getTransMatrix());
	planets[4].draw();
	setGouraud(false);

	//draw the paths if it is set to
	if(displayPaths){
		setAmbientColor(.9,.9,.9);

		for(int i=0;i<5;i++){
			setModelTransMatrix(pPaths[i].getTransMatrix());
			pPaths[i].draw();
		}
	}
	glutSwapBuffers();
}
void callbackReshape(int width, int height){}// Called when the window is resized.
void callbackKeyboard(unsigned char key, int x, int y){// Called when a key is pressed. x, y is the current mouse position.

	switch(key){ //Q or q to quit
	case 'Q':
	case 'q':
		exit(0);
		break;
	case 'W': //W or w to move foward
	case 'w':
		key_FWD=true;
		break;
	case 'A': //A or a to move left
	case 'a':
		key_LEFT=true;
		break;
	case 'D': //D or d to move right
	case 'd':
		key_RIGHT=true;
		break;
	case 'S': //S or s to move back
	case 's':
		key_BKWD=true;
		break;
	case 'R': //R or r to reset the camera
	case 'r':
		aCamera.setRotation (-M_PI/6,0,0);
		aCamera.setPosition(25,105,175); //set the camera's initial position
		aCamera.setAspectRatio(1);
		aCamera.setFov(70);
		break;
	case ' ': //press space to attach camera to planet
		trackPlanet=!trackPlanet;
		break;
	case 'E': //E or e to enable the orbit paths
	case 'e':
		displayPaths=!displayPaths;
		break;      
	case 'F':
	case 'f': //enable/disable mouse control therefore hiding/showing the cursor
		mouseControllable=!mouseControllable;
		break;
	}
}
void callbackKeyboardUp(unsigned char key, int x, int y){// Called when a key is released. x, y is the current mouse position.
	switch(key){ //release W or w
	case 'W':
	case 'w':
		key_FWD=false;
		break;
	case 'A': //release A or a
	case 'a':
		key_LEFT=false;
		break;
	case 'D': //release D or d
	case 'd':
		key_RIGHT=false;
		break;
	case 'S': //release S or s
	case 's':
		key_BKWD=false;
		break;
	}
}
void callbackSpecialFunc(int key, int x, int y){// Called when a "special" non alphanumeric key is pressed
	switch(key){ //on key up, move up
	case GLUT_KEY_UP:
		key_UP=true;
		break;
	case GLUT_KEY_DOWN: //on key down, move down
		key_DOWN=true;
		break;
	}
}
void callbackSpecialUpFunc(int key, int x, int y){// called when a "special" non alphanumeric key is released
	switch(key){
	case GLUT_KEY_UP: //release key up
		key_UP=false;
		break;
	case GLUT_KEY_DOWN: //release key down
		key_DOWN=false;
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
	int sz=generateSphere(points,normals,100,100,true);

	//Create sun object
	sun.setProgram(aProgram);
	sun.loadPoints(points, normals, sz);
	sun.setDrawType(GL_TRIANGLES);
	sun.translate(30,20,0);
	sun.setScale(10,10,10);

	//create first planet based off of the sun
	planets[0]=sun;
	planets[0].setProgram(aProgram);
	planets[0].setDrawType(GL_TRIANGLES);
	planets[0].translate(10,0,0);
	planets[0].setScale(3,3,3);

	//create second planet based off of the sun
	planets[1]=sun;
	planets[1].setProgram(aProgram);
	planets[1].setDrawType(GL_TRIANGLES);
	planets[1].translate(20,0,0);
	planets[1].setScale(3,3,3);
	
	//create third planet
	sz=generateSphere(points,normals,15,15,true);
	planets[2].setProgram(aProgram);
	planets[2].loadPoints(points, normals, sz);
	planets[2].setDrawType(GL_TRIANGLES);
	planets[2].translate(30,0,0);
	planets[2].setScale(2,2,2);
	
	//create fourth planet
	sz=generateSphere(points,normals,6,6,false); //Icy Blue Planet
	planets[3].setProgram(aProgram);
	planets[3].loadPoints(points, normals, sz);
	planets[3].setDrawType(GL_TRIANGLES);
	planets[3].translate(40,0,0);
	planets[3].setScale(4,4,4);

	//create moon
	planets[4]=planets[2];
	planets[4].setProgram(aProgram);
	planets[4].setDrawType(GL_TRIANGLES);
	planets[4].translate(10,0,0);
	planets[4].setScale(1,1,1);

	//translate all planets to around the sun
	for(int i=0;i<5;i++){
		planets[i].translate(sun.getPosition());
	}
	//create the orbit paths for the planets
	for(int j=0; j<5;j++){
		for(int i=0; i<101; i++){
			float a=i*2*M_PI/100;
			points[i]=pRadii[j]*vec3(cos(a),0,sin(a));
			normals[i]=vec3(0,0,0);
		}
		pPaths[j].setProgram(aProgram);
		pPaths[j].loadPoints(points, normals, 101);
		pPaths[j].setDrawType(GL_LINE_STRIP);
		pPaths[j].translate(0,0,0);
	}

	//create the stars around the planets (points)
	for(int i=0; i<500;i++){
		points[i]=(500*(rand()%1000)/1000+300)*normalize(vec3((float)(rand()%1000)/1000-.5,(float)(rand()%1000)/1000-.5,(float)(rand()%1000)/1000-.5));
		normals[i]=vec3(1,0,0);
	}
	stars.setProgram(aProgram);
	stars.setPosition(0,0,0);
	stars.loadPoints(points,normals,500);
	stars.setDrawType(GL_POINTS);

	delete normals;
	delete points;
}
//creates the color variable for the passed program so that the color can be changed in this application
GLuint initShaderColorVariable(GLuint p){
	GLuint loc = glGetUniformLocation(p, "diffuseColor");
	if(loc!=-1){
		glUniform4f(loc,1.0,1.0,1.0,1.0);
	}
	return loc; //returns the location of the color variable
}
//creates the trans_m variable for the passed program so that the rotation can be changed in the app
GLuint initShaderTransVariable(GLuint p){
	GLuint loc = glGetUniformLocation(p, "model_trans");
	if(loc!=-1){
		GLfloat m[16]={ //identity matrix
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		};
		glUniformMatrix4fv(loc, 1, GL_FALSE, m);
	}
	return loc; //returns the location of the transmatrix variable
}
//initialize the variables used for the app
void initApp(){
	aProgram=initShader(VERTEX_SHADER_SOURCE,FRAGMENT_SHADER_SOURCE); //create the shader program
	createVertexArrays(); //create the vertex arrays

	//create uniform varibles
	aColor_loc=initShaderColorVariable(aProgram);
	aModelTransMatrix_loc=initShaderTransVariable(aProgram);
	aCameraTransMatrix_loc=glGetUniformLocation(aProgram, "camera_trans");

	alightColor_loc=glGetUniformLocation(aProgram, "lightColor");
	aAmbLightColor_loc=glGetUniformLocation(aProgram, "ambientColor");
	lightPos_loc=glGetUniformLocation(aProgram, "lightPos");
	camPos_loc=glGetUniformLocation(aProgram, "camPos");
	aShine_loc=glGetUniformLocation(aProgram, "shininess");
	aGouraud_loc=glGetUniformLocation(aProgram, "gouraud");

	//initialize the camera's position
	aCamera.setRotation (-M_PI/6,0,0);
	aCamera.setPosition(25,105,175); //set the camera's initial position
	glClearColor( .075, .025, .05, 1 ); // dark background

	//set the mouse positionr
	mouseX=glutGet(GLUT_WINDOW_WIDTH)/2;
	mouseY=glutGet(GLUT_WINDOW_HEIGHT)/2;
	glutWarpPointer(mouseX,mouseY);

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