#version 330 core
out vec2 vTex;

uniform vec2 offset;

//@
vec2 textures[6] = vec2[]
(
	vec2(0.0, 0.0),
	vec2(1.0, 0.0),
	vec2(1.0, 1.0),

	vec2(1.0, 1.0),
    vec2(0.0, 1.0),
	vec2(0.0, 0.0)//,
);

vec2 positions[6] = vec2[]
(
	vec2(-1.0,  1.0),
	vec2( 1.0,  1.0),
	vec2( 1.0, -1.0),

	vec2( 1.0, -1.0),
    vec2(-1.0, -1.0),
	vec2(-1.0,  1.0)//,
);

void main()
{
    vTex = textures[gl_VertexID]; 
    
	gl_Position = vec4(positions[gl_VertexID] + offset, 0.0, 1.0);
}