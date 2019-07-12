#version 330 core
layout (points) in;
//layout (triangle_strip, max_vertices = 9) out;
layout (line_strip, max_vertices = 2) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 2.0;

void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE;
    EmitVertex();

    gl_Position = vec4(0,0,0,1);
    EmitVertex();

    EndPrimitive();
}

void main()
{
   GenerateLine(0); // first vertex normal
   GenerateLine(1); // second vertex normal
   GenerateLine(2); // third vertex normal


}


/*
#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 6) out;

in vec3 vNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
*/


/*
void main()
{
    vec3 Pos = gl_in[0].gl_Position.xyz;
    vec4 position = projection * view * model * vec4(Pos, 1);
    
    mat4 Translation = mat4(
                        vec4(1.0, 0.0, 0.0, 0.0),
                        vec4(0.0, 1.0, 0.0, 0.0),
                        vec4(0.0, 0.0, 1.0, 0.0),
                        vec4(model[3].xyzw));
    
    float d = length(view[3].xyz) / 10 ;
//    gl_Position = projection * view * model * vec4(d * (vec3(1,0,0)),1.0);

    gl_Position = position;
    EmitVertex();
    gl_Position = position + vNormal;
    EmitVertex();
    EndPrimitive();


    gl_Position = position;
    EmitVertex();
    gl_Position = projection * view * model * vec4(Pos +(vec3(1,0,0)),1.0);
    EmitVertex();


    gl_Position = position;
    EmitVertex();
    gl_Position = projection * view * model * vec4( Pos + (vec3(0,0,1)),1.0);
    EmitVertex();
    EndPrimitive();
    
    
    gl_Position = position;
    EmitVertex();
    gl_Position = projection * view * model * vec4(Pos + (vec3(0,1,0)),1.0);
    EmitVertex();
    EndPrimitive();


}
*/



