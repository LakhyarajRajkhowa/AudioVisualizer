#version 330 core

layout(location = 0) in vec2  aPos;
layout(location = 1) in float aBarIndex;
layout(location = 2) in float aAmplitude;

uniform mat4  uProjection;
uniform int   uNumBars;
uniform float uTime;
uniform float uBass;

uniform float uTotalWidth;

out float vAmplitude;
out float vNormIndex;
out float vLocalY;     

const float yOffset = -1.0f;
const float barGap = 0.5f; 
const float maxHeight = 1.0f;


void main()
{
    float n = float(uNumBars);
    float normIdx = aBarIndex / (n - 1.0); 
    
    float barSlot = uTotalWidth / n; 
    float barWidth = barSlot * (1.0 - barGap); 
    
    float barHeight = aAmplitude * maxHeight;
    
    // Ensure a tiny minimum so they don't disappear completely 
    barHeight = max(barHeight, 0.005);

    vec2 worldPos;
    worldPos.x = (-uTotalWidth / 2.0) + barSlot * (aBarIndex + 0.5) + aPos.x * barWidth; 
    
    // Baseline growth 
    worldPos.y = yOffset + (aPos.y * barHeight);

    vAmplitude = aAmplitude;
    vNormIndex = normIdx;
    vLocalY = aPos.y; 

    gl_Position = uProjection * vec4(worldPos, 0.0, 1.0); 
}