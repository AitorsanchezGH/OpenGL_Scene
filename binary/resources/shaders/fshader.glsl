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

uniform Light    ulightG;
uniform Light    ulightD[NLD];
uniform Material umaterial;
uniform vec3     ueye;

in  vec3 vnor;
in  vec3 vpos;

out vec4 outColor;

vec3 funDirectional(Light light, Material material, vec3 N, vec3 V);

void main() {

    vec3 N = normalize(vnor);
    vec3 V = normalize(ueye - vpos);

    vec3 color = umaterial.emissive.rgb + ulightG.ambient * umaterial.ambient.rgb;
    for(int i=0; i<NLD; i++) color += funDirectional(ulightD[i],umaterial,N,V);

    outColor = vec4(color, umaterial.diffuse.a);

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
