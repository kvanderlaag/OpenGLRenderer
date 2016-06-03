#version 430 core

layout(location=0) in vec3 position;
layout(location=1) in vec2 inTex;
layout(location=2) in vec3 norm;

layout(location=0) out vec3 worldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	worldPos = (model * vec4(position, 1.0)).xyz;

	gl_Position = proj * view * vec4(worldPos, 1.0);
}