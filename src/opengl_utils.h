#ifndef OPENGL_UTILS_H
#define OPENGL_UTILS_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
class VAO{
public:
    unsigned int ID;
    unsigned int associatedVBOID;
    //std::vector<unsigned int> associatedVBOs;
};
VAO *getVAOFromAttribData(const std::vector<float> &attrib_data, const std::vector<unsigned int> &attrib_sizes)
{
    VAO *vao = new VAO();
    //unsigned int VBO;
    glGenVertexArrays(1, &(vao->ID));
    glGenBuffers(1, &(vao->associatedVBOID));
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(vao->ID);
    glBindBuffer(GL_ARRAY_BUFFER, vao->associatedVBOID);
    glBufferData(GL_ARRAY_BUFFER, attrib_data.size(), &attrib_data[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int offset = 0;
    unsigned int index = 0;
    unsigned int size_sum = 0;
    for(auto const& value: attrib_sizes){
        size_sum += value;
    }
    for(auto const& value: attrib_sizes){
        glVertexAttribPointer(index, value, GL_FLOAT, GL_FALSE, size_sum * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(index);
        index++;
        offset += value;
    }
    return vao;
}
void getPositionVAO(const float* vertices, unsigned int size, unsigned int& VAO, unsigned int& VBO)
{
    //unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}


#endif
