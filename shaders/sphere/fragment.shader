#version 330 core

in vec2 textureCoords;
out vec4 color;
uniform sampler2D sphereTexture;

void main()
{
    color = texture(sphereTexture, textureCoords);
} 