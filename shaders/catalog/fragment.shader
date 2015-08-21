#version 330 core

in float point_radius;
in vec4 color_;
out vec4 col2;

void main()
{
	vec2 r = gl_PointCoord - vec2(0.5);
	if (length(r)>0.5) discard;
	col2 = color_;

} 