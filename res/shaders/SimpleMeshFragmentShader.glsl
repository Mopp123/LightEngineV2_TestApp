#version 330 core

in vec2 var_texCoord;
in vec3 var_normal;

// Directional light's properties..
struct DirectionalLight
{
	vec3 color;
	vec3 ambient;
};
uniform vec3 directionalLight_direction;

uniform sampler2D diffuseTexture;
uniform DirectionalLight directionalLight;

out vec4 outColor;


vec3 calcDiffuseColor(vec3 normal, vec3 lightDirection, vec3 lightColor)
{
	normal = normalize(normal);
	lightDirection = normalize(lightDirection);
	float brightness = max(dot(normal, -lightDirection), 0);
	return lightColor * brightness;
}

void main(void)
{
	
	vec3 diffuseColor = calcDiffuseColor(var_normal, directionalLight_direction, directionalLight.color);
	
	vec4 diffuseTextureColor = texture(diffuseTexture, var_texCoord);
	
	vec4 finalAmbientColor = vec4(directionalLight.ambient, 1.0) * diffuseTextureColor;
	vec4 finalDiffuseColor = vec4(directionalLight.color, 1.0) * diffuseTextureColor * vec4(diffuseColor, 1.0);
	
	outColor = finalAmbientColor + finalDiffuseColor;
}