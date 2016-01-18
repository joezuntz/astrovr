#version 330 core

in vec2 textureCoords;
out vec4 color;
uniform sampler2D sphereTexture;
uniform float alpha;
void main()
{
    color = texture(sphereTexture, textureCoords);
	color.a = alpha;
} 