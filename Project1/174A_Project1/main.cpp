#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <random>

#include "windows\glew\include\GL\glew.h"
#if defined(__APPLE__)
#include <GLUT/glut.h>
#elif defined(_WIN32)
#include "windows/glut/glut.h"
#else
#include <GL/glut.h>
#endif

#define BUFFER_OFFSET( bytes ) ((GLvoid *)(bytes))

using namespace std;

/////////////
/* Classes */
/////////////
struct vec3{ //Struct for a 3D Vector
	vec3(GLfloat newX,GLfloat newY,GLfloat newZ):x(newX),y(newY),z(newZ){}
	vec3():x(0),y(0),z(0){}
	GLfloat x;
	GLfloat y;
	GLfloat z;

	vec3 operator+(const vec3& rhs){
		GLfloat newX=x+rhs.x;
		GLfloat newY=y+rhs.y;
		GLfloat newZ=z+rhs.z;
		return vec3(newX,newY,newZ);
	}
	vec3 operator-(const vec3& rhs){
		GLfloat newX=x-rhs.x;
		GLfloat newY=y-rhs.y;
		GLfloat newZ=z-rhs.z;
		return vec3(newX,newY,newZ);
	}
	vec3 operator/(const float& rhs){
		GLfloat newX=x/rhs;
		GLfloat newY=y/rhs;
		GLfloat newZ=z/rhs;
		return vec3(newX,newY,newZ);
	}
	vec3 operator*(const float& rhs){
		GLfloat newX=x*rhs;
		GLfloat newY=y*rhs;
		GLfloat newZ=z*rhs;
		return vec3(newX,newY,newZ);
	}
};
class DrawableObject{ //Struct for a drawable object to keep track of Vertex Array and Buffer pointers
private:
	GLenum type;
	GLuint program;
	GLuint vArray;
	GLuint buffer;
	GLuint num_points;

	bool initialized;
	bool has_points;
	bool has_program;

	void checkInitialized(){
		if(has_points&&has_program){
			initialized=true;
		}
	}
public:
	DrawableObject():type(GL_POINTS), initialized(false), has_points(false), has_program(false){};
	void setDrawType(GLenum mode){ //Sets the draw mode for the object - GL_LINES, GL_TRIANGLES, GL_POINTS
		this->type=mode;
	}
	void setProgram(GLuint newProgram){ //Sets the shader program that the object uses to render
		this->program=newProgram;
		this->has_program=true;
		checkInitialized();
	}
	bool loadPoints(vec3*& points, int num){ //Loads the passed points in the GPU memory and stores the name of the buffer and vertex array
		glGenVertexArrays(1, &(this->vArray));
		glGenBuffers(1,&(this->buffer));
		glBindVertexArray(this->vArray);
		glBindBuffer( GL_ARRAY_BUFFER, this->buffer);

		glBufferData( GL_ARRAY_BUFFER, num*sizeof( vec3 ), points, GL_STATIC_DRAW );
		glEnableVertexAttribArray(0);

		this->num_points=num;

		this->has_points=true;
		checkInitialized();
		return true; //returns success
	}
	bool draw(){ //draws the points to the screen
		if(!initialized)return false; //returns failure if the object has not been properly set up
		glUseProgram(this->program);
		glBindVertexArray(this->vArray);
		glDrawArrays( this->type, 0, num_points );
		return true;
	}
	bool bindVertexShaderVariable(char* name, GLuint size=3, GLuint offset=0){ //binds a vertex shader variable that is used in the shader program passed to the object
		if(!initialized)return false; //returns failure if the object has not been properly set up
		GLuint loc = glGetAttribLocation( this->program, name );
		glEnableVertexAttribArray( loc );
		glVertexAttribPointer( loc,size, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(offset) );
		return true;
	}
};
///////////////////////////
/* Application variables */
///////////////////////////
const double PI=3.1415926535;
GLuint fractalProgram; //Name of the shader program that is used in this application
GLuint color_loc; //name of the variable in fractalProgram used to change the color

GLuint rotation_loc; 
float rotation_angle=0;
float ROTATION_DELTA=2*PI/30;

const int MAX_PAGES=3;
int page=0;

DrawableObject drawableElem[MAX_PAGES];

const int SIERPINSKI_SIZE=500000;
const int BRANCH_SIZE=100000;

