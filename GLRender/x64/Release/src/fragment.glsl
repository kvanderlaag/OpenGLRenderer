#version 430 core

in vec2 texcoords;

in float lightDistance;
in vec4 vNormal;
in vec3 lightDirection;
in vec3 eyeDir;
in vec3 reflectDir;

out vec4 outColor;

uniform sampler2D texture;

uniform float lightIntensity;

uniform float blend;

void main()
{
	float cosAlpha = clamp(dot( eyeDir, reflectDir ), 0.0, 1.0);

	float SpecularIntensity = 1.0f;
	float Shininess = 200.0f;
	
	vec4 MaterialSpecularColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vec4 LightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	vec4 textureColor = texture(texture, texcoords);

	float lightRatio = dot(lightDirection, vNormal.xyz);
	vec4 diffuse = lightRatio * textureColor * LightColor * lightIntensity / (lightDistance * lightDistance);
	vec4 specular = SpecularIntensity * MaterialSpecularColor * LightColor * lightIntensity * pow(cosAlpha,Shininess) / (lightDistance * lightDistance);

	vec4 ambient = vec4(0.05, 0.05, 0.05, 1.0);
	
	
    outColor = clamp(diffuse + ambient + specular, 0.0, 1.0);
}