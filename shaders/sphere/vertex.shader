#version 330 core
  
layout (location = 0) in vec3  pixels;
layout (location = 1) in vec2 textureCoords_;
out vec2 textureCoords;

uniform mat4 projection;

void main(){
    gl_Position = projection * vec4(pixels.xyz, 1.0f);
    textureCoords = textureCoords_;
}
