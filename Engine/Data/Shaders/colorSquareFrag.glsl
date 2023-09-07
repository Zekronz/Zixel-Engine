#version 330 core

out vec4 FragColor;

in vec2 vUV;
in vec2 vSize;

uniform sampler2D diffuseSampler;
uniform bool hasTexture;
uniform vec4 blend;

uniform float squareHue;

vec3 hsv2rgb(vec3 color){

    vec4 k = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(color.xxx + k.xyz) * 6.0 - k.www);
    return color.z * mix(k.xxx, clamp(p - k.xxx, 0.0, 1.0), color.y);
    
}

void main(){
    
    float value = vUV.y;
    float sat = vUV.x;

    FragColor = vec4(mix(mix(vec3(1.0), hsv2rgb(vec3(squareHue, 1.0, 1.0)), sat), vec3(0.0), value), 1.0);

}