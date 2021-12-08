#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

layout(location = 3) in mat4 transformationMatrix;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec3 viewPos;

out vec2 var_texCoord;
out vec3 var_normal;

void main(void)
{
	gl_Position = projectionMatrix * viewMatrix * transformationMatrix * vec4(position, 1.0);
	var_texCoord = texCoord;
	var_normal = (transformationMatrix * vec4(normal, 0)).xyz;
}