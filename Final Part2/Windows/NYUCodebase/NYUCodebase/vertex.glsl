attribute vec4 color;
attribute vec4 position;
attribute vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

varying vec4 vertexColor;
varying vec2 texCoordVar;

void main()
{
	vec4 p = viewMatrix * modelMatrix  * position;
	vertexColor = color;
	texCoordVar = texCoord;
	gl_Position = projectionMatrix * p;
}