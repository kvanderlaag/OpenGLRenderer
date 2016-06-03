#version 430 core

in vec3 position;

out vec3 worldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 cameraPosition;

void main() {

	worldPos = (model * vec4(position, 1.0)).xyz;

	gl_Position = proj * view * model * vec4(position, 1.0);
}