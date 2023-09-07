#version 330 core

out vec4 FragColor;

in vec2 vUV;
in vec2 vSize;

uniform sampler2D diffuseSampler;
uniform bool hasTexture;
uniform vec4 blend;

uniform vec4 rgba;

void main(){

    float checkerSize = 8.0;
    vec3 col1 = vec3(0.749);
    vec3 col2 = vec3(1.0);

    vec2 pixel = (vUV * vSize);
    int ind = int(floor(pixel.x / checkerSize) + floor(pixel.y / checkerSize)) % 2;
    vec3 curCol = (ind == 0) ? col1 : col2;

    FragColor = vec4(mix(curCol, rgba.rgb, 1.0 - (vUV.y * (1.0 - rgba.a))), 1.0);

}