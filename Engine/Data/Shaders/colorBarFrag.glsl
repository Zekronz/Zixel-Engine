#version 330 core

out vec4 FragColor;

in vec2 vUV;
in vec2 vSize;

uniform sampler2D diffuseSampler;
uniform bool hasTexture;
uniform vec4 blend;

uniform int colorMode;
uniform int channel;
uniform vec3 color;

vec3 hsv2rgb(vec3 color){

    vec4 k = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(color.xxx + k.xyz) * 6.0 - k.www);
    return color.z * mix(k.xxx, clamp(p - k.xxx, 0.0, 1.0), color.y);
    
}

void main(){
    
    if(channel == 0){

        //Hue.
        if(colorMode == 0){
            FragColor = vec4(hsv2rgb(vec3(vUV.x, 1.0, 1.0)), 1.0);
        //Red.
        }else{
            FragColor = vec4(mix(vec3(0.0, color.yz), vec3(1.0, color.yz), vUV.x), 1.0);
        }

    }
    else if(channel == 1){

        //Saturation.
        if(colorMode == 0){
            FragColor = vec4(mix(hsv2rgb(vec3(color.x, 0.0, color.z)), hsv2rgb(vec3(color.x, 1.0, color.z)), vUV.x), 1.0);
        //Green.
        }else{
            FragColor = vec4(mix(vec3(color.x, 0.0, color.z), vec3(color.x, 1.0, color.z), vUV.x), 1.0);
        }

    }
    else if(channel == 2){

        //Value.
        if(colorMode == 0){
            FragColor = vec4(mix(hsv2rgb(vec3(color.xy, 0.0)), hsv2rgb(vec3(color.xy, 1.0)), vUV.x), 1.0);
        //Blue.
        }else{
            FragColor = vec4(mix(vec3(color.xy, 0.0), vec3(color.xy, 1.0), vUV.x), 1.0);
        }

    }
    //Alpha.
    else if(channel == 3){

        float checkerSize = 8.0;
        vec3 col1 = vec3(0.749);
        vec3 col2 = vec3(1.0);

        vec2 pixel = (vUV * vSize);
        int ind = int(floor(pixel.x / checkerSize) + floor(pixel.y / checkerSize)) % 2;
        vec3 curCol = (ind == 0) ? col1 : col2;

        //HSV.
        if(colorMode == 0){
            FragColor = vec4(mix(curCol, hsv2rgb(color), vUV.x), 1.0);
        }
        //RGB.
        else{
            FragColor = vec4(mix(curCol, color, vUV.x), 1.0);
        }

    }
    else{
        discard;
    }

}