////////////////////////
/* Fractal Generation */
////////////////////////
/* Generate Sierpinski Points */
int genSierpinskiPoints(vec3*& points,vec3 v0, vec3 v1, vec3 v2){
	//generates the points and stores them in "points" with v0, v1, and v2 as the seeds for the algorithm
	const int size=SIERPINSKI_SIZE;
	points=new vec3[size];
	points[0]=v0;

	vec3 verts[3];
	verts[0]=v0;
	verts[1]=v1;
	verts[2]=v2;

	for(int i=1;i<size;i++){
		vec3 *chosen=&verts[rand()%3];
		points[i]=*chosen-(*chosen-points[i-1])/2;
	}
	return size; //returns the size of the resulting array
}
////////////////////////////////////
/* Generate Branch Fractal Points */
////////////////////////////////////
int auxGenBranchPoints(vec3* points, const vec3& root, float angle, int depth, int splits, float length){
	//recursively generates the points in the "points" array for each line until the initial "depth" value is met.
	if(depth==0){return 0;}

	points[0]=root;
	points[1]=vec3(root.x+length*cos(angle),root.y+length*sin(angle),0);

	int iterated=2;
	float angleDelta=PI/(splits+1);

	for(int i=0;i<splits;i++){
		iterated+=auxGenBranchPoints(points+iterated,points[1],angle-PI/2+angleDelta*(i+1),depth-1,splits, length*.5);
	}

	return iterated; //returns the relevant size of the array by adding up how many points were created in all iterations
}

int genBranchPoints(vec3*& points, int splits=2, int depth=5, vec3 root=vec3(0,-.5,0), float scale=1){
	//initial call to allocate memory and then begin the auxillary function
	points=new vec3[BRANCH_SIZE];
	return auxGenBranchPoints(points+2,root,PI/2,depth-1,splits, scale*.5);
}

////////////////////////////////
/* Generate Snowflake Fractal */
////////////////////////////////
int auxGenTriFractal(vec3* points, int depth, float angle, vec3 root, float sz){
	//recursively generate tris in the "points" array for each corner, the left, right, and bottom side
	if(depth==0){return 0;}
	
	int iterated=3;

	float height=sqrt(sz*sz-sz*.5*sz*.5);

	points[0]=root+vec3(height*cos(angle),height*sin(angle),0); //top vert
	points[1]=root+vec3(sz*-.5*cos(angle-PI/2),sz*-.5*sin(angle-PI/2),0); //left vert
	points[2]=root+vec3(sz*.5*cos(angle-PI/2),sz*.5*sin(angle-PI/2),0); //right vert

	vec3 line=(points[0]-points[1]); //left side
	float lAngle=atan2(line.y,line.x)+PI/2; //angle pointing off to the left
	line=(points[0]-points[2]); //right side
	float rAngle=atan2(line.y,line.x)-PI/2; //angle pointing off to the right

	iterated+=auxGenTriFractal(points+iterated,depth-1,lAngle,points[0]-(points[0]-points[1])/2,sz/3); //Left Iteration
	iterated+=auxGenTriFractal(points+iterated,depth-1,rAngle,points[0]-(points[0]-points[2])/2,sz/3); //Right Iteration
	iterated+=auxGenTriFractal(points+iterated,depth-1,angle+PI,root,sz/3); //Bottom Iteration

	iterated+=auxGenTriFractal(points+iterated,depth-1,angle,points[0]-vec3(cos(angle),sin(angle),0)*.29*sz,sz/3); //Top Iteration
	iterated+=auxGenTriFractal(points+iterated,depth-1,angle+2*PI/3,points[1]+vec3(cos(rAngle),sin(rAngle),0)*.29*sz,sz/3); //Bottom Left Iteration
	iterated+=auxGenTriFractal(points+iterated,depth-1,angle-2*PI/3,points[2]+vec3(cos(lAngle),sin(lAngle),0)*.29*sz,sz/3); //Bottom Right Iteration

	return iterated; //returns the relevant size of the array by adding up how many points were created in all recursions
}
int genTriFractal(vec3*& points, int depth=6, vec3 root=vec3(0,-.3,0), float sz=1){
	//initial call to allocated memory and then begin auxillary function
	points = new vec3[BRANCH_SIZE];
	return auxGenTriFractal(points, depth, PI/2, root, sz);
}
///////////////////////////
/* Application Functions */
///////////////////////////
// change the "page" that is being displayed by adding "num" to "page"(changes the fractal)
void addToPage(int num){
	page+=num;
	if(page>=MAX_PAGES){
		page=0;
	}else if(page<0){
		page=MAX_PAGES-1;
	}
	glutPostRedisplay();
}
void incPage(){ //increment the "page" by one
	addToPage(1);
}
void decPage(){ //decrement the "page" by one
	addToPage(-1);
}

