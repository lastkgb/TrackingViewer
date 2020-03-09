#version 330 core
in vec2 UV;

out vec3 color;

uniform sampler2D groundSampler;

void main()
{
    color = texture(groundSampler, UV).rgb;
}