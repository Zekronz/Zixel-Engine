#version 330 core
layout (location = 0) in vec3 aPos;

out vec2 vTex;

uniform mat4 matModel;
uniform mat4 matProj;
uniform vec4 quadPos;
uniform vec4 atlasUV;

void main(){

    gl_Position = matProj * matModel * vec4(quadPos.xy + (aPos.xy * quadPos.zw), 0.0, 1.0);
    vTex = atlasUV.xy + (aPos.xy * atlasUV.zw);
    
}