// updates the color variable of the program being used in this application
void updateColor(float r, float g, float b){
	glUniform4f(color_loc,(GLfloat)r,(GLfloat)g,(GLfloat)b,1.0);
	glutPostRedisplay();
}
// Sets the rotation angle for and updates the uniform variable in the shader
void setRotation(float d){
	rotation_angle=d;

	if(rotation_angle>2*PI){
		rotation_angle-=2*PI;
	}else if(rotation_angle<0){
		rotation_angle+=2*PI;
	}

	GLfloat m[16]={ //calculates the rotation matrix
		cos(rotation_angle),-sin(rotation_angle),0,0,
		sin(rotation_angle),cos(rotation_angle),0,0,
		0,0,1,0,
		0,0,0,1
	};
	glUniformMatrix4fv(rotation_loc,1,GL_FALSE,m);

	glutPostRedisplay();
}
// Adds 'd' to the current rotated angle and sets the angle
void addToRotation(float d){
	rotation_angle+=d;
	setRotation(rotation_angle);
}

///////////////////////////////
/* GLUT Callback Definitions */
///////////////////////////////
// Called when the window needs to be redrawn.
void callbackDisplay()
{
	//draw to the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear the draw buffer so that no ghosting occurs
	drawableElem[page].draw(); //draw the element in the array based on what page is active
	glutSwapBuffers();
}
// Called when the window is resized.
void callbackReshape(int width, int height){}
// Called when a key is pressed. x, y is the current mouse position.
void callbackKeyboard(unsigned char key, int x, int y)
{	
	if (key == 27||key=='q'||key=='Q'){ // Quit when esc, Q, or q are pressed
		exit(0);
	}
	else if(key==' '){ // change which fractal is drawn when space is pressed
		incPage();
	}else if(key=='a'||key=='A'){ //increase rotation if 'a' or 'A' is pressed
		addToRotation(ROTATION_DELTA);
	}else if(key=='d'||key=='D'){ //decrease rotation if 'd' or 'D' is pressed
		addToRotation(-ROTATION_DELTA);
	}else if(key=='s'||key=='S'){ // reset rotation if 's' or 'S' is pressed
		setRotation(0);
	}else if(isdigit(key)&&key!='0'){ //change the color based on which number key is pressed
		int digit=key-'0';
		float r=1;
		float g=(float)(digit)/9;
		float b=.5-(float)(digit )/9;
		
		updateColor(r,g,b);
	}
}
void callbackKeyboardUp(unsigned char key, int x, int y){}// Called when a key is released. x, y is the current mouse position.
void callbackMouse(int button, int state, int x, int y){}// Called when a mouse button is pressed or released
void callbackMotion(int x, int y){// Called when the mouse is moved with a button pressed
	//Rotates the space based off of the mouse position
	double xO=(float)y-glutGet(GLUT_WINDOW_HEIGHT)/2;
	double yO=(float)x-glutGet(GLUT_WINDOW_WIDTH)/2;

	float a=-atan2(yO,xO)-PI;
	
	setRotation(a);
}
void callbackPassiveMotion(int x, int y){}// Called when the mouse is moved with no buttons pressed
void callbackIdle(){}// Called when the system is idle. Can be called many times per frame.
void callbackTimer(int) // Called when the timer expires
{
	//glutTimerFunc(1000/30, callbackTimer, 0);
	//glutPostRedisplay();
}

