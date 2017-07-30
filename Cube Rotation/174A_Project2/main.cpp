#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <random>

#include "DrawableEntity.h"
#include "vec.h"
#include "mat.h"
#include "shaders.h"
#include "CameraEntity.h"

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

const int DRAW_ENT_NUM=8;
DrawableEntity dEntities[DRAW_ENT_NUM];

bool drawCrossHair=false;
DrawableEntity dCrossHair;
DrawableEntity dGroundPlane;

GLuint aProgram;
GLuint aColor_loc;
GLuint aTransMatrix_loc;
CameraEntity aCamera;

int colorScalar=1;

int mouseX=0;
int mouseY=0;
bool mouseControllable=false;

const float VEL=.25;

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

void animate(){ //this function is called 60 times per second before the frame is redrawn
	//rotate each entity
	for(int i=0;i<DRAW_ENT_NUM;i++){
		dEntities[i].rotate(0,.01*(i+1),0);
	}
 
	//if the app is controllable via mouse
	if(mouseControllable){
		//find the mouse's distance from the center of teh screen
		int xDelta=mouseX-glutGet(GLUT_WINDOW_WIDTH)/2;
		int yDelta=mouseY-glutGet(GLUT_WINDOW_HEIGHT)/2;

		const int BOX_SZ=0;

		glutSetCursor(GLUT_CURSOR_NONE); //hide the cursor 

		if(abs(xDelta)>BOX_SZ){ //change the rotation of the camera based on how far the mouse is from the origin
			aCamera.rotate(0,-2*M_PI/360*xDelta/10,0);
		}
		if(abs(yDelta)>BOX_SZ){
			aCamera.rotate(-2*M_PI/360*yDelta/10,0,0);
		}

		//check if a key is being pressed and act accordingly
		if(key_FWD)aCamera.forward(VEL);
		if(key_BKWD)aCamera.backward(VEL);
		if(key_RIGHT)aCamera.right(VEL);
		if(key_LEFT)aCamera.left(VEL);
		if(key_UP)aCamera.translate(0,VEL,0);
		if(key_DOWN)aCamera.translate(0,-VEL,0);

		//reset the cursor to the center of the window
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH)/2,glutGet(GLUT_WINDOW_HEIGHT)/2);
	}else{
		glutSetCursor(GLUT_CURSOR_INHERIT); //if the mouse is not being used, then show the cursor
	}
}
void setTransMatrix(mat4 m){ //set the transformation matrix to be used for the drawn object
	glUniformMatrix4fv(aTransMatrix_loc,1,GL_FALSE,m);
	glutPostRedisplay();
}
void setColor(float r, float g, float b){ //set the color that the object will be drawn with
	glUniform4f(aColor_loc,(GLfloat)r,(GLfloat)g,(GLfloat)b,1.0);
	glutPostRedisplay();
}
////////////////////
/* GLUT Callbacks */
////////////////////
void callbackDisplay()
{glDepthMask (true); 
	//draw to the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear the draw buffer so that no ghosting occurs

	for (int i=0; i<DRAW_ENT_NUM;i++){ //draw each cube object with a different color
		float r=1;
		float g=(float)((i+colorScalar)%DRAW_ENT_NUM)/DRAW_ENT_NUM;
		float b=.5-(float)((i+colorScalar)%DRAW_ENT_NUM)/DRAW_ENT_NUM;

		setColor(r,g,b);
		mat4 m=aCamera.getPerspMatrix()*aCamera.getTransMatrix()*dEntities[i].getTransMatrix();
		setTransMatrix(m);

		dEntities[i].draw();
	}

	//Draw the ground plane
	setColor(.12,.12,.12);
	mat4 m=aCamera.getPerspMatrix()*aCamera.getTransMatrix()*dGroundPlane.getTransMatrix();
	setTransMatrix(m);
	dGroundPlane.draw();

	//draw the crosshair if applicable
	if(drawCrossHair){
		setColor(1,1,1);
		setTransMatrix(identity());
		dCrossHair.draw();
	}
	//
//	mat4 m=aCamera.getPerspMatrix()*aCamera.getTransMatrix()*dGroundPlane.getTransMatrix();
//	setTransMatrix(m);
	glutSwapBuffers();
}
void callbackReshape(int width, int height){}// Called when the window is resized.
void callbackKeyboard(unsigned char key, int x, int y){// Called when a key is pressed. x, y is the current mouse position.
	if(key=='Q'||key=='q'){
		exit(0);
	}else if(key=='C'||key=='c'){
		// toggle Display crosshair in the middle of the screen
		drawCrossHair=!drawCrossHair;
	}else if(key=='I'||key=='i'){ //FORWARD
		//Move camera forward relative to current angle 
		aCamera.forward(VEL);
		key_FWD=true;
	}else if(key=='J'||key=='j'){ //LEFT
		//Move camera left relative to current angle
		aCamera.left(VEL);
		key_LEFT=true;
	}else if(key=='K'||key=='k'){ //RIGHT
		//Move camera right relative to current angle
		aCamera.right(VEL);
		key_RIGHT=true;
	}else if(key=='M'||key=='m'){ //BACK
		//Move camera back relative to current angle 
		aCamera.backward(VEL);
		key_BKWD=true;
	}else if(key=='R'||key=='r'){ //RESET Camera position
		// Reset camera position to origin and angles to 0
		aCamera.setPosition(0,0,45);
		aCamera.setRotation(0,0,0);
		aCamera.setAspectRatio(1);
	}else if(isdigit(key)&&key!='0'){
		//goes to a specific set of colors
		colorScalar=key-'0';
	}else if(key=='x'||key=='X'){
		//cycles through the colors
		colorScalar++;
		colorScalar=colorScalar%8+1;
	}else if(key=='n'||key=='N'){
		//Make horizontal viewing angle narrow
		aCamera.addToAspectRatio(-.01);
	}else if(key=='w'||key=='W'){
		// Make horizontal viewing angle narrow
		aCamera.addToAspectRatio(.01);
	}else if(key==' '){
		//set application to mouse controllable
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH)/2,glutGet(GLUT_WINDOW_HEIGHT)/2);
		mouseControllable=!mouseControllable;
	}
}
void callbackKeyboardUp(unsigned char key, int x, int y){// Called when a key is released. x, y is the current mouse position.
	//check if the keys are released
	if(key=='i'||key=='I')key_FWD=false;
	else if(key=='m'||key=='M')key_BKWD=false;
	else if(key=='k'||key=='K')key_RIGHT=false;
	else if(key=='j'||key=='J')key_LEFT=false;

}
void callbackSpecialFunc(int key, int x, int y){// Called when a "special" non alphanumeric key is pressed
	if(key==GLUT_KEY_UP){
		// Move camera vertically up
		aCamera.translate(0,VEL,0);
		key_UP=true;
	}else if(key==GLUT_KEY_DOWN){
		// Move camera vertically down
		aCamera.translate(0,-VEL,0);
		key_DOWN=true;
	}else if(key==GLUT_KEY_LEFT){
		// rotate the camera left
		aCamera.rotate(0,2*M_PI/360,0);
	}else if(key==GLUT_KEY_RIGHT){
		// rotate the camera right
		aCamera.rotate(0,-2*M_PI/360,0);
	}
}
void callbackSpecialUpFunc(int key, int x, int y){// called when a "special" non alphanumeric key is released
	//check if the keys are released
	if(key==GLUT_KEY_UP)key_UP=false;
	else if(key==GLUT_KEY_DOWN)key_DOWN=false;
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
	glutCreateWindow("Cube Rotation");
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
	vec3* points=new vec3[14];
	points[0]=vec3(0.5, 0.5, 0.5);
	points[1]=vec3(-0.5, 0.5, 0.5);
	points[2]=vec3(0.5,-0.5, 0.5);
	points[3]=vec3(-0.5,-0.5, 0.5);
	points[4]=vec3(-0.5,-0.5,-0.5);
	points[5]=vec3(-0.5, 0.5, 0.5);
	points[6]=vec3(-0.5, 0.5,-0.5);
	points[7]=vec3(0.5, 0.5, 0.5);
	points[8]=vec3(0.5, 0.5,-0.5);
	points[9]=vec3(0.5,-0.5, 0.5);
	points[10]=vec3(0.5,-0.5,-0.5);
	points[11]=vec3(-0.5,-0.5,-0.5);
	points[12]=vec3(0.5, 0.5,-0.5);
	points[13]=vec3(-0.5, 0.5,-0.5);
	
	dEntities[0].loadPoints(points, 14);
	dEntities[0].setProgram(aProgram);
	dEntities[0].setDrawType(GL_TRIANGLE_STRIP);
	dEntities[0].bindVertexShaderVariable("vPosition");
	dEntities[0].bindVertexShaderVariable("normals",3,14*sizeof(vec3));
	dEntities[0].setNumPoints(14);
	dEntities[0].setScale(5,5,5);

	for(int i=1; i<DRAW_ENT_NUM;i++){
		dEntities[i]=dEntities[0];
	}
	//set the different positions of each cube object
	dEntities[0].setPosition(-10,-10,-10);
	dEntities[1].setPosition(10,-10,-10);
	dEntities[2].setPosition(-10,10,-10);
	dEntities[3].setPosition(10,10,-10);
	dEntities[4].setPosition(10,10,10);
	dEntities[5].setPosition(-10,10,10);
	dEntities[6].setPosition(10,-10,10);
	dEntities[7].setPosition(-10,-10,10);

	//create the crosshair vertexArray
	points[0]=vec3(0,.03,-.5);
	points[1]=vec3(0,-.03,-.5);
	points[2]=vec3(.03,0,-.5);
	points[3]=vec3(-.03,0,-.5);

	dCrossHair.loadPoints(points,4);
	dCrossHair.setProgram(aProgram);
	dCrossHair.setDrawType(GL_LINES);
	dCrossHair.bindVertexShaderVariable("vPosition");
	
	//create the groundplane vertexArray
	points[0]=vec3(1,0,-1);
	points[1]=vec3(1,0,1);
	points[2]=vec3(-1,0,1);
	points[3]=vec3(-1,0,-1);

	dGroundPlane.setScale(50,50,50);
	dGroundPlane.setPosition(0,-20,0);
	dGroundPlane.loadPoints(points,4);
	dGroundPlane.setProgram(aProgram);
	dGroundPlane.setDrawType(GL_QUADS);
	dGroundPlane.bindVertexShaderVariable("vPosition");

	delete points;
}
//creates the color variable for the passed program so that the color can be changed in this application
GLuint initShaderColorVariable(GLuint p){
	GLuint loc = glGetUniformLocation(p, "color");
	if(loc!=-1){
		glUniform4f(loc,1.0,1.0,1.0,1.0);
	}
	return loc; //returns the location of the color variable
}
//creates the trans_m variable for the passed program so that the rotation can be changed in the app
GLuint initShaderTransVariable(GLuint p){
	GLuint loc = glGetUniformLocation(p, "trans_m");
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
void initApp(){
	aProgram=initShader(VERTEX_SHADER_SOURCE,FRAGMENT_SHADER_SOURCE); //create the shader
	createVertexArrays(); //create the vertex arrays

	aColor_loc=initShaderColorVariable(aProgram); //create the uniform color variable
	aTransMatrix_loc=initShaderTransVariable(aProgram); //create teh transformation matrix variable

	aCamera.setPosition(0,0,45); //set the camer'a initial position
	glClearColor( .1, .1, .1, .1 ); // dark background
 
	mouseX=glutGet(GLUT_WINDOW_WIDTH)/2;
	mouseY=glutGet(GLUT_WINDOW_HEIGHT)/2;

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