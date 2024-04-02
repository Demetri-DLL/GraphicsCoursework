#version 400

uniform mat4 modelViewMatrix;
//uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColour;

out vec3 vColour;

void main()
{
	gl_Position = projectionMatrix * modelViewMatrix * vec4(inPosition, 1);
	vColour = inColour;
}