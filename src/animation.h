//#include <glad/glad.h>
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/quaternion.hpp>
//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//
//#include "shader.h"
//
//#include <string>
//#include <fstream>
//#include <sstream>
//#include <iostream>
//#include <map>
//#include <vector>
////
////struct BoneInfo
////{
////    glm::mat4 boneOffset;
////    glm::mat4 finalTransform;
////};
//
//class AnimationComponent{
//public:
//    const aiScene* scene;
//    float animationTime;
//    vector<glm::mat4> boneTransforms;
//    vector<BoneInfo> boneInfos;
//    map<string, int> boneMap;
//    unsigned int NumBones;
//    double mDuration;
//    aiNode* mRootNode;
//    AnimationComponent(){
//        scene = NULL;
//    }
//    AnimationComponent(const aiScene* scene, string const& path){
//        Assimp::Importer importer;
//        this -> scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
//        this -> NumBones = this -> scene -> mMeshes[0] -> mNumBones;
//        for (int i = 0 ; i < scene -> mMeshes[0] -> mNumBones ; i++){
//            boneTransforms.push_back(glm::mat4(1.0f));
//            BoneInfo temp = BoneInfo();
//            temp.boneOffset = ToMat4(scene -> mMeshes[0] -> mBones[i] -> mOffsetMatrix);
//            temp.finalTransform = glm::mat4(1.0f);
//            boneInfos.push_back(temp);
//            string t = this -> scene -> mMeshes[0] -> mBones[i] -> mName.data;
//            boneMap[this -> scene -> mMeshes[0] -> mBones[i] -> mName.data] = i;
//        }
//        //assume one animation
////        this -> mDuration = scene->mAnimations[0]->mDuration;
////        aiNode* mRootNode = new aiNode;
////        *mRootNode = *(scene -> mRootNode);
//
//    }
//
//    vector<glm::mat4> extractBoneTransforms(float animationTime, const int animationIndex){
//        double k = this -> scene -> mAnimations[0] -> mDuration;
//        this -> animationTime = fmod(animationTime, this -> scene->mAnimations[animationIndex]->mDuration);
////        this -> animationTime = fmod(animationTime, this -> mDuration);
//        ReadNodeHierarchy(this -> animationTime, scene->mRootNode, glm::mat4(1.0f));
////        ReadNodeHierarchy(this -> animationTime, this -> mRootNode, glm::mat4(1.0f));
//        for(int i = 0; i< scene -> mMeshes[0] -> mNumBones; i++){
//            boneTransforms[i] = boneInfos[i].finalTransform;
//        }
//        return boneTransforms;
//    }
//
//
//
//private:
//    glm::mat4 ToMat4(const aiMatrix4x4 from)
//    {
//        glm::mat4 to;
//
//        to[0][0] = (GLfloat)from.a1; to[0][1] = (GLfloat)from.b1;  to[0][2] = (GLfloat)from.c1; to[0][3] = (GLfloat)from.d1;
//        to[1][0] = (GLfloat)from.a2; to[1][1] = (GLfloat)from.b2;  to[1][2] = (GLfloat)from.c2; to[1][3] = (GLfloat)from.d2;
//        to[2][0] = (GLfloat)from.a3; to[2][1] = (GLfloat)from.b3;  to[2][2] = (GLfloat)from.c3; to[2][3] = (GLfloat)from.d3;
//        to[3][0] = (GLfloat)from.a4; to[3][1] = (GLfloat)from.b4;  to[3][2] = (GLfloat)from.c4; to[3][3] = (GLfloat)from.d4;
//
//        return to;
//    }
//
//    void ReadNodeHierarchy(float animationTime, const aiNode* node, const glm::mat4& parentTransform){
//        string nodeName(node -> mName.data);
//        const aiAnimation * animation = scene -> mAnimations[0];
//        glm::mat4 nodeTransform = ToMat4(node->mTransformation);
//
//        const aiNodeAnim* nodeAnim = FindNodeAnim(animation, nodeName);
//
//        if(nodeAnim) {
//            const aiVector3D& scaling = CalcInterpolatedValueFromKey(animationTime, nodeAnim->mNumScalingKeys, nodeAnim->mScalingKeys);
//            glm::mat4 scalingM = scale(glm::mat4(1.0f), glm::vec3(scaling.x, scaling.y, scaling.z));
//
//            const aiQuaternion& rotationQ = CalcInterpolatedValueFromKey(animationTime, nodeAnim->mNumRotationKeys, nodeAnim->mRotationKeys);
//            glm::mat4 rotationM = glm::mat4(glm::quat(rotationQ.w, rotationQ.x, rotationQ.y, rotationQ.z));
//
//            const aiVector3D& translation = CalcInterpolatedValueFromKey(animationTime, nodeAnim->mNumPositionKeys, nodeAnim->mPositionKeys);
//            glm::mat4 translationM = translate(glm::mat4(1.0f), glm::vec3(translation.x, translation.y, translation.z));
//
//            nodeTransform = translationM * rotationM * scalingM;
//        }
//
//        glm::mat4 globalTransform = parentTransform * nodeTransform;
//
//        if (boneMap.find(nodeName) != boneMap.end()){
//            unsigned int boneIndex = boneMap[nodeName];
//            boneInfos[boneIndex].finalTransform =
//                    ToMat4(scene->mRootNode->mTransformation) *
//                    globalTransform *
//                    boneInfos[boneIndex].boneOffset;
//        }
//
//        for (int i = 0; i < node -> mNumChildren; i++){
//            ReadNodeHierarchy(animationTime, node->mChildren[i], globalTransform);
//        }
//    }
//    aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const string nodeName)
//    {
//        for (int i = 0; i < animation->mNumChannels; i++)
//            if (animation->mChannels[i]->mNodeName.data == nodeName)
//                return animation->mChannels[i];
//        return nullptr;
//    }
//
//    aiVector3D CalcInterpolatedValueFromKey(float animationTime, const int numKeys, const aiVectorKey* const vectorKey) const
//    {
//        aiVector3D ret;
//        if (numKeys == 1)
//        {
//            ret = vectorKey[0].mValue;
//            return ret;
//        }
//
//        unsigned int keyIndex = FindKeyIndex(animationTime, numKeys, vectorKey);
//        unsigned int nextKeyIndex = keyIndex + 1;
//
//        assert(nextKeyIndex < numKeys);
//
//        float deltaTime = vectorKey[nextKeyIndex].mTime - vectorKey[keyIndex].mTime;
//        float factor = (animationTime - (float)vectorKey[keyIndex].mTime) / deltaTime;
//
//        assert(factor >= 0.0f && factor <= 1.0f);
//
//        const aiVector3D& startValue = vectorKey[keyIndex].mValue;
//        const aiVector3D& endValue = vectorKey[nextKeyIndex].mValue;
//
//        ret.x = startValue.x + (endValue.x - startValue.x) * factor;
//        ret.y = startValue.y + (endValue.y - startValue.y) * factor;
//        ret.z = startValue.z + (endValue.z - startValue.z) * factor;
//
//        return ret;
//    }
//
//    aiQuaternion CalcInterpolatedValueFromKey(float animationTime, const int numKeys, const aiQuatKey* const quatKey) const
//    {
//        aiQuaternion ret;
//        if (numKeys == 1)
//        {
//            ret = quatKey[0].mValue;
//            return ret;
//        }
//
//        unsigned int keyIndex = FindKeyIndex(animationTime, numKeys, quatKey);
//        unsigned int nextKeyIndex = keyIndex + 1;
//
//        assert(nextKeyIndex < numKeys);
//
//        float deltaTime = quatKey[nextKeyIndex].mTime - quatKey[keyIndex].mTime;
//        float factor = (animationTime - (float)quatKey[keyIndex].mTime) / deltaTime;
//
//        assert(factor >= 0.0f && factor <= 1.0f);
//
//        const aiQuaternion& startValue = quatKey[keyIndex].mValue;
//        const aiQuaternion& endValue = quatKey[nextKeyIndex].mValue;
//        aiQuaternion::Interpolate(ret, startValue, endValue, factor);
//        ret = ret.Normalize();
//
//        return ret;
//    }
//
//    unsigned int FindKeyIndex(const float animationTime, const int numKeys, const aiVectorKey* const vectorKey) const
//    {
//        assert(numKeys > 0);
//        for (int i = 0; i < numKeys - 1; i++)
//            if (animationTime < (float)vectorKey[i + 1].mTime)
//                return i;
//        assert(0);
//    }
//
//    unsigned int FindKeyIndex(const float animationTime, const int numKeys, const aiQuatKey* const quatKey) const
//    {
//        assert(numKeys > 0);
//        for (int i = 0; i < numKeys - 1; i++)
//            if (animationTime < (float)quatKey[i + 1].mTime)
//                return i;
//        assert(0);
//    }
//};
