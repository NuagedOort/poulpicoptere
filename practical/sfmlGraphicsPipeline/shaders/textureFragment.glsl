#version 400

//Structure definition for Material, DirectionalLight, PointLight and SpotLight
//Parameters are exactly the same as the corresponding C++ classes
//Refer to the C++ documentation for more information

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
    vec3 position;
    vec3 spotDirection;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float innerCutOff;
    float outerCutOff;
};

uniform Material material;
uniform DirectionalLight directionalLight;

#define MAX_NR_POINT_LIGHTS 10
uniform int numberOfPointLight;
uniform PointLight pointLight[MAX_NR_POINT_LIGHTS];

#define MAX_NR_SPOT_LIGHTS 10
uniform int numberOfSpotLight;
uniform SpotLight spotLight[MAX_NR_SPOT_LIGHTS];

uniform sampler2D texSampler;

// Surfel: a SURFace ELement. All coordinates are in world space
in vec2 surfel_texCoord;
in vec3 surfel_position;
in vec4 surfel_color;
in vec3 surfel_normal;

// Camera position in world space
in vec3 cameraPosition;

// Resulting color of the fragment shader
out vec4 outColor;

//Phong illumination model for a directional light
vec3 computeDirectionalLight(DirectionalLight light, vec3 surfel_to_camera)
{
    vec3 surfel_to_light = -light.direction;

    // Diffuse shading
    float diffuse_factor = max(dot(surfel_normal, surfel_to_light), 0.0);

    // Specular shading
    vec3 reflect_direction = reflect(-surfel_to_light, surfel_normal);
    float specular_factor = pow(max(dot(surfel_to_camera, reflect_direction), 0.0), material.shininess);

    // Combine results
    vec3 ambient  =                   light.ambient  * material.ambient ;
    vec3 diffuse  = diffuse_factor  * light.diffuse  * material.diffuse ;
    vec3 specular = specular_factor * light.specular * material.specular;

    return (ambient + diffuse + specular);
}

//Phong illumination model for a point light
vec3 computePointLight(PointLight light, vec3 surfel_to_camera)
{
    // Diffuse shading
    vec3 surfel_to_light = light.position - surfel_position;
    float distance = length( surfel_to_light );
    surfel_to_light *= float(1) / distance;
    float diffuse_factor = max(dot(surfel_normal, surfel_to_light), 0.0);

    // Specular shading
    vec3 reflect_direction = reflect(-surfel_to_light, surfel_normal);
    float specular_factor = pow(max(dot(surfel_to_camera, reflect_direction), 0.0), material.shininess);

    // Attenuation: TODO
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    //float attenuation = 1.0;

    // Combine results
    vec3 ambient  = attenuation *                   light.ambient  * material.ambient ;
    vec3 diffuse  = attenuation * diffuse_factor  * light.diffuse  * material.diffuse ;
    vec3 specular = attenuation * specular_factor * light.specular * material.specular;

    return (ambient + diffuse + specular);
}

//Phong illumination model for a spot light
vec3 computeSpotLight(SpotLight light, vec3 surfel_to_camera)
{
    // Diffuse
    vec3 surfel_to_light = light.position - surfel_position;
    float distance = length( surfel_to_light );
    surfel_to_light *= float(1) / distance;
    float diffuse_factor = max(dot(surfel_normal, surfel_to_light), 0.0);

    // Specular
    vec3 reflect_direction = reflect(-surfel_to_light, surfel_normal);
    float specular_factor = pow(max(dot(surfel_to_camera, reflect_direction), 0.0), material.shininess);

    // Spotlight (soft edges): TODO
    //float intensity = 1.0;
    float cos_theta = dot(surfel_to_light, -light.spotDirection);
    float intensity = clamp( (cos_theta - light.outerCutOff ) / ( light.innerCutOff - light.outerCutOff ), 0, 1 );

    // Attenuation
    //float attenuation = 1.0;
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Combine results
    vec3 ambient  =             attenuation *                   light.ambient  * material.ambient ;
    vec3 diffuse  = intensity * attenuation * diffuse_factor  * light.diffuse  * material.diffuse ;
    vec3 specular = intensity * attenuation * specular_factor * light.specular * material.specular;

    return (ambient + diffuse + specular);
}

