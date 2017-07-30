//Vertex Shader
const char* VERTEX_SHADER_SOURCE="in vec4 vPosition;\
									uniform mat4 trans_m;\
									void main()\
									{\
									gl_Position = vPosition*trans_m;\
									} ";
//Fragment Shader
const char* FRAGMENT_SHADER_SOURCE="\
								  uniform vec4 color;\
								  void main()\
								  {\
								  gl_FragColor=color;\
								  }";