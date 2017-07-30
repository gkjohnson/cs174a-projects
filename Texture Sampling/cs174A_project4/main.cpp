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
#include "windows\devil\include\IL\il.h"


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
DrawableEntity quads[2];

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
bool mouseControllable=false;

bool trackPlanet=false;
bool displayPaths=false;
//velocity variable for moving the camera
const float VEL=0.1;

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
			aCamera.rotate(0,-2*M_PI/360*xDelta/10,0);
		}
		if(abs(yDelta)>BOX_SZ){
			aCamera.rotate(-2*M_PI/360*yDelta/10,0,0);
		}

		//reset the cursor to the center of the window
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH)/2,glutGet(GLUT_WINDOW_HEIGHT)/2);

	}else{
		glutSetCursor(GLUT_CURSOR_INHERIT);
	}
	if(key_FWD)aCamera.forward(VEL);
	if(key_BKWD)aCamera.backward(VEL);
	if(key_RIGHT)aCamera.right(VEL);
	if(key_LEFT)aCamera.left(VEL);
	if(key_UP)aCamera.translate(0,VEL,0);
	if(key_DOWN)aCamera.translate(0,-VEL,0);


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
	setAmbientColor(.1,.05,.075);
	setLightColor(1,1,1);
	setLightPosition(0,0,10);
	setShininess(0);
	
	//draw square with zoomed out texture
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	setModelTransMatrix(quads[0].getTransMatrix());
	quads[0].draw();


	//draw square with normal zoom texture
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	setModelTransMatrix(quads[1].getTransMatrix());
	quads[1].draw();

	glutSwapBuffers();
}
void callbackReshape(int width, int height){
	std::cout<<"resized "<<width<<" "<<height<<std::endl;
	glViewport(0,0,width,height);
}// Called when the window is resized.
void callbackKeyboard(unsigned char key, int x, int y){// Called when a key is pressed. x, y is the current mouse position.

	switch(key){
	case 27:
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
		aCamera.setPosition(0,0,10); //set the camera's initial position
		aCamera.setAspectRatio(1);
		aCamera.setFov(70);
		break;
	case 'F':
	case 'f':
		mouseControllable=!mouseControllable;
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
	}
}
void callbackSpecialFunc(int key, int x, int y){// Called when a "special" non alphanumeric key is pressed
	switch(key){
	case GLUT_KEY_UP:
		key_UP=true;
		break;
	case GLUT_KEY_DOWN:
		key_DOWN=true;
		break;
	}
}
void callbackSpecialUpFunc(int key, int x, int y){// called when a "special" non alphanumeric key is released
	switch(key){
	case GLUT_KEY_UP:
		key_UP=false;
		break;
	case GLUT_KEY_DOWN:
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
	glutInitWindowSize(1600, 800);
	glutCreateWindow("Textures");
	glutFullScreen();

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
	vec3* uv=0;
	//create the points for the plane
	points=new vec3[4];
	points[0]=vec3(-1,1,0);
	points[1]=vec3(1,1,0);
	points[2]=vec3(1,-1,0);
	points[3]=vec3(-1,-1,0);
	//create the normals for the plane
	normals=new vec3[4];
	normals[0]=vec3(0,0,1);
	normals[1]=vec3(0,0,1);
	normals[2]=vec3(0,0,1);
	normals[3]=vec3(0,0,1);
	//create the uvs for the plane
	uv=new vec3[4];
	uv[0]=vec3(0,0,0);
	uv[1]=vec3(1,0,0);
	uv[2]=vec3(1,1,0);
	uv[3]=vec3(0,1,0);
	//create the quads
	quads[0].setProgram(aProgram);
	quads[0].loadPoints(points,normals,uv,4);
	quads[0].setDrawType(GL_QUADS);
	quads[0].setPosition(-1.1,0,0);

	//change the uvs for the second quad
	uv[0]=vec3(0,0,0);
	uv[1]=vec3(2,0,0);
	uv[2]=vec3(2,2,0);
	uv[3]=vec3(0,2,0);

	quads[1]=quads[0];
	quads[1].setPosition(1.1,0,0);
	quads[0].loadPoints(points,normals,uv,4);

	// Load texture into an array
	ILuint id;
	ilGenImages(1, &id);
	ilBindImage(id);
	
	ilLoadImage("resources/texture1.png");

	int width=ilGetInteger(IL_IMAGE_WIDTH);
	int height=ilGetInteger(IL_IMAGE_HEIGHT);

	GLubyte* texture=new GLubyte[width*height*3];

	ilCopyPixels(0, 0, 0, width, height, 1, IL_RGB,IL_UNSIGNED_BYTE, texture);

	// Bind GL Texture
	GLuint  texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	//specify the filters for magnification and minimization  and wrapping.
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	//load the texture into the card and generate mipmaps for it
	glTexImage2D(GL_TEXTURE_2D, 0,3,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,texture);
	glGenerateMipmap(GL_TEXTURE_2D);

	GLuint tex_loc;
	tex_loc = glGetUniformLocation(aProgram, "diffuseMap");

	glUniform1i(tex_loc,0);

	// Cleanup IL
	ilBindImage(id);
	ilDeleteImage(id);

	delete normals;
	delete points; //deletes both uvs and points
	delete texture;
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

	aCamera.setRotation (0,0,0);
	aCamera.setPosition(0,0,10); //set the camera's initial position
	glClearColor( .05, .075, .1, 1 ); // dark background

	//set the mouse to the center of the screen
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
	ilInit();
	initApp();

	initCallbacks();

	//start glut main loop
	glutMainLoop();
	return 0;
}