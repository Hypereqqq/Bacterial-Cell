#version 330 core

layout (location = 0) in vec3 vert; // v
layout (location = 1) in vec2 texCoord; // vt
layout (location = 2) in vec3 norm; // vn

out vec3 FragPos;                   
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main()
{
    FragPos = vec3(M * vec4(vert, 1.0));
    Normal = mat3(transpose(inverse(M))) * norm;  
    TexCoords = texCoord;
    
    gl_Position = P * V * vec4(FragPos, 1.0);
}