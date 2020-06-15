#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in ivec4 aBoneIds;
layout (location = 5) in vec4 aBoneWeights;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 defaultN;
    vec4 FragPosLightSpace;
    vec3 fromBones;
    vec3 deformationColor;
} vs_out;

struct Light {
    vec3 dir;
    vec3 color; // this is I_d (I_s = I_d, I_a = 0.3 * I_d)
};

const int MAX_BONES = 80;

uniform float useNormalMap;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
uniform Light light;

uniform vec3 viewPos;

uniform mat4 bones[2*MAX_BONES];

uniform vec3 bone_color[MAX_BONES];
uniform int mode;
uniform float MAX_ERROR_DISTANCE;
void main()
{
    mat4 boneTransform = mat4(1.0);
    mat4 GTboneTransform = mat4(1.0);

    //if there's animation information
    if(aBoneWeights[0]!=0.0){
        boneTransform = bones[aBoneIds[0]] * aBoneWeights[0];
        boneTransform += bones[aBoneIds[1]] * aBoneWeights[1];
        boneTransform += bones[aBoneIds[2]] * aBoneWeights[2];
        boneTransform += bones[aBoneIds[3]] * aBoneWeights[3];
    }
    vs_out.fromBones = vec3(0,0,0);
    if (mode == 1){
        vs_out.fromBones = bone_color[aBoneIds[0]] * aBoneWeights[0];
        vs_out.fromBones += bone_color[aBoneIds[1]] * aBoneWeights[1];
        vs_out.fromBones += bone_color[aBoneIds[2]] * aBoneWeights[2];
        vs_out.fromBones += bone_color[aBoneIds[3]] * aBoneWeights[3];
    }

    if (mode == 2){
        GTboneTransform = bones[MAX_BONES + aBoneIds[0]] * aBoneWeights[0];
        GTboneTransform += bones[MAX_BONES + aBoneIds[1]] * aBoneWeights[1];
        GTboneTransform += bones[MAX_BONES + aBoneIds[2]] * aBoneWeights[2];
        GTboneTransform += bones[MAX_BONES + aBoneIds[3]] * aBoneWeights[3];
    }
    gl_Position = projection * view * world * boneTransform * vec4(aPos, 1.0);
    vec4 GT_Position = projection * view * world * GTboneTransform * vec4(aPos, 1.0);

    vec3 displacement = (gl_Position - GT_Position).xyz;
    float distance = length(displacement);
    displacement = normalize(displacement);
    float error = (1 - distance / MAX_ERROR_DISTANCE);
    vs_out.deformationColor = vec3(1, error, error);




    vs_out.FragPos = vec3(world * boneTransform * vec4(aPos, 1.0));
    vs_out.TexCoords = aTexCoord;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(world)));
    vec3 T = normalize(normalMatrix * (boneTransform * vec4(aTangent, 0)).xyz);
    vec3 N = normalize(normalMatrix * (boneTransform * vec4(aNormal, 0)).xyz);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentLightPos = TBN * light.dir;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;


    //vs_out.defaultN = mat3(transpose(inverse(world))) * aNormal;
    vs_out.defaultN = transpose(inverse(mat3(world))) * (boneTransform * vec4(aNormal, 0)).xyz;


}
