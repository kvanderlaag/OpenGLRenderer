#version 430 core


layout(location=0) in vec2 texcoords;
layout(location=1) in vec3 vNormal;

layout(location=2) in float lightDistance;
layout(location=3) in vec3 lightDirection;
layout(location=4) in vec3 eyeDir;
layout(location=5) in vec3 reflectDir;
layout(location=6) in vec3 worldPos;

out vec4 outColor;

uniform sampler2D mtexture;
uniform samplerCube shadowMap;


uniform vec3 lightPosition;
uniform float lightIntensity;
uniform vec4 lightColor;

uniform int blend;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float ns;
uniform float d;
uniform int numLights;

float CalcShadowFactor(vec3 LightDirection, float lightDistance)
{
    float SampledDistance = texture(shadowMap, LightDirection).r;

    float Distance = lightDistance;

    if (Distance < SampledDistance + 0.0001f)
        return 1.0; // Inside the light
    else
        return 0.5; // Inside the shadow
} 

void main()
{
	int lights = 0;
	if (numLights < 0) {
		lights = 0;
	}
	else if (numLights > 4) {
		lights = 4;
	} else {
		lights = numLights;
	}

	float SpecularIntensity = 1.0f;
	float Shininess = ns;

	vec4 MaterialSpecularColor = vec4(ks, 1);
	vec4 MaterialDiffuseColor = vec4(kd, 1);
	vec4 MaterialAmbientColor = vec4(ka, 1);

	
	vec4 textureColor = texture(mtexture, texcoords);

	vec4 ambient = MaterialAmbientColor * textureColor;

	float cosAlpha = clamp(dot( eyeDir, reflectDir ), 0.0, 1.0);
	float lightRatio = dot(normalize(lightDirection), normalize(vNormal.xyz));
	vec4 diffuse = lightRatio * textureColor * MaterialDiffuseColor * lightColor * lightIntensity / (lightDistance * lightDistance);
	vec4 specular = SpecularIntensity * MaterialSpecularColor * lightColor * lightIntensity * pow(cosAlpha,Shininess) / (lightDistance * lightDistance);
	
	vec3 lightWorldDir = worldPos - lightPosition;
	float shadowFactor = CalcShadowFactor(lightWorldDir, lightDistance);

    if (blend == 0) {
		outColor = clamp(ambient + (diffuse + specular) * shadowFactor, 0.0, 1.0);
	} else if (blend == 1) {
		outColor = vec4( 0.5 * vNormal.xyz + vec3(0.5, 0.5, 0.5), 1);
	} else {
		float val = CalcShadowFactor(worldPos - lightPosition, lightDistance);
		outColor = vec4(val, val, val, 1);
	}
	
}