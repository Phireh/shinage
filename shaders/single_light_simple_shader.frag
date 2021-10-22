#version 150 

in vec3 fPos;
in vec3 fColor;
in vec3 lightViewPos;
in vec3 transformedNormal;

uniform vec3 lightColor;

out vec4 out_color;

void main()
{
    /* Ambient component of Phong lighting */
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    /* Diffuse component of Phong lighting */
    vec3 norm = normalize(transformedNormal);
    vec3 lightDir = normalize(lightViewPos - fPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    /* Specular component of Phong lighting */
    float specularStrength = 0.5;
    // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
    vec3 viewDir = normalize(-fPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor; 
    
    out_color = vec4(fColor * (ambient + diffuse + specular), 1.0);
}
