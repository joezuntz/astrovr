#version 330 core
  
layout (location = 0) in vec3 pixels; // position and 

void main(){
    gl_Position = vec4(pixels.xyz, 1.0f);
}
