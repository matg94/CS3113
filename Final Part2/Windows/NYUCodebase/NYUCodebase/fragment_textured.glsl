
uniform sampler2D diffuse;
varying vec4 vertexColor;
varying vec2 texCoordVar;

void main() {	
	vec4 temp = texture2D(diffuse, texCoordVar)*vertexColor;
	gl_FragColor = temp;
}	  