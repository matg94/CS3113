
uniform sampler2D diffuse;
uniform vec2 LightPositions[6];

varying vec4 vertexColor;
varying vec2 texCoordVar;
varying vec2 varPosition;

float attenuate(float dist, float a, float b) {
	return 0.2 / (1.0 + a*dist + b*dist*dist);	
}

void main() {	

	float brightness = 0.0;

	for (int i = 0; i < 6; i++) {
		brightness += attenuate(distance(LightPositions[i], varPosition), 0.15, 5.0);
	}

	vec4 temp = texture2D(diffuse, texCoordVar)*vertexColor;
	gl_FragColor = temp*brightness;
	gl_FragColor.a = temp.a;

}	  