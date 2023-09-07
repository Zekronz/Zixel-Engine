#version 330 core

out vec4 FragColor;

in vec2 vUV;
in vec2 vSize;

uniform sampler2D diffuseSampler;
uniform bool hasTexture;
uniform vec4 blend;

uniform float checkerSize;

void main(){

    vec3 col1 = vec3(0.749);
    vec3 col2 = vec3(1.0);

    vec2 pixel = (vUV * vSize);
    int ind = int(floor(pixel.x / checkerSize) + floor(pixel.y / checkerSize)) % 2;
    vec3 curCol = (ind == 0) ? col1 : col2;

    FragColor = vec4(curCol, 1.0);

}