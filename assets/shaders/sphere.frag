#version 330 core

in float intensity;
out vec4 FragColor;

void main()
{
    float c = 0.5 + intensity * 2.0;
    FragColor = vec4(vec3(c),1.0);
}