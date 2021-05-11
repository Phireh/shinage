#version 150 

in vec4 vertex; 

out vec2 TexCoords; 

uniform mat4 projMatrix; 
    
void main()
{ 
    gl_Position = projMatrix * vec4(vertex.xy, 0.0, 1.0); 
    TexCoords = vertex.zw; 
}