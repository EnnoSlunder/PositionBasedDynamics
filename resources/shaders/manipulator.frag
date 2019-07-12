#version 330
out vec4 fColor;

uniform vec3 viewPos;


in vec3 vColor;
in vec3 behindPlane;

void main()
{
//    if(behindPlane.x == 1)
//    {
//       fColor = vec4(1,1,1,0);
//    }
//    else
    {
       fColor = vec4(vColor,1);
    }

}























































