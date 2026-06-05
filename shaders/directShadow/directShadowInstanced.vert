#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 3) in mat4 instanceModel;

uniform mat4 lightSpace;

void main(){
    gl_Position = lightSpace * instanceModel * vec4(aPos, 1.0);
}
