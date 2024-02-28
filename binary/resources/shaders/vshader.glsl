#version 330 core

struct Light {
    vec3  position;
    vec3  direction;
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float innerCutOff;
    float outerCutOff;
    float c0;
    float c1;
    float c2;
};

struct Material {
    vec4  ambient;
    vec4  diffuse;
    vec4  specular;
    vec4  emissive;
    float shininess;
};

#define NLD 1

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;

uniform mat4     uN;
uniform mat4     uM;
uniform mat4     uPVM;
uniform vec3     ueye;
uniform Light    ulightG;
uniform Light    ulightD[NLD];
uniform Material umaterial;

out vec4 vColor;

vec3 funDirectional(Light light, Material material, vec3 N, vec3 V);

void main() {

    vec4 inPositionH = vec4(inPosition,1);
    vec4 inNormalH   = vec4(inNormal  ,0);

    gl_Position = uPVM * inPositionH;

    vec3 N = normalize((uN*inNormalH).xyz);
    vec3 V = normalize(ueye - (uM*inPositionH).xyz);

    vec3 color = umaterial.emissive.rgb + ulightG.ambient * umaterial.ambient.rgb;
    for(int i=0; i<NLD; i++) color += funDirectional(ulightD[i],umaterial,N,V);

    vColor = vec4(color, umaterial.diffuse.a);

}

vec3 funDirectional(Light light, Material material, vec3 N, vec3 V) {

    vec3  L = normalize(-light.direction);
    vec3  R = normalize(reflect(-L,N));

    float dotLN = dot(L,N);
    float dotRV = 0.0;
    if(dotLN<0.0) dotLN = 0.0;
    else          dotRV = max(dot(R,V), 0.0);
    
    vec3  ambient  = light.ambient  * material.ambient.rgb;
    vec3  diffuse  = light.diffuse  * material.diffuse.rgb  * dotLN;
    vec3  specular = light.specular * material.specular.rgb * pow(dotRV,material.shininess);

    vec3 color = ambient + diffuse + specular;

    return color;

}
