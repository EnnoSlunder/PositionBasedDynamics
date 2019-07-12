#version 330 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


out VS_OUT {
    vec3 normal;
} vs_out;


void main()
{
//    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
//    vs_out.normal = vec3(projection * vec4(normalMatrix * Normal, 0.0));
////    vs_out.normal = vec3(projection * view * model * vec4(aNormal, 0.0));
//    gl_Position = projection * view * model * vec4(Position, 1.0);

    gl_Position = vec4(Position, 1.0);
//    gl_Position = projection * view * model * vec4(Position, 1.0);
}



