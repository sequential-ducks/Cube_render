#version 330 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexColour;
layout (location = 2) in vec2 texCoord;

out vec3 colour;
out vec2 TexCoord;

void main()
{
    //colour = vertexColour;
    gl_Position = vec4(vertexPosition, 1.0);
    TexCoord = texCoord;
}