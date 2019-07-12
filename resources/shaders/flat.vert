#version 330 core
layout (location = 0) in vec3 position;

out vec3 vColor;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform vec3 Color;

void main()
{
        gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(position, 1.0);
        vColor = Color;
}
