#version 330 core
out vec4 FragColor;

in vec2 vTexCoords;

//uniform sampler2D screenTexture;
uniform sampler2D textureA;
//uniform sampler2D textureB;

void main()
{

    vec4 texA = texture(textureA, vTexCoords);
//    vec4 texB = texture(textureB, vTexCoords);

//    vec3 col = texture(screenTexture, vTexCoords).rgb;
//    vec4 test = texture(screenTexture, vTexCoords);
//    FragColor = vec4(col, 1.0);
//    if(test[3] < 0.9)
//    {
//        test = vec4(0,1,0,1);
//    }
    FragColor = texA;
//    FragColor = vec4(0,1,0,1);
}



/*
uniform sampler2D qt_Texture0;
varying vec4 qt_TexCoord0;

void main(void)
{
    gl_FragColor = texture2D(qt_Texture0, qt_TexCoord0.st);
}

*/
