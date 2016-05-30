#version 430 core

in vec3 position;
in vec3 normal;
in vec2 inTexCoords;

out vec2 texcoords;
out vec4 vNormal;
out float lightDistance;
out vec3 lightDirection;
out vec3 eyeDir;
out vec3 reflectDir;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;

void main() {

	vec4 modelPos = model * vec4(position, 1.0);
	vNormal = model * vec4(normal, 1);

	lightDistance = distance(modelPos.xyz, lightPosition);
	lightDirection = normalize(modelPos.xyz - lightPosition);

	eyeDir = normalize(cameraPosition - modelPos.xyz);
	reflectDir = normalize(reflect(lightDirection,vNormal.xyz));   

	gl_Position = proj * view * model * vec4(position, 1.0);
	
	texcoords = inTexCoords;
	

}