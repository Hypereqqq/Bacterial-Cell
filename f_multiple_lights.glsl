#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    float shininess;
}; 

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
};

struct SpotLight {
    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
};

#define NR_POINT_LIGHTS 2

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform int textureOn;
uniform vec3 color;

// prototypy funkcji
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec3 texCol1;

void main()
{
	if (textureOn == 1) // 1 - texture, 0 - color
	{
		texCol1 = texture(material.diffuse, TexCoords).rgb; 
	}
	else if (textureOn == 0)
	{
		texCol1 = color;
	}
	else
	{
		FragColor = texture(material.diffuse, TexCoords);
		return;
	}

    // wlasciwosci
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

	// swiatlo punktowe
	vec3 result = vec3(0.0);
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir); 
	
    // swiatlo reflektorowe
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
    FragColor = vec4(result, 1.0);
}

// obliczanie koloru swiatla punktowego
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // obliczanie swiatla rozproszonego
    float diff = max(dot(normal, lightDir), 0.0);

    // obliczanie t³umienia
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  
    
    // laczenie wynikow
    vec3 ambient = light.ambient * texCol1;
    vec3 diffuse = light.diffuse * diff * texCol1;
    ambient *= attenuation;
    diffuse *= attenuation;
    return (ambient + diffuse);
}

// obliczanie koloru swiatla reflektorowego
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // obliczanie swiatla rozproszonego
    float diff = max(dot(normal, lightDir), 0.0);

    // obliczanie t³umienia
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));   
    
    // obliczanie intensywnosci swiatla
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // laczenie wynikow
    vec3 ambient = light.ambient * texCol1;
    vec3 diffuse = light.diffuse * diff * texCol1;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    return (ambient + diffuse);
}