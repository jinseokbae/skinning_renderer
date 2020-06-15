#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuseSampler;
    sampler2D specularSampler;
    sampler2D normalSampler;
    float shininess;
};

struct Light {
    vec3 dir;
    vec3 color; // this is I_d (I_s = I_d, I_a = 0.3 * I_d)
};

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 defaultN;
    vec4 FragPosLightSpace;
    vec3 fromBones;
    vec3 deformationColor;
} fs_in;

uniform float useNormalMap;
uniform float useSpecularMap;
uniform float useShadow;
uniform float useLighting;
uniform int mode;

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

uniform sampler2D depthMapSampler;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMapSampler, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.defaultN);
    vec3 lightDir = normalize(light.dir - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    //empirical clamp
    bias = min(bias, 0.006);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMapSampler, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMapSampler, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}


void main()
{
    //assuming all reflectance coefficients are 1;
    vec3 color = texture(material.diffuseSampler, fs_in.TexCoords).rgb;
	vec3 ambient = vec3(0.0,0.0,0.0);
	vec3 diffuse = vec3(0.0,0.0,0.0);
	vec3 specular = vec3(0.0,0.0,0.0);
	float shadow;
	vec3 lightDir;

	if (mode == 1){
	    color =  fs_in.fromBones;
	}

	if (mode == 2){
	    color = fs_in.deformationColor;
	}

    // on-off by key 3 (useLighting).
    // if useLighting is 0, return diffuse value without considering any lighting.(DO NOT CHANGE)
	if (useLighting < 0.5f){
        FragColor = vec4(color, 1.0);
        return;
    }


    // on-off by key 2 (useShadow).
    // calculate shadow
    // if useShadow is 0, do not consider shadow.
    // if useShadow is 1, consider shadow.


    // on-off by key 1 (useNormalMap).
    // if model does not have a normal map, this should be always 0.
    // if useNormalMap is 0, we use a geometric normal as a surface normal.
    // if useNormalMap is 1, we use a geometric normal altered by normal map as a surface normal.
    float diff;
    if(useNormalMap < 0.5f){
        vec3 norm = normalize(fs_in.defaultN);
        lightDir = normalize(light.dir - fs_in.FragPos);
        diff = max(dot(norm, lightDir), 0.0);
        diffuse = diff * color * light.color;
        ambient = 0.3* diffuse;
        if(useShadow > 0.5f){
            shadow = ShadowCalculation(fs_in.FragPosLightSpace);
            FragColor = vec4 (ambient + (1-shadow)*diffuse, 1.0);
            return;
        }
        else {
            FragColor = vec4(ambient + diffuse, 1.0);
            return;
        }

    }
    // useNormalMap > 0.5f

    vec3 normal = texture(material.normalSampler, fs_in.TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    diff = max(dot(lightDir, normal), 0.0);
    diffuse = diff * color * light.color;
    ambient = 0.3f * diffuse;

    if (useSpecularMap < 0.5f){
        if(useShadow > 0.5f){
            shadow = ShadowCalculation(fs_in.FragPosLightSpace);
            FragColor = vec4 (ambient + (1-shadow)*diffuse, 1.0);
            return;
        }
        else {
            FragColor = vec4(ambient + diffuse, 1.0);
            return;
        }
    }
    vec3 k_s;
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    vec3 temp = texture(material.specularSampler, fs_in.TexCoords).rgb;
    k_s = vec3(temp.x, temp.x, temp.x);//only considers red channel
    specular = spec * k_s * light.color;

    if(useShadow > 0.5f){
        shadow = ShadowCalculation(fs_in.FragPosLightSpace);
        FragColor = vec4 (ambient + (1-shadow)*(diffuse+specular), 1.0);
        return;
    }
    else {
        FragColor = vec4(ambient + diffuse + specular, 1.0);
        return;
    }
}