float ComputeScattering(float lightDotView) {
    float result = 1.0f - G_SCATTERING * G_SCATTERING;
    result /= (4.0f * PI * pow(1.0f + G_SCATTERING * G_SCATTERING - (2.0f * G_SCATTERING) *      lightDotView, 1.5f));
    return result;
}

float4 main(const PS_INPUT input) : SV_TARGET {

    float upSampledDepth = depth.Load(int3(screenCoordinates, 0)).x;

    float3 color = 0.0f.xxx;
    float totalWeight = 0.0f;

    // Select the closest downscaled pixels.

    int xOffset = screenCoordinates.x % 2 == 0 ? -1 : 1;
    int yOffset = screenCoordinates.y % 2 == 0 ? -1 : 1;

    int2 offsets[] = {int2(0, 0),
    int2(0, yOffset),
    int2(xOffset, 0),
    int2(xOffset, yOffset)};

    for (int i = 0; i < 4; i ++) {
        float3 downscaledColor = volumetricLightTexture.Load(int3(downscaledCoordinates + offsets[i], 0));

        float downscaledDepth = depth.Load(int3(downscaledCoordinates, + offsets[i] 1));

        float currentWeight = 1.0f;
        currentWeight *= max(0.0f, 1.0f - (0.05f) * abs(downscaledDepth - upSampledDepth));

        color += downscaledColor * currentWeight;
        totalWeight += currentWeight;
    }

    float3 volumetricLight;
    const float epsilon = 0.0001f;
    volumetricLight.xyz = color/(totalWeight + epsilon);

    return float4(volumetricLight.xyz, 1.0f);

}

void main()
{
    //Surface to camera vector
    vec3 surfel_to_camera = normalize( cameraPosition - surfel_position );

    int clampedNumberOfPointLight = max(0, min(numberOfPointLight, MAX_NR_POINT_LIGHTS));
    int clampedNumberOfSpotLight = max(0, min(numberOfSpotLight, MAX_NR_SPOT_LIGHTS));

    vec3 tmpColor = vec3(0.0, 0.0, 0.0);

    tmpColor += computeDirectionalLight(directionalLight, surfel_to_camera);

    for(int i=0; i<clampedNumberOfPointLight; ++i)
        tmpColor += computePointLight(pointLight[i], surfel_to_camera);

    for(int i=0; i<clampedNumberOfSpotLight; ++i)
        tmpColor += computeSpotLight(spotLight[i], surfel_to_camera);

    vec4 textureColor = texture(texSampler, surfel_texCoord);
    outColor = textureColor*vec4(tmpColor,1.0);


    // Mie scaterring approximated with Henyey-Greenstein phase function.
    float3 worldPos = getWorldPosition(input.TexCoord);
    float3 startPosition = g_CameraPosition;

    float3 rayVector = endRayPosition.xyz- startPosition;

    float rayLength = length(rayVector);
    float3 rayDirection = rayVector / rayLength;

    float stepLength = rayLength / NB_STEPS;

    float3 step = rayDirection * stepLength;

    float3 currentPosition = startPosition;

    float3 accumFog = 0.0f.xxx;

    for (int i = 0; i < NB_STEPS; i++) {
        float4 worldInShadowCameraSpace = mul(float4(currentPosition, 1.0f), g_ShadowViewProjectionMatrix);
        worldInShadowCameraSpace /= worldInShadowCameraSpace.w;

        float shadowMapValue = shadowMap.Load(uint3(shadowmapTexCoord, 0)).r;

        if (shadowMapValue > worldByShadowCamera.z) {
            accumFog += ComputeScattering(dot(rayDirection, sunDirection)).xxx * g_SunColor;
        }
        currentPosition += step;
    }
    accumFog /= NB_STEPS;


    ditherPattern[4][4] = {{ 0.0f, 0.5f, 0.125f, 0.625f},
    { 0.75f, 0.22f, 0.875f, 0.375f},
    { 0.1875f, 0.6875f, 0.0625f, 0.5625},
    { 0.9375f, 0.4375f, 0.8125f, 0.3125}};

    // Offset the start position.
    startPosition += step * ditherValue;
}