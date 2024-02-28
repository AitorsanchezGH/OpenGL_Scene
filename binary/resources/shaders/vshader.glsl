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

layout (location = 0) in vec3 inPosition;

uniform mat4     uPVM;
uniform Light    ulightG;
uniform Material umaterial;

out vec4 vColor;

void main() {

    vec4 inPositionH = vec4(inPosition,1);

    gl_Position = uPVM * inPositionH;

    vec3 color = umaterial.emissive.rgb + ulightG.ambient * umaterial.ambient.rgb;

    vColor = vec4(color, umaterial.diffuse.a);

}
