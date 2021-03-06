#version 330 core
layout (location = 0) in vec3 mPos;
//layout (location = 1) in vec3 mNor;
//layout (location = 2) in vec2 mTex;

out vec3 vTex;

uniform mat4 MVP;

void main()
{
    vTex = mPos;    
    gl_Position = (MVP * vec4(mPos, 1.0)).xyww;
}