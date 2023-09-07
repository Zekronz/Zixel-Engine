#version 330 core
out vec4 FragColor;

in vec2 vTex;

uniform sampler2D diffuseSampler;
uniform bool hasTexture;
uniform vec4 blend;

void main(){

    if(hasTexture){

        FragColor = texture(diffuseSampler, vTex) * blend;

    }else{

        FragColor = blend;

    }

}