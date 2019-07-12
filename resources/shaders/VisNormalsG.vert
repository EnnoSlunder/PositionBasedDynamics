#version 330 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = vec3(projection * vec4(normalMatrix * Normal, 0.0));
////    vs_out.normal = vec3(projection * view * model * vec4(aNormal, 0.0));
    gl_Position = projection * view * model * vec4(Position, 1.0);
}


/*

#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;

void main()
{
    gl_Position = vec4(Position, 1.0);

}



////             Joey Normal shader

*/
