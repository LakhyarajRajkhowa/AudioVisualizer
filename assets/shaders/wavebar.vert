#version 330 core

layout(location = 0) in vec2  aPos;
layout(location = 1) in float aBarIndex;
layout(location = 2) in float aAmplitude;

uniform mat4  uProjection;
uniform int   uNumBars;
uniform float uTime;
uniform float uBass;

// Aesthetic Control Uniforms
uniform float uBarAspect;
uniform float uMaxBarHeight;  // Base scaling
uniform float uMaxCap;        // HARD CEILING (The "Cut-off") 
uniform float uBassBoost;
uniform float uPulseSpeed;
uniform float uPulseSpread;   // New: Spreads the pulse across bars
uniform float uYOffset;
uniform float uTotalWidth;

out float vAmplitude;
out float vNormIndex;
out float vLocalY;      

void main()
{
    float n = float(uNumBars);
    float normIdx = aBarIndex / (n - 1.0); 
    
    float barSlot = uTotalWidth / n; 
    float barWidth = barSlot * (1.0 - uBarAspect); 

    // EVEN SPREAD: Adding (normIdx * uPulseSpread) offsets the sine wave
    // so the pulse travels across the bars instead of hitting them all at once.
    float pulse = 1.0 + uBass * uBassBoost * sin(uTime * uPulseSpeed + (normIdx * uPulseSpread));
    
    // 1. Calculate desired height 
    float barHeight = aAmplitude * uMaxBarHeight * pulse;
    
    // 2. APPLY CAP: This "cuts" the bars if they exceed the limit
    barHeight = min(barHeight, uMaxCap);
    
    // Ensure a tiny minimum so they don't disappear completely 
    barHeight = max(barHeight, 0.005);

    vec2 worldPos;
    worldPos.x = (-uTotalWidth / 2.0) + barSlot * (aBarIndex + 0.5) + aPos.x * barWidth; 
    
    // Baseline growth 
    worldPos.y = uYOffset + (aPos.y * barHeight);

    vAmplitude = aAmplitude;
    vNormIndex = normIdx;
    vLocalY = aPos.y; 

    gl_Position = uProjection * vec4(worldPos, 0.0, 1.0); 
}