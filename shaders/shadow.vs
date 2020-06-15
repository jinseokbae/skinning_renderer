#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in ivec4 aBoneIds;
layout (location = 5) in vec4 aBoneWeights;
const int MAX_BONES = 80;

uniform mat4 lightSpaceMatrix;
uniform mat4 world;

uniform mat4 bones[MAX_BONES];

void main()
{
    mat4 boneTransform = mat4(1.0);

    //if there's animation information
    if(aBoneWeights[0]!=0.0){
        boneTransform = bones[aBoneIds[0]] * aBoneWeights[0];
        boneTransform += bones[aBoneIds[1]] * aBoneWeights[1];
        boneTransform += bones[aBoneIds[2]] * aBoneWeights[2];
        boneTransform += bones[aBoneIds[3]] * aBoneWeights[3];
    }

    gl_Position = lightSpaceMatrix * world * boneTransform * vec4(aPos, 1.0);
}