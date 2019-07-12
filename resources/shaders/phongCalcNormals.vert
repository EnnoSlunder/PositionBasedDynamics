#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 barycentric;

out vec3 vFragPos;
out vec3 vColor;
out vec3 vNormal;
out vec3 vBC;

//out vec3 wireFrameColor;
//out vec4 overlayColorl;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

void main()
{
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(position, 1.0);
    vFragPos = vec3(ModelMatrix * vec4(position, 1.0) );
    vColor = vec3(0.2f , 0.0f , 0.5f);
//    vNormal = normal;
    vBC = barycentric;
    vNormal = mat3(transpose(inverse( ModelMatrix ))) * normal;
}
