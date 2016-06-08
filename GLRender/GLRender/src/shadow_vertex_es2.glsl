#version 100

in vec3 position;
in vec2 inTex;
in vec3 norm;

out vec3 worldPos;

uniform mat4 smodel;
uniform mat4 sview;
uniform mat4 sproj;

void main() {
	worldPos = (smodel * vec4(position, 1.0)).xyz;

	gl_Position = sproj * sview * smodel * vec4(position, 1.0);
}