#version 330 core
layout (location = 0) in vec4 aPos;

out vec2 vUV;
out vec2 vSize;

uniform mat4 matModel;
uniform mat4 matProj;
uniform vec4 quadPos;
uniform vec4 atlasUV;

void main(){

    gl_Position = matProj * matModel * vec4(quadPos.xy + (aPos.xy * quadPos.zw), 0.0, 1.0);

    vUV = aPos.xy;
    vSize = quadPos.zw;
    
}