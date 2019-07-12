#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 barycentric;

out vec3 vFragPos;
out vec3 vColor;
out vec3 vNormal;
out vec3 vBC;

out vec3 behindPlane;

uniform vec3 pN;
uniform vec3 pO;
uniform vec3 color;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

void main()
{

    vec3 modelPos = ModelMatrix[3].xyz;
    vec3 viewPos = ViewMatrix[3].xyz;
    float d = length(viewPos - modelPos);

    //    float scale = (d / 8);
    float scale = 1.0;
//      float scale = d / 4;


    vec4 worldPos = ModelMatrix * vec4((d / 8) * position, 1.0);
    vec4 screenPos = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(scale * position, 1.0);


    vec3 temp = (worldPos.xyz - pO);
    float dist = dot(temp, pN);
    behindPlane.x = 0;
    if(dist < 0.0)
    {
        behindPlane.x = 1;
    }

    gl_Position = screenPos;

    vColor = color;


//    if(color.x == 0.0f && color.y == 0.0f && color.z == 0.0f)
//    {
//        vColor = vec3(1,0,1);
//    }
}

//        float diff = max(dot(normal, lightDir), 0.0);
