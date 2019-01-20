#version 330 core 
  
layout(location = 0) in vec3 vVertex; // 渲染覆盖整个视口的正方形

smooth out vec2 vUV;					
 
void main()
{  
	vUV = vVertex.xy;	
	gl_Position = vec4(vVertex.xyz, 1);
}