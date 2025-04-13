#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    
    // Return 80% of texture 1 and 20% of texture 2
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}