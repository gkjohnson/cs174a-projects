//Vertex Shader Source Code
//Phong and Gouraud are implemented within the same shader and toggled using a uniform variable
const char* VERTEX_SHADER_SOURCE="\
									in vec4 vPosition;\
									in vec4 vNormal;\
									uniform mat4 model_trans;\
									uniform mat4 camera_trans;\
									uniform bool gouraud;\
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
									varying vec4 fColor;\
									\
									void main()\
									{\
									vPosition=vPosition*model_trans;\
									vNormal.w=0;\
									vNormal=normalize(vNormal*model_trans);\
									gl_Position = vPosition*camera_trans;\
									\
									fNormal=vNormal;\
									fPos=vPosition;\
									fPos.w=0;\
									if(gouraud){\
										\
									    vec4 lp = lightPos;\
                                        lp.w = 0;\
                                        vec4 cp = camPos;\
                                        cp.w = 0;\
										vec4 lightVector=normalize(lp-fPos);\
										vec4 viewDir=normalize(cp-fPos);\
										\
										vec4 amb=ambientColor;\
										float difMult=clamp(dot(vNormal,lightVector),0,1);\
										float specMult=pow(clamp(dot(vNormal,normalize(viewDir+lightVector)),0,1),shininess);\
										if(shininess==0||dot(vNormal,lightVector)<0){specMult=0;}\
										\
										fColor=amb+difMult*diffuseColor*lightColor+specMult*difMult*lightColor;\
									}else{\
									}\
									}";
//Fragment Shader Source Code
const char* FRAGMENT_SHADER_SOURCE = "\
								  uniform vec4 diffuseColor;\
								  uniform vec4 lightColor;\
								  uniform vec4 ambientColor;\
								  uniform vec4 lightPos;\
								  uniform vec4 camPos;\
								  uniform float shininess;\
								  uniform bool gouraud;\
								  uniform mat4 camera_trans;\
								  \
								  varying vec4 fNormal;\
								  varying vec4 fPos;\
								  varying vec4 fColor;\
								  \
								  void main()\
								  {\
								  if(gouraud){\
								  gl_FragColor=fColor;\
								  }else{\
									  vec4 lp = lightPos;\
                                      lp.w = 0;\
                                      vec4 cp = camPos;\
                                      cp.w = 0;\
									  vec4 lightVector=normalize(lp-fPos);\
									  vec4 viewDir=normalize(cp-fPos);\
									  \
									  vec4 amb=ambientColor;\
									  float difMult=clamp(dot(fNormal,lightVector),0,1);\
									  float specMult=pow(clamp(dot(fNormal,normalize(viewDir+lightVector)),0,1),shininess);\
									  if(shininess==0||dot(fNormal,lightVector)<0){specMult=0;}\
									  \
									 gl_FragColor=amb+difMult*diffuseColor*lightColor+specMult*difMult*lightColor;\
								  }\
								  }";
