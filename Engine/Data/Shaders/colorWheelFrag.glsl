#version 330 core

#define PI 3.1415926538

out vec4 FragColor;

in vec2 vUV;
in vec2 vSize;

uniform sampler2D diffuseSampler;
uniform bool hasTexture;
uniform vec4 blend;

uniform float wheelThickness;

vec3 hsv2rgb(vec3 color){

    vec4 k = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(color.xxx + k.xyz) * 6.0 - k.www);
    return color.z * mix(k.xxx, clamp(p - k.xxx, 0.0, 1.0), color.y);
    
}

void main(){
    
    float dist = distance(vUV * vSize, vec2(0.5) * vSize);
    float dir = ((atan(vUV.y - 0.5, vUV.x - 0.5) * 180.0 / PI) + 180.0) / 360.0;
    
    float radius = (vSize.x < vSize.y) ? vSize.x / 2.0 : vSize.y / 2.0;

    if(dist <= radius && dist >= radius - wheelThickness){
    
        float delta = 1.0;
        float outerAlpha = clamp(1.0 - ((dist - (radius - delta)) / delta), 0.0, 1.0);
        float innerAlpha = clamp((dist - (radius - wheelThickness + delta)) / delta, 0.0, 1.0);
        
        FragColor = vec4(hsv2rgb(vec3(dir, 1.0, 1.0)), min(outerAlpha, innerAlpha));
        
    }else{
        discard;
    }

}