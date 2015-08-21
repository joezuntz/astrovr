#version 330 core
  
layout (location = 0) in vec4 pixels; // point location 
layout (location = 1) in vec4 color; // point color
out vec4 color_; // Specify a color output to the fragment shader
out float point_radius;

//uniform mat4 model;
//uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * vec4(pixels.xyz, 1.0f);
	gl_PointSize = pixels.w;
    // Pass the color on from the CPU
    color_ = color;
    point_radius = pixels.w;

}
