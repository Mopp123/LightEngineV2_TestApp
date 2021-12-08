#version 330 core

in vec2 var_texCoord;
in vec3 var_toCamVec_tangentSpace;

in vec3 var_directionalLight_direction_tangentSpace;

// Directional light's properties..
struct DirectionalLight
{
	vec3 color;
	vec3 ambient;
};

// Material properties...
struct Material
{
	float specularStrength;
	float specularShininess;
	vec3 specularColor;
};
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;

uniform DirectionalLight directionalLight;
uniform Material material = Material(1.0,32.0,vec3(1.0,1.0,1.0));


out vec4 outColor;


vec3 calcDiffuseColor(vec3 normal, vec3 lightDirection, vec3 lightColor)
{
	float brightness = max(dot(normal, -lightDirection), 0);
	return lightColor * brightness;
}

vec3 calcSpecularColor(vec3 normal, vec3 lightDirection, vec3 lightColor, vec3 toCamVector)
{
	lightDirection = normalize(lightDirection);
	vec3 reflectedLight = normalize(reflect(lightDirection, normal));
	float specular = pow(max(dot(toCamVector, reflectedLight), 0), 16);
	return 1.5f * specular * lightColor;
}

vec3 calcNormal()
{
	return normalize(texture(normalTexture, var_texCoord).rgb * 2.0 - 1.0);
}

void main(void)
{
	vec3 normal = calcNormal();

	vec3 diffuseColor = calcDiffuseColor(normal, var_directionalLight_direction_tangentSpace, directionalLight.color);
	vec3 specularColor = calcSpecularColor(normal, var_directionalLight_direction_tangentSpace, directionalLight.color, var_toCamVec_tangentSpace);

	vec4 diffuseTextureColor = texture(diffuseTexture, var_texCoord);
	vec4 specularTextureColor = texture(specularTexture, var_texCoord);
	
	vec4 finalAmbientColor = vec4(directionalLight.ambient, 1.0) * diffuseTextureColor;
	vec4 finalDiffuseColor = vec4(directionalLight.color, 1.0) * diffuseTextureColor * vec4(diffuseColor, 1.0);
	vec4 finalSpecularColor = vec4(directionalLight.color, 1.0) * specularTextureColor * vec4(specularColor, 1.0);

	outColor = finalAmbientColor + finalDiffuseColor + finalSpecularColor;
}