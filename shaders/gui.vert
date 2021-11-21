#version 150

in vec2 texCoords;
in vec3 position;

out vec4 fColor;

uniform sampler2D tex;
uniform float alpha;

void main()
{
    gl_Position = vec4(position, 1.0);
    vec4 rawColor = texture(tex, texCoords);
    fColor = vec4(rawColor.rgb, rawColor.a * alpha);
}
