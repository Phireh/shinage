#version 150  

in vec3 position;
in vec3 normal;
in vec2 texCoords;

out vec3 fPos;
out vec3 fColor;
out vec3 lightViewPos;
out vec3 transformedNormal;

uniform mat4 modelMatrix;  
uniform mat4 viewMatrix;  
uniform mat4 projMatrix;
uniform vec3 lightWorldPos;
uniform sampler2D tex;

void main()
{ 
    gl_Position = projMatrix*viewMatrix*modelMatrix*vec4(position, 1.0);
    fColor = vec3(texture(tex, texCoords));
    fPos = vec3(viewMatrix*modelMatrix*vec4(position, 1.0));
    lightViewPos = vec3(viewMatrix * vec4(lightWorldPos, 1.0));
    // TODO: This is kinda expensive, optimize later
    mat3 normalMatrix = mat3(transpose(inverse(viewMatrix * modelMatrix)));
    transformedNormal = normalMatrix * normal;
}
