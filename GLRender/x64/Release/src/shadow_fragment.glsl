#version 430 core

in vec3 worldPos;

uniform vec3 cameraPosition;

out float FragColor;

void main()
{
    vec3 LightToVertex = worldPos - cameraPosition;

    float LightToPixelDistance = length(LightToVertex);

    FragColor = LightToPixelDistance;
}