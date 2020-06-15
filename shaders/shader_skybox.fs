#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skyboxTexture1;

void main()
{
    FragColor = vec4(texture(skyboxTexture1, TexCoords).rgb, 1.0f);
}
