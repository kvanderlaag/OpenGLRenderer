#version 430 core

layout(location=0) in vec3 position;
layout(location=1) in vec2 inTexCoords;
layout(location=2) in vec3 normal;


layout(location=0) out vec2 texcoords;
layout(location=1) out vec3 vNormal;

layout(location=2) out float lightDistance;
layout(location=3) out vec3 lightDirection;
layout(location=4) out vec3 eyeDir;
layout(location=5) out vec3 reflectDir;
layout(location=6) out vec3 worldPos;
layout(location=7) out vec3 lightWorldDir;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 worldViewProj;
uniform mat4 normalMatrix;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;

void main() {
	vec4 modelPos = model * vec4(position, 1.0);
	vNormal = normalize(normalMatrix * vec4(normal, 0)).xyz;

	eyeDir = normalize(cameraPosition - modelPos.xyz);

	worldPos = modelPos.xyz;

	lightWorldDir = worldPos - vec4(lightPosition, 1).xyz;

	lightDistance = distance(modelPos.xyz, lightPosition);
	lightDirection = normalize(modelPos.xyz - lightPosition);

		
	reflectDir = normalize(reflect(lightDirection,vNormal));   
	

	gl_Position = worldViewProj * vec4(position, 1.0);
	
	texcoords = inTexCoords;
	

}