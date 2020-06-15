#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <glm/gtx/string_cast.hpp>


using namespace std;

struct BoneInfo
{
    glm::mat4 boneOffset;
    glm::mat4 finalTransform;
};

class Model
{
public:
    /*  Model Data */
    Mesh mesh;
    Texture* diffuse;
    Texture* normal;
    Texture* specular;
    unsigned int VAO;
    bool ignoreShadow = false;
    bool hasBone = false;
    const aiScene* scene;
    float animationTime;
    vector<glm::mat4> boneTransforms;
    vector<BoneInfo> boneInfos;
    map<string, int> boneMap;
    unsigned int mNumBones;
    aiNode* mRootNode;
    double mDuration;
    aiAnimation* animation;
    int GTFlag; // 0 is not GT, 1 is GT


    float model_scale;
    unsigned int mode = 0;
    //mode 0 : original model
    //mode 1 : visualize bone weights
    //mode 2 : deformation error

    Model() {    }
    /*  Functions   */
    // constructor, expects a filepath to a 3D model.
    Model(string const &path, float scale, unsigned int mode, int GTFlag ):diffuse(NULL), normal(NULL), specular(NULL){

        model_scale = scale;
        loadModel(path);
        this->VAO = mesh.VAO;
        this->mode = mode;
        this -> GTFlag = GTFlag;
    }

    void bind() {
        glBindVertexArray(mesh.VAO);
        glActiveTexture(GL_TEXTURE0);
        if(!(this -> diffuse)){
            this -> diffuse = new Texture("../resources/default_diffuse.png");
        }
        if(this -> diffuse) glBindTexture(GL_TEXTURE_2D, diffuse->ID);
        if (this->specular) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, specular->ID);
        }
        if (this->normal) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, normal->ID);
        }
    }

    vector<glm::mat4> extractBoneTransforms(float animationTime, const int animationIndex){
        this -> animationTime = fmod(animationTime, mDuration);
        ReadNodeHierarchy(this -> animationTime, mRootNode, glm::mat4(1.0f));
        for(int i = 0; i< mNumBones; i++){
            boneTransforms[i] = boneInfos[i].finalTransform;
        }
        return boneTransforms;
    }

private:
    /*  Functions   */
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        this -> scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
        // check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // process ASSIMP's root node recursively
        if(scene -> mNumAnimations > 0) {
            mDuration = scene -> mAnimations[0] -> mDuration;
            hasBone = true;
            mRootNode = new aiNode;
            ConstructNodeTree(scene->mRootNode, mRootNode);
            int k = mRootNode  -> mNumMeshes;
            processNode(mRootNode->mChildren[0], scene, path);
            animation = new aiAnimation;
            animation -> mNumChannels = scene -> mAnimations[0] -> mNumChannels;
            animation -> mChannels = new aiNodeAnim*[animation -> mNumChannels];
            for (int i = 0; i< animation -> mNumChannels; i++){
                animation -> mChannels[i] = new aiNodeAnim;
                animation -> mChannels[i] -> mNodeName = scene -> mAnimations[0] -> mChannels[i] -> mNodeName.data;
                animation -> mChannels[i] -> mNumScalingKeys = scene -> mAnimations[0] -> mChannels[i] -> mNumScalingKeys;
                animation -> mChannels[i] -> mNumRotationKeys = scene -> mAnimations[0] -> mChannels[i] -> mNumRotationKeys;
                animation -> mChannels[i] -> mNumPositionKeys = scene -> mAnimations[0] -> mChannels[i] -> mNumPositionKeys;

                animation -> mChannels[i] -> mScalingKeys = new aiVectorKey[animation -> mChannels[i] -> mNumScalingKeys];
                for (int j = 0; j < animation -> mChannels[i] -> mNumScalingKeys; j++){
                    animation -> mChannels[i] -> mScalingKeys[j].mValue = scene -> mAnimations[0] -> mChannels[i] -> mScalingKeys[j].mValue;
                    animation -> mChannels[i] -> mScalingKeys[j].mTime = scene -> mAnimations[0] -> mChannels[i] -> mScalingKeys[j].mTime;
                }

                animation -> mChannels[i] -> mRotationKeys = new aiQuatKey[animation -> mChannels[i] -> mNumRotationKeys];
                for (int j = 0; j < animation -> mChannels[i] -> mNumRotationKeys; j++){
                    animation -> mChannels[i] -> mRotationKeys[j].mValue = scene -> mAnimations[0] -> mChannels[i] -> mRotationKeys[j].mValue;
                    animation -> mChannels[i] -> mRotationKeys[j].mTime = scene -> mAnimations[0] -> mChannels[i] -> mRotationKeys[j].mTime;
                }

                animation -> mChannels[i] -> mPositionKeys = new aiVectorKey[animation -> mChannels[i] -> mNumPositionKeys];
                for (int j = 0; j < animation -> mChannels[i] -> mNumPositionKeys; j++){
                    animation -> mChannels[i] -> mPositionKeys[j].mValue = scene -> mAnimations[0] -> mChannels[i] -> mPositionKeys[j].mValue;
                    animation -> mChannels[i] -> mPositionKeys[j].mTime = scene -> mAnimations[0] -> mChannels[i] -> mPositionKeys[j].mTime;
                }
            }
        }
//
        processNode(scene->mRootNode->mChildren[0], scene, path);

    }

    void ConstructNodeTree(aiNode * aNode, aiNode* node){
        node -> mNumChildren = aNode -> mNumChildren;
        node -> mChildren = new aiNode*[node -> mNumChildren];
        if (aNode -> mMeshes){
            node -> mMeshes = new unsigned int;
            node -> mMeshes[0] = aNode -> mMeshes[0];
        }
        node -> mName = aNode -> mName.data;
        node -> mTransformation = aNode -> mTransformation;
        for (int i = 0; i < node -> mNumChildren; i++){
            node -> mChildren[i] = new aiNode;
            ConstructNodeTree(aNode -> mChildren[i], node -> mChildren[i]);
        }

    }

    // Original code : processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    // modified version : only process FIRST item of obj file. Do not consider texture or material.
    void processNode(aiNode *node, const aiScene *scene, string const& path)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[0]];
        // check if this model has bones
//        if(mesh -> mNumBones){
//            this -> hasBone = true;
//            this -> animationComponent = new AnimationComponent(scene, path);
//        }
        if(hasBone){
            this -> mNumBones = this -> scene -> mMeshes[0] -> mNumBones;
            for (int i = 0 ; i < scene -> mMeshes[0] -> mNumBones ; i++){
                boneTransforms.push_back(glm::mat4(1.0f));
                BoneInfo temp = BoneInfo();
                temp.boneOffset = ToMat4(scene -> mMeshes[0] -> mBones[i] -> mOffsetMatrix);
//                temp.boneOffset[3] = model_scale * temp.boneOffset[3];
//                temp.boneOffset[3][3] = 1.0f;
                temp.finalTransform = glm::mat4(1.0f);
                boneInfos.push_back(temp);
                string t = this -> scene -> mMeshes[0] -> mBones[i] -> mName.data;
                boneMap[this -> scene -> mMeshes[0] -> mBones[i] -> mName.data] = i;
            }
        }
        this->mesh = processMesh(mesh, scene);
    }


    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // Walk through each of the mesh's vertices

        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector;
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            // texture coordinates
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            // tangent

            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            vertices.push_back(vertex);
        }
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // Let's fill out vertices - bones relation
        if(hasBone){
            int numBones = 0;
            for (unsigned int i = 0; i < mesh -> mNumBones; i++){
                //check for each bones
                unsigned int boneIndex = numBones ++;
                for (int j = 0; j < mesh -> mBones[i] -> mNumWeights; j++){
                    // check how many vertices are influenced by this bone
                    unsigned int vertexId = mesh->mBones[i]->mWeights[j].mVertexId;
                    float weight = mesh->mBones[i]->mWeights[j].mWeight;

                    for (int k = 0; k < 4; k ++){
//                    unsigned int vectorId = k / 4;
                        unsigned int elementId = k % 4;
                        // same functionality with vector push_back
                        if (vertices[vertexId].boneWeights[elementId] == 0.0){
                            vertices[vertexId].boneIds[elementId] = boneIndex;
                            vertices[vertexId].boneWeights[elementId] = weight;
                            break;
                        }
                    }
                }
            }
        }
        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices);
    }



    glm::mat4 ToMat4(const aiMatrix4x4 from)
    {
        glm::mat4 to;

        to[0][0] = (GLfloat)from.a1; to[0][1] = (GLfloat)from.b1;  to[0][2] = (GLfloat)from.c1; to[0][3] = (GLfloat)from.d1;
        to[1][0] = (GLfloat)from.a2; to[1][1] = (GLfloat)from.b2;  to[1][2] = (GLfloat)from.c2; to[1][3] = (GLfloat)from.d2;
        to[2][0] = (GLfloat)from.a3; to[2][1] = (GLfloat)from.b3;  to[2][2] = (GLfloat)from.c3; to[2][3] = (GLfloat)from.d3;
        to[3][0] = (GLfloat)from.a4; to[3][1] = (GLfloat)from.b4;  to[3][2] = (GLfloat)from.c4; to[3][3] = (GLfloat)from.d4;

        return to;
    }

    void ReadNodeHierarchy(float animationTime, const aiNode* node, const glm::mat4& parentTransform){
        string nodeName(node -> mName.data);
        glm::mat4 nodeTransform = glm::mat4(1.0f);
//        nodeTransform = 100.0f * nodeTransform;
//        nodeTransform[3][3] = 1.0f;
//        cout << glm::to_string(nodeTransform) << endl;
        const aiNodeAnim* nodeAnim = FindNodeAnim(animation, nodeName);


        if(nodeAnim) {
            nodeTransform = ToMat4(node->mTransformation);

            const aiVector3D& scaling = CalcInterpolatedValueFromKey(animationTime, nodeAnim->mNumScalingKeys, nodeAnim->mScalingKeys);
            glm::mat4 scalingM = scale(glm::mat4(1.0f), glm::vec3(scaling.x, scaling.y, scaling.z));

            const aiQuaternion& rotationQ = CalcInterpolatedValueFromKey(animationTime, nodeAnim->mNumRotationKeys, nodeAnim->mRotationKeys);
            glm::mat4 rotationM = glm::mat4(glm::quat(rotationQ.w, rotationQ.x, rotationQ.y, rotationQ.z));

            const aiVector3D& translation = CalcInterpolatedValueFromKey(animationTime, nodeAnim->mNumPositionKeys, nodeAnim->mPositionKeys);
            glm::mat4 translationM = translate(glm::mat4(1.0f),  glm::vec3(translation.x, translation.y, translation.z));

            nodeTransform = translationM * rotationM * scalingM;
        }

        glm::mat4 globalTransform = parentTransform * nodeTransform;
        if (boneMap.find(nodeName) != boneMap.end()){

            unsigned int boneIndex = boneMap[nodeName];
            boneInfos[boneIndex].finalTransform =
                    globalTransform *
                    boneInfos[boneIndex].boneOffset;

        }
        for (int i = 0; i < node -> mNumChildren; i++){
            ReadNodeHierarchy(animationTime, node->mChildren[i], globalTransform);
        }



    }
    aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const string nodeName)
    {
        for (int i = 0; i < animation->mNumChannels; i++)
            if (animation->mChannels[i]->mNodeName.data == nodeName)
                return animation->mChannels[i];
        return nullptr;
    }

    aiVector3D CalcInterpolatedValueFromKey(float animationTime, const int numKeys, const aiVectorKey* const vectorKey) const
    {
        aiVector3D ret;
        if (numKeys == 1)
        {
            ret = vectorKey[0].mValue;
            return ret;
        }

        unsigned int keyIndex = FindKeyIndex(animationTime, numKeys, vectorKey);
        unsigned int nextKeyIndex = keyIndex + 1;

        assert(nextKeyIndex < numKeys);

        float deltaTime = vectorKey[nextKeyIndex].mTime - vectorKey[keyIndex].mTime;
        float factor = (animationTime - (float)vectorKey[keyIndex].mTime) / deltaTime;

        assert(factor >= 0.0f && factor <= 1.0f);

        const aiVector3D& startValue = vectorKey[keyIndex].mValue;
        const aiVector3D& endValue = vectorKey[nextKeyIndex].mValue;

        ret.x = startValue.x + (endValue.x - startValue.x) * factor;
        ret.y = startValue.y + (endValue.y - startValue.y) * factor;
        ret.z = startValue.z + (endValue.z - startValue.z) * factor;

        return ret;
    }

    aiQuaternion CalcInterpolatedValueFromKey(float animationTime, const int numKeys, const aiQuatKey* const quatKey) const
    {
        aiQuaternion ret;
        if (numKeys == 1)
        {
            ret = quatKey[0].mValue;
            return ret;
        }

        unsigned int keyIndex = FindKeyIndex(animationTime, numKeys, quatKey);
        unsigned int nextKeyIndex = keyIndex + 1;

        assert(nextKeyIndex < numKeys);

        float deltaTime = quatKey[nextKeyIndex].mTime - quatKey[keyIndex].mTime;
        float factor = (animationTime - (float)quatKey[keyIndex].mTime) / deltaTime;

        assert(factor >= 0.0f && factor <= 1.0f);

        const aiQuaternion& startValue = quatKey[keyIndex].mValue;
        const aiQuaternion& endValue = quatKey[nextKeyIndex].mValue;
        aiQuaternion::Interpolate(ret, startValue, endValue, factor);
        ret = ret.Normalize();

        return ret;
    }

    unsigned int FindKeyIndex(const float animationTime, const int numKeys, const aiVectorKey* const vectorKey) const
    {
        assert(numKeys > 0);
        for (int i = 0; i < numKeys - 1; i++)
            if (animationTime < (float)vectorKey[i + 1].mTime)
                return i;
        assert(0);
    }

    unsigned int FindKeyIndex(const float animationTime, const int numKeys, const aiQuatKey* const quatKey) const
    {
        assert(numKeys > 0);
        for (int i = 0; i < numKeys - 1; i++)
            if (animationTime < (float)quatKey[i + 1].mTime)
                return i;
        assert(0);
    }

};
class Entity {
public:
    Model* model;
    glm::mat4 modelMatrix;
    Entity(Model* model, glm::mat4 modelMatrix) {
        this->model = model;
        this->modelMatrix = modelMatrix;
    }

    Entity(Model* model, glm::vec3 position, float rotX, float rotY, float rotZ, float scale) {
        this->model = model;
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, position);
        transform = glm::rotate(transform, glm::radians(rotX), glm::vec3(1.0f, 0.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(rotZ), glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, glm::vec3(scale));

        this->modelMatrix = transform;
    }

    glm::mat4 getModelMatrix() {
        return this->modelMatrix;
    }
};

#endif