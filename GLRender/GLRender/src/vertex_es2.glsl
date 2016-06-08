#version 100

in vec3 position;
in vec2 inTexCoords;
in vec3 normal;


out mediump vec2 texcoords;
out mediump vec3 vNormal;

out mediump float lightDistance;
out mediump vec3 lightDirection;
out mediump vec3 eyeDir;
out mediump vec3 reflectDir;
out mediump vec3 worldPos;
out mediump vec3 lightWorldDir;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
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
	

	gl_Position = proj * view * model * vec4(position, 1.0);
	
	texcoords = inTexCoords;
	

}