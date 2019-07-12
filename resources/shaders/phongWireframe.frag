#version 330
in vec3 vFragPos;
in vec4 vColor;
in vec3 vNormal;
in vec3 vBC;

out vec4 fColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct PointLight
{
    vec3 position;
    vec3 color;
};

uniform vec3 wireFrameColor = vec3(0,0,0);
uniform vec4 overlayColor = vec4(0,0,0,0);

uniform vec3 viewPos;

uniform int numPointLights;

#define NR_MAX_LIGHTS       5

uniform Material mMaterial;
uniform PointLight PointLights[NR_MAX_LIGHTS];



void main()
{

    vec4 pColor = vec4(0,0,0,0);
    for(int i = 0; i < numPointLights; i++)
    {
        vec3 lightDir = normalize(PointLights[i].position - vFragPos);
        vec3 normal = normalize(vNormal);

        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * mMaterial.diffuse * PointLights[i].color;

        float specularStrength = 0.5;
        vec3 viewDir = normalize(viewPos - vFragPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 20);
        vec3 specular = specularStrength * spec * PointLights[i].color;

        vec3 ambient = PointLights[i].color * mMaterial.ambient;

//        fColor += vec4(diffuse + specular , 1.0);
        pColor += vec4(ambient + diffuse + specular, 1);
    }

    fColor = vec4(0.0, 0.0, 1.0, 1.0);

    if(any(lessThan(vBC, vec3(0.02))))
    {
//        fColor = vec4(0.2, 0.2, 0.2, 1.0);
//        fColor = vec4(0.0, 0.0, 0.0, 1.0);
          fColor = vec4(wireFrameColor, 1.0);

        // debug color value
//        fColor = vec4(vColor.z, 0.0, 0.0, 1.0);
    }
    else
    {
        fColor = pColor + overlayColor;
    }

}


// WIP sketch
/*
    for(p in pointLights)
        result += pResult

    for(d in directionalIghts)
        result += lResult




        ?? Working on Light Vector.
            how to send to shader (see Tutorial)
            how to splitt point lights and dir lights




*/

























































