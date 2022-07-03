#version 330 core

in vec4 fUVx2;
in vec3 fNormal;

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


uniform sLight theLights[20];

// texture samplers
uniform sampler2D shadowMap;
uniform sampler2D texture_00;

uniform bool useWholeColor;
uniform vec4 wholeColor;

//float ShadowCalculation(vec4 fragPosLightSpace);

void main()
{

	vec4 vertColor;

	if(useWholeColor)
	{
		vertColor = wholeColor;
	}
	else
	{
		vertColor = texture(texture_00, fUVx2.xy);

		if(vertColor.a < 0.1)
			discard;
	}

	// ambient
    vec3 ambient = vec3(0.3f, 0.3f, 0.3f) * vertColor.rgb;

	vec3 norm = normalize(fNormal);

	// diffuse 
    vec3 lightDir = normalize(-theLights[0].direction.xyz);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = theLights[0].diffuse.rgb * diff * vertColor.rgb;

	vec4 pixelColor = vec4(ambient + diffuse, 1.f);

	gl_FragColor = pixelColor;
}

//float ShadowCalculation(vec4 fragPosLightSpace)
//{
//	// perform perspective divide
//    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//
//    // transform to [0,1] range
//    projCoords = projCoords * 0.5 + 0.5;
//
//    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
//    float closestDepth = texture(shadowMap, projCoords.xy).r;
//
//    // get depth of current fragment from light's perspective
//    float currentDepth = projCoords.z;
//
//    // check whether current frag pos is in shadow
//    float bias = 0.002;
//	float shadow = 0.0;
//	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
//	for(int x = -1; x <= 1; ++x)
//	{
//	    for(int y = -1; y <= 1; ++y)
//	    {
//	        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
//	        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
//	    }    
//	}
//	shadow /= 9.0;
//
//    return shadow;
//}