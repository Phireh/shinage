#version 150  

in vec3 position; 
in vec3 vColor;
in vec3 normal;

out vec3 fPos;
out vec3 fColor;
out vec3 lightViewPos;
out vec3 transformedNormal;

uniform mat4 modelMatrix;  
uniform mat4 viewMatrix;  
uniform mat4 projMatrix;
uniform vec3 lightWorldPos;

void main()
{ 
    gl_Position = projMatrix*viewMatrix*modelMatrix*vec4(position, 1.0);
    fColor = vColor;
    fPos = viewMatrix*modelMatrix*vec4(position, 1.0);
    lightViewPos = vec3(view * vec4(lightWorldPos, 1.0));
    // TODO: This is kinda expensive, optimize later
    mat3 normalMatrix = mat3(transpose(inverse(viewMatrix * modelMatrix)));
    transformedNormal = normalMatrix * normal;
}
