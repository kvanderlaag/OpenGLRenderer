#version 100


in mediump vec2 texcoords;
in mediump vec3 vNormal;

in mediump float lightDistance;
in mediump vec3 lightDirection;
in mediump vec3 eyeDir;
in mediump vec3 reflectDir;
in mediump vec3 worldPos;
in mediump vec3 lightWorldDir;

out vec4 outColor;

uniform mediump sampler2D mtexture;
uniform mediump samplerCube shadowMap;


uniform mediump vec3 lightPosition;
uniform mediump float lightIntensity;
uniform mediump vec4 lightColor;

uniform int blend;

uniform mediump vec3 ka;
uniform mediump vec3 kd;
uniform mediump vec3 ks;
uniform mediump float ns;
uniform mediump float d;
uniform int numLights;

mediump float CalcShadowFactor(vec3 LightDirection, float lightDistance1)
{
    mediump float SampledDistance = textureCube(shadowMap, LightDirection).x;

    mediump float Distance = lightDistance1;

    if (Distance < SampledDistance + 0.005f)
        return 1.0; // Inside the light
    else
        return 0.5; // Inside the shadow
} 

void main()
{

	mediump float SpecularIntensity = 1.0f;
	mediump float Shininess = ns;

	mediump vec4 MaterialSpecularColor = vec4(ks, 1);
	mediump vec4 MaterialDiffuseColor = vec4(kd, 1);
	mediump vec4 MaterialAmbientColor = vec4(ka, 1);

	
	mediump vec4 textureColor = texture2D(mtexture, texcoords);

	mediump vec4 ambient = MaterialAmbientColor * textureColor;

	mediump float cosAlpha = clamp(dot( eyeDir, reflectDir ), 0.0, 1.0);
	mediump float lightRatio = dot(normalize(lightDirection), normalize(vNormal.xyz));
	mediump vec4 diffuse = lightRatio * textureColor * MaterialDiffuseColor * lightColor * lightIntensity / (lightDistance * lightDistance);
	mediump vec4 specular = SpecularIntensity * MaterialSpecularColor * lightColor * lightIntensity * pow(cosAlpha,Shininess) / (lightDistance * lightDistance);
	
	mediump vec3 lightWorldDir = worldPos - lightPosition;
	mediump float shadowFactor = CalcShadowFactor(lightWorldDir, distance(worldPos, lightPosition));

    if (blend == 0) {
		outColor = clamp(ambient + shadowFactor*(diffuse + specular), 0.0, 1.0);
	} else if (blend == 1) {
		outColor = mediump vec4( 0.5 * vNormal.xyz + vec3(0.5, 0.5, 0.5), 1);
	} else {
		mediump float val = textureCube(shadowMap, lightWorldDir).x / 50.f;
		outColor = mediump vec4(val, val, val, 1);
	}
	
}