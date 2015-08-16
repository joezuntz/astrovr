#version 330 core
  
layout (location = 0) in vec4 pixels; // position and 
layout (location = 1) in vec4 color; // position and 
out vec4 color_; // Specify a color output to the fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * view * model * vec4(pixels.xyz, 1.0f);
    color_ = color;
    // Choose colour based on pixel locations

}
