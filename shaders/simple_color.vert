#version 150  

in vec3 position; 
in vec3 vColor; 

out vec3 fColor; 

uniform mat4 modelMatrix;  
uniform mat4 viewMatrix;  
uniform mat4 projMatrix;  

void main()
{ 
    gl_Position = projMatrix*viewMatrix*modelMatrix*vec4(position, 1.0); 
    fColor = vColor; 
}