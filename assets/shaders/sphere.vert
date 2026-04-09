#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uMVP;

uniform float uBass;
uniform float uMid;
uniform float uTreble;
uniform float uTime;

out float intensity;

void main()
{
    vec3 pos = aPos;

    float audio = uBass * 1.5 + uMid + uTreble * 0.5;

    float wave = sin(pos.x * 4.0 + uTime) *
                 cos(pos.y * 4.0 + uTime);

    float displacement = audio * wave * 0.02;

    pos += aNormal * displacement;

    intensity = displacement;

    gl_Position = uMVP * vec4(pos,1.0);
}