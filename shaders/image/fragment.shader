#version 330 core

in vec3 color;
in vec2 texcoord;
out vec4 outputColor;

uniform sampler2D tex;

void main()
{
    outputColor = texture(tex, texcoord) * vec4(color, 1.0);
//    outputColor = vec4(color, 1.0);

} 