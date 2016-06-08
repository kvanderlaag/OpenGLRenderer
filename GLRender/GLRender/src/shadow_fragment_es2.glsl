#version 100

in mediump vec3 worldPos;

uniform mediump vec3 cameraPosition;

out mediump float outColor;

void main()
{
    mediump vec3 LightToVertex = worldPos - cameraPosition;

    mediump float LightToPixelDistance = length(LightToVertex);

	if (LightToPixelDistance < 0.f)
		discard;

    outColor = LightToPixelDistance;
}