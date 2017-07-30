//Vertex Shader Source Code
const char* VERTEX_SHADER_SOURCE="in vec4 vPosition;\
									in vec4 vNormal;\
									in vec4 vUV;\
									\
									uniform mat4 model_trans;\
									uniform mat4 camera_trans;\
									\
									varying vec4 fNormal;\
									varying vec4 fPos;\
									varying vec2 fUV;\
									\
									void main()\
									{\
									vPosition=vPosition*model_trans;\
									fNormal=vNormal;\
									fNormal.w=0;\
									fPos=vPosition;\
									fPos.w=0;\
									fUV=vUV;\
									gl_Position = vPosition*camera_trans;\
									} ";
//Fragment Shader Source Code
const char* FRAGMENT_SHADER_SOURCE="uniform sampler2D diffuseMap;\
								   \
								  uniform vec4 diffuseColor;\
								  uniform vec4 lightColor;\
								  uniform vec4 ambientColor;\
								  uniform vec4 lightPos;\
								  uniform vec4 camPos;\
								  uniform float shininess;\
								  \
								  varying vec4 fNormal;\
								  varying vec4 fPos;\
								  varying vec2 fUV;\
								  \
								  \
								  void main()\
								  {\
								  diffuseColor=texture2D(diffuseMap,fUV);\
								  lightPos.w=0;\
								  camPos.w=0;\
								  vec4 lightVector=normalize(lightPos-fPos);\
								  vec4 viewDir=normalize(camPos-fPos);\
								  \
								  vec4 amb=ambientColor;\
								  vec4 difMult=clamp(dot(fNormal,lightVector),vec4(0,0,0,0),vec4(1,1,1,1));\
								  vec4 specMult=pow(clamp(dot(fNormal,normalize(viewDir+lightVector)),vec4(0,0,0,0),vec4(1,1,1,1)),shininess);\
								  if(shininess==0||dot(fNormal,lightVector)<0){specMult=0;}\
								  \
								  gl_FragColor=amb+difMult*diffuseColor*lightColor+specMult*difMult*lightColor;\
								  }";


//TODO: create a gourand shader