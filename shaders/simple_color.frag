#version 150 

in vec3 fColor; 

out vec4 out_color; 

void main()
{ 
    out_color = vec4(fColor, 1.0); 
}