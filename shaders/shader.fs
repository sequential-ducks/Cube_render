#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    
    float weight = 1.2 * sin(TexCoord.y * 2.87) - sin(TexCoord.x * 0.28); // Example dynamic weight
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), weight);
}