#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uMVP;

uniform float uBass;
uniform float uMid;
uniform float uTreble;
uniform float uTime;

/* ===== Tunable parameters ===== */

uniform float uBassWeight;
uniform float uMidWeight;
uniform float uTrebleWeight;

uniform float uWaveFrequency;
uniform float uDisplacementScale;

out float intensity;

void main()
{
    vec3 pos = aPos;

    float audio =
        uBass * uBassWeight +
        uMid * uMidWeight +
        uTreble * uTrebleWeight;

    float wave =
        sin(pos.x * uWaveFrequency + uTime) *
        cos(pos.y * uWaveFrequency + uTime);

    float displacement = audio * wave * uDisplacementScale;

    pos += aNormal * displacement;

    intensity = displacement;

    gl_Position = uMVP * vec4(pos,1.0);
}