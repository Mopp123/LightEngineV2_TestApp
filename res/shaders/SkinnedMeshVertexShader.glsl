#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in ivec4 boneIDs;
layout(location = 5) in vec4 boneWeights;

const int maxBones = 50;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 transformationMatrix;
uniform vec3 viewPos;
uniform vec3 directionalLight_direction;

uniform mat4 boneMatrices[maxBones];

out vec2 var_texCoord;
out vec3 var_toCamVec_tangentSpace;
out vec3 var_directionalLight_direction_tangentSpace;

void main(void)
{
	mat4 finalBoneTransform = boneMatrices[boneIDs[0]] * boneWeights[0];
	finalBoneTransform += boneMatrices[boneIDs[1]] * boneWeights[1];
	finalBoneTransform += boneMatrices[boneIDs[2]] * boneWeights[2];
	finalBoneTransform += boneMatrices[boneIDs[3]] * boneWeights[3];

	vec4 vertex_worldSpace = finalBoneTransform * vec4(position, 1.0);
	mat4 cameraSpace = viewMatrix * finalBoneTransform;
	gl_Position = projectionMatrix * cameraSpace * vec4(position, 1.0);

	var_texCoord = texCoord;

	// Create TBN matrix
	vec3 t = normalize(vec3(cameraSpace * vec4(tangent, 0)));
	vec3 n = normalize(vec3(cameraSpace * vec4(normal, 0)));

	vec3 b = normalize(cross(n,t)); // not sure if this is even required to be normalized, since t and n are already normalized..
	
	mat3 TBNMatrix = transpose(mat3(t,b,n));

	vec3 toCam = viewPos - vertex_worldSpace.xyz;
	var_toCamVec_tangentSpace = normalize(TBNMatrix * (viewMatrix * vec4(toCam, 0)).xyz);
	/*
	!!NOTE!!
	Above was changed from this:
		var_toCamVec_tangentSpace = normalize(TBNMatrix * (cameraSpace * vec4(toCam, 0)).xyz);

	Currently appears to look right compared to this old one, but I may have had some logic, why it was differently before..??!
	*/
	var_directionalLight_direction_tangentSpace = TBNMatrix * (viewMatrix * vec4(directionalLight_direction, 0.0)).xyz;
}