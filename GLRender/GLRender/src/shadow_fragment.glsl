#version 430 core

layout(location=0) in vec3 worldPos;

uniform vec3 cameraPosition;

out float outColor;

void main()
{
    vec3 LightToVertex = worldPos - cameraPosition;

    float LightToPixelDistance = length(LightToVertex);

	if (LightToPixelDistance < 0)
		discard;

    outColor = LightToPixelDistance;
}