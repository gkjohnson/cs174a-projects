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
									fNormal=fNormal*model_trans;\
									fNormal=normalize(fNormal);\
									\
									fPos=vPosition;\
									fPos.w=0;\
									fUV=vUV;\
									gl_Position = vPosition*camera_trans;\
									} ";
//Fragment Shader Source Code
const char* FRAGMENT_SHADER_SOURCE="\
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
								  vec4 lightVector=normalize(lightPos-fPos);\
								  vec4 viewDir=normalize(camPos-fPos);\
								  \
								  vec4 amb=ambientColor;\
								  float difMult=clamp(dot(fNormal,lightVector),0,1);\
								  float specMult=pow(clamp(dot(fNormal,normalize(viewDir+lightVector)),0,1),shininess);\
								  if(shininess==0||dot(fNormal,lightVector)<0){specMult=0.0;}\
								  \
								  gl_FragColor=amb+difMult*diffuseColor*lightColor+specMult*difMult*lightColor;\
								  } \n";


//TODO: create a gourand shader