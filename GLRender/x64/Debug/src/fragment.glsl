#version 150

in vec3 ocolor;
out vec4 outColor;

void main()
{
    outColor = vec4(1.0 - ocolor.r, 1.0 - ocolor.g, 1.0 - ocolor.b, 1.0);
}