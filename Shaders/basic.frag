#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

// Material
uniform bool useTexture;
uniform sampler2D diffuseTexture;
uniform vec3 solidColor;
uniform vec3 materialSpecular;
uniform float materialShininess;
uniform float alpha;

// Emissive (for glowing buttons)
uniform vec3 emissiveColor;
uniform float emissiveStrength;

// Camera
uniform vec3 viewPos;

// Lights
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    bool active;
};

#define MAX_LIGHTS 20
uniform int numLights;
uniform Light lights[MAX_LIGHTS];

vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffColor)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    // Ambient
    vec3 ambient = light.ambient * diffColor;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffColor;

    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), materialShininess);
    vec3 specular = light.specular * spec * materialSpecular;

    return (ambient + diffuse + specular) * attenuation;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Two-sided lighting: flip normal if facing away from camera
    if (dot(norm, viewDir) < 0.0)
        norm = -norm;

    vec3 diffColor;
    if (useTexture)
        diffColor = texture(diffuseTexture, TexCoord).rgb;
    else
        diffColor = solidColor;

    // Global ambient so nothing is fully black
    vec3 result = vec3(0.08) * diffColor;

    for (int i = 0; i < numLights && i < MAX_LIGHTS; i++)
    {
        if (lights[i].active)
            result += CalcPointLight(lights[i], norm, FragPos, viewDir, diffColor);
    }

    // Add emissive
    result += emissiveColor * emissiveStrength;

    FragColor = vec4(result, alpha);
}