////////////////////////////////
/* Application Init Functions */
////////////////////////////////
//generates the verts and creates the drawable objects
void createVertexArrays(){
	vec3* vecs;
	int sz=genSierpinskiPoints(vecs,vec3(-0.5,-0.5,0), vec3(0.5,-0.5,0), vec3(0,0.5,0));
	
	//Create Sierpinski DrawableObject
	drawableElem[0].setDrawType(GL_POINTS);
	drawableElem[0].loadPoints(vecs,sz);
	drawableElem[0].setProgram(fractalProgram);
	drawableElem[0].bindVertexShaderVariable("vPosition");
	delete[] vecs;

	//Create Branch Algorithm DrawableObject
	sz=genBranchPoints(vecs,4,8,vec3(0,-.5,0),1);
	drawableElem[1].setDrawType(GL_LINES);
	drawableElem[1].loadPoints(vecs,sz);
	drawableElem[1].setProgram(fractalProgram);
	drawableElem[1].bindVertexShaderVariable("vPosition");
	delete[] vecs;
	
	//Create Snowflake Algorithm DrawableObject
	sz=genTriFractal(vecs);
	drawableElem[2].setDrawType(GL_TRIANGLES);
	drawableElem[2].loadPoints(vecs,sz);
	drawableElem[2].setProgram(fractalProgram);
	drawableElem[2].bindVertexShaderVariable("vPosition");
	delete [] vecs;
}

//creates the color variable for the passed program so that the color can be changed in this application
GLuint initShaderColorVariable(GLuint p){
	GLuint loc = glGetUniformLocation(p, "color");
	if(loc!=-1){
		glUniform4f(loc,1.0,1.0,1.0,1.0);
	}
	return loc; //returns the location of the color variable
}
//creates the rotation_m variable for the passed program so that the rotation can be changed in the app
GLuint initShaderRotationVariable(GLuint p){
	GLuint loc = glGetUniformLocation(p, "rotation_m");
	if(loc!=-1){
		GLfloat m[16]={ //identity matrix
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		};
		glUniformMatrix4fv(loc, 1, GL_FALSE, m);
	}
	return loc; //returns the location of the rotation variable
}

//returns a program with the passed shader code as the shader functions
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
		cout<<"Error compiling Vertex Shader";
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
		cout<<"Error compiling Fragment Shader";
		exit(EXIT_FAILURE);
	}
	glAttachShader( program, fShader );

	/* Link the new program */
	glLinkProgram(program);
	GLint linked;
	glGetProgramiv( program, GL_LINK_STATUS, &linked ); //verify that program was linked successfully
	if ( !linked ) {
		cout<<"Error linking program";
		exit(EXIT_FAILURE);
	}
	glUseProgram(program); //use the program that was just created when rendering

	return program; //returns the program with the passed shaders
}

//initialize the shaders and create the vertices that will be used
void appInit(){
	/* Vertex Shader Source */
	char* vShaderSource="in vec4 vPosition;\
		uniform mat4 rotation_m;\
		void main()\
		{\
			gl_Position = rotation_m*vPosition;\
		} ";
	/* Fragment Shader Source */
	char* fShaderSource="uniform vec4 color;\
		void main()\
		{\
			gl_FragColor = color;\
		}";
	//get shader program and variable locations
	fractalProgram=initShader(vShaderSource, fShaderSource);
	color_loc=initShaderColorVariable(fractalProgram);
	rotation_loc=initShaderRotationVariable(fractalProgram);

	updateColor(1.0, 4/9.0, .5-4/9.0);
	setRotation(0);
	createVertexArrays();
	glClearColor( .1, .1, .1, .1 ); // dark background
}
//initialize the window
void initGlut(int& argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 50);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Fractals");
}

//bind callback functions to glut
void initCallbacks()
{
	glutDisplayFunc(callbackDisplay);
	glutReshapeFunc(callbackReshape);
	glutKeyboardFunc(callbackKeyboard);
	glutKeyboardUpFunc(callbackKeyboardUp);
	glutMouseFunc(callbackMouse);
	glutMotionFunc(callbackMotion);
	glutPassiveMotionFunc(callbackPassiveMotion);
	glutIdleFunc(callbackIdle);
	glutTimerFunc(1000/30, callbackTimer, 0);
}

//////////
/* Main */
//////////
int main(int argc, char** argv)
{
	//initialize glut and glew
	initGlut(argc, argv);
	glewInit();
	initCallbacks();
	//create buffers and shaders
	appInit();
	//start glut main loop
	glutMainLoop();
	return 0;
}