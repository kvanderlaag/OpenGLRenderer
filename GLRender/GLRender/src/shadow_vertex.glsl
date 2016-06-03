#version 430 core

layout(location=0) in vec3 position;
layout(location=1) in vec2 inTex;
layout(location=2) in vec3 norm;

layout(location=0) out vec3 worldPos;

uniform mat4 smodel;
uniform mat4 sview;
uniform mat4 sproj;

void main() {
	worldPos = (smodel * vec4(position, 1.0)).xyz;

	gl_Position = sproj * sview * smodel * vec4(position, 1.0);
}