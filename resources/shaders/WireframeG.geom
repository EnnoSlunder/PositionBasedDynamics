#version 330 core
layout(triangles) in;
//layout (line_strip, max_vertices = 12) out;
layout (triangle_strip, max_vertices = 64) out;

const float MAGNITUDE = 2.0;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 vertexNormal;

float LINE_WIDTH = 0.01;

vec4 doEmitVertex(vec4 p, vec2 edge) {
        // a common normal
        //vertexNormal = (iModelViewNormal * vec4(n, 0)).xyz;

        // edges of the line, emitted as 2 vertices
        gl_Position = p + vec4(edge, 0, 0);
         EmitVertex();
        gl_Position = p - vec4(edge, 0, 0);
         EmitVertex();

        return (p + vec4(edge, 0, 0));
}

vec4 doEmitLine(vec3 p1, vec3 p2) {
        // normalize p1
        vec4 np1 = projection * view * model * vec4(p1, 1);
        np1 = np1 / np1.w;

        // normalize p2
        vec4 np2 = projection * view * model * vec4(p2, 1);
        np2 = np2 / np2.w;

        // perpendicular "edge" vector
        vec2 edge = (np2-np1).xy;
        edge = normalize(vec2(edge.y, -edge.x)) * LINE_WIDTH;

        // emit the line vertices
        vec4 innerVert1 = doEmitVertex(np1, edge);
        vec4 innerVert2 =  doEmitVertex(np2, edge);

        return innerVert1;

}

void main() {
    vec4 triVerts[3];

    for(int i = 0; i < gl_in.length(); i++) {
            // each "line" is emitted as a triangle strip
            vec4 vertPos = doEmitLine(
                    gl_in[i].gl_Position.xyz,
                    gl_in[(i+1) % gl_in.length()].gl_Position.xyz
            );

              EndPrimitive();
            triVerts[i] = vertPos;
    }

    /*
    for(int i = 0; i < 3; i++)
    {
        gl_Position = triVerts[i];
        EmitVertex();
    }
    EndPrimitive();
    */
}
