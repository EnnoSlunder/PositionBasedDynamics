#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

//uniform samplerCube skybox;

uniform sampler2D skybox;

void main()
{
    FragColor = texture(skybox, TexCoords);
//    FragColor = vec4(0,1,0,1);
}
