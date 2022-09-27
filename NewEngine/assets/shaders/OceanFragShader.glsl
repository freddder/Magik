#version 330 core

in vec4 fUVx2;
in vec3 fNormal;
in vec4 fVertPosLightSpace;
in vec4 fVertWorldPosition;

struct sLight
{
	vec4 position;			
	vec4 diffuse;	
	//vec4 specular;	// rgb = highlight colour, w = power
	//vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;		// Spot or directional lights
	vec4 extraParam;	// x = lightType, y = inner angle, z = outer angle, w = 0 for off, 1 for on
							// 0 = pointlight
							// 1 = spot light
							// 2 = directional light
};

layout (std140) uniform Fog
{
	vec4 fogViewOrigin;
	vec4 fogColor;
	float fogDensity;
	float fogGradient;
};

layout (std140) uniform Lights
{
    sLight theLights[20];
};

// texture samplers
uniform sampler2D texture_0;
uniform sampler2D shadowMap;

//uniform vec2 globalUVRatios;
uniform vec2 UVoffset;

uniform bool useWholeColor;
uniform vec4 wholeColor;

float ShadowCalculation(vec4 fragPosLightSpace);

void main()
{

	vec4 vertColor;

	if(useWholeColor)
	{
		vertColor = wholeColor;
	}
	else
	{
		vec2 newUV1 = vec2(fUVx2.x + UVoffset.x, fUVx2.y + UVoffset.y);
		vec2 newUV2 = vec2(fUVx2.x - UVoffset.y, fUVx2.y - UVoffset.y);

		vertColor = texture(texture_0, vec2(newUV1.x, newUV1.y)) * 0.5f + 
					texture(texture_0, vec2(newUV2.y, newUV2.x)) * 0.5f;

		if(vertColor.a < 0.1)
			discard;
	}

	// ambient
    vec3 ambient = 0.4 * vertColor.rgb;

	vec3 norm = normalize(fNormal);

	// diffuse 
    vec3 lightDir = normalize(-theLights[0].direction.xyz);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diff * theLights[0].diffuse.rgb;

	float shadow = ShadowCalculation(fVertPosLightSpace);

	vec3 pixelColor = (ambient + (1.0 - shadow) * (diffuse)) * vertColor.xyz;

	float distanceToFogOrigin = length(fVertWorldPosition.xyz - fogViewOrigin.xyz);
	float fFogVisibility = exp(-pow(distanceToFogOrigin * fogDensity, fogGradient));
	fFogVisibility = clamp(fFogVisibility, 0.0, 1.0);
	pixelColor = mix(fogColor.rgb, pixelColor, fFogVisibility);

	gl_FragColor = vec4(pixelColor, 1.f);
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
	// perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // check whether current frag pos is in shadow
    float bias = 0.002;
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
	    for(int y = -1; y <= 1; ++y)
	    {
	        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
	        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
	    }    
	}
	shadow /= 9.0;

    return shadow;
}