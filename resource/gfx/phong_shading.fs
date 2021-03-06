#version 330 core
in vec3 vPos;
in vec3 vNor;
in vec2 vTex;
out vec4 FragColor;

uniform sampler2D Diffuse;
uniform sampler2D Specular;

uniform vec3 vLightPos;
uniform vec3 Lightcolor;

// strength
uniform bool blinn_phong;
uniform float sa;
uniform float sd;
uniform float ss;
uniform int shininess;

void main()
{    
    // ambient
    float ka = 1.0;
    vec4 ambi = ka * sa * texture(Diffuse, vTex) * vec4(Lightcolor, 1.0);

    // diff
    vec3 normal = normalize(vNor);
    vec3 lightdir = normalize(vLightPos - vPos);
    float kd = max(dot(normal, lightdir), 0.0);
    vec4 diff = kd * sd * texture(Diffuse, vTex) * vec4(Lightcolor, 1.0);

    // spec
    vec3 viewdir = normalize(-vPos);
    vec3 halfwaydir, reflectdir;
    float ks;
    if(blinn_phong)
    {
        halfwaydir = normalize(lightdir + viewdir);
        ks = pow(max(dot(normal, halfwaydir), 0.0), shininess);
    }
    else
    {
        reflectdir = reflect(-lightdir, normal);
        ks = pow(max(dot(viewdir, reflectdir), 0.0), shininess);
    }
    vec4 spec = ks * ss * texture(Specular, vTex) * vec4(Lightcolor, 1.0);

    FragColor = ambi + diff + spec;
}