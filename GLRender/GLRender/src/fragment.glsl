#version 430 core


layout(location=0) in vec2 texcoords;
layout(location=1) in vec3 vNormal;

layout(location=2) in float lightDistance;
layout(location=3) in vec3 lightDirection;
layout(location=4) in vec3 eyeDir;
layout(location=5) in vec3 reflectDir;
layout(location=6) in vec3 worldPos;
layout(location=7) in vec3 lightWorldDir;

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

float CalcShadowFactor(vec3 LightDirection, float lightDistance1)
{
    float SampledDistance = texture(shadowMap, LightDirection).r;

    float Distance = lightDistance1;

    if (Distance < SampledDistance + 0.005f)
        return 1.0; // Inside the light
    else
        return 0.5; // Inside the shadow
} 

void main()
{

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
	float shadowFactor = CalcShadowFactor(lightWorldDir, distance(worldPos, lightPosition));

    if (blend == 0) {
		outColor = clamp(ambient + shadowFactor*(diffuse + specular), 0.0, 1.0);
	} else if (blend == 1) {
		outColor = vec4( 0.5 * vNormal.xyz + vec3(0.5, 0.5, 0.5), 1);
	} else {
		float infinity = 1.0 / 0.0;
		float val = texture(shadowMap, lightWorldDir).r / 50;
		outColor = vec4(val, val, val, 1);
	}
	
}