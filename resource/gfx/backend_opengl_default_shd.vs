#version 330 core
layout (location = 0) in vec3 mPos;
layout (location = 1) in vec2 mTex;

out vec2 vTex;

void main()
{
    vTex = mTex;    
    gl_Position = vec4(mPos, 1.0);
}