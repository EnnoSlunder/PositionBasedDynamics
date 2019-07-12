#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
out vec4 vColor;


uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;


void main()
{
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(position, 1.0);
    vColor = vec4(color, 1.0);
}
