#version 330

uniform int IDindex;

uniform vec3 color;

out vec3 FragColor;

void main()
{
    int id = 4;
    if(color.x == 0)
    {
        FragColor = vec3(IDindex,0,0);
    }
    else
    {
        FragColor = color;
    }

//    FragColor = vec3(1,0,0);
}












