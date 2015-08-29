#version 330 core
  

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 col;
layout (location = 2) in vec2 tex;

out vec3 color;
out vec2 texcoord;

uniform mat4 projection;

void main(){
	// Compute the position of this pixel
    gl_Position = projection*vec4(position.xy, -0.5f, 1.0f);

    // Pass the color and texture coordinates
    color=col;
    texcoord = tex;

}
