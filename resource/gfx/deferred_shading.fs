#version 330 core
in vec2 vTex;

out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light 
{
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
};
const int NR_LIGHTS = 128;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos   = texture(gPosition, vTex).rgb;
    vec3 Normal    = texture(gNormal, vTex).rgb;
    vec3 Diffuse   = texture(gAlbedoSpec, vTex).rgb;
    float Specular = texture(gAlbedoSpec, vTex).a;
    
    // then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.4; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
        // specular
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = lights[i].Color * spec * Specular;
        // attenuation
        float distance = length(lights[i].Position - FragPos);
        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
        distance *= 5000;
        lighting += diffuse  * attenuation;   
        lighting += specular * attenuation;       
    }
    FragColor = vec4(lighting, 1.0);

    //FragColor = texture(gAlbedoSpec, vTex);
}
