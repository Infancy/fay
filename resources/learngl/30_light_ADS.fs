#version 330 core
in vec3 vPos;
in vec3 vNor;
in vec2 vTex;

out vec4 FragColor;

uniform sampler2D diff;
uniform sampler2D spec;

uniform vec3 vLightPos;
uniform vec3 lightcolor;

// strength
uniform float sa;
uniform float sd;
uniform float ss;
uniform int shininess;

void main()
{    
    // ambient
    float ka = 1.0;
    vec4 ambient = ka * sa * texture(diff, vTex) * vec4(lightcolor, 1.0);

    // diff
    vec3 normal = normalize(vNor);
    vec3 lightdir = normalize(vLightPos - vPos);
    float kd = max(dot(normal, lightdir), 0.0);
    vec4 diffuse = kd * sd * texture(diff, vTex) * vec4(lightcolor, 1.0);

    // spec
    vec3 viewdir = normalize(-vPos);
    vec3 reflectdir = reflect(-lightdir, normal);
    float ks = pow(max(dot(viewdir, reflectdir), 0.0), shininess);
    vec4 specular = ks * ss * texture(spec, vTex) * vec4(lightcolor, 1.0);

    FragColor = ambient + diffuse + specular;
}