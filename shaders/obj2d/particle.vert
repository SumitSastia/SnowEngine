#version 450 core

layout (location = 0) in vec2 aPos;

uniform mat4 finalMatrix;

void main(){
    gl_Position = finalMatrix * vec4(aPos, 0.0, 1.0);
}
