#version 330 core

// ─────────────────────────────────────────────────────────────────────────────
//  wavebar.frag
//  Holographic glow aesthetic:
//    • Frequency-mapped colour  cyan ──► violet ──► magenta
//    • Vertical fade: bright tip, darker root
//    • Horizontal soft-edge: bars feather at left/right edges
//    • Animated scanline shimmer for the VLC "hologram" look
//    • Additive blending is set by the renderer – no alpha premult needed
// ─────────────────────────────────────────────────────────────────────────────

in float vAmplitude;
in float vNormIndex;   // [0..1] bar position in array
in float vLocalY;      // [0..1] bottom → tip of bar

uniform float uTime;
uniform float uBass;
uniform float uMid;
uniform float uTreble;

out vec4 fragColor;

// ── Helpers ───────────────────────────────────────────────────────────────────

// Simple hue-based palette  (no external texture needed)
// Maps t in [0,1] through cyan → blue → violet → magenta → pink
vec3 holoColor(float t)
{
    // Control points in RGB
    vec3 c0 = vec3(0.00, 1.00, 1.00);   // cyan      t=0.00
    vec3 c1 = vec3(0.10, 0.50, 1.00);   // sky blue  t=0.25
    vec3 c2 = vec3(0.50, 0.10, 1.00);   // violet    t=0.50
    vec3 c3 = vec3(1.00, 0.05, 0.80);   // magenta   t=0.75
    vec3 c4 = vec3(1.00, 0.40, 0.20);   // orange    t=1.00

    t = clamp(t, 0.0, 1.0);
    if (t < 0.25) return mix(c0, c1, t / 0.25);
    if (t < 0.50) return mix(c1, c2, (t - 0.25) / 0.25);
    if (t < 0.75) return mix(c2, c3, (t - 0.50) / 0.25);
                  return mix(c3, c4, (t - 0.75) / 0.25);
}

void main()
{
    vec3 col = holoColor(vNormIndex);

    // Soften the vertical profile: bright tip, subtle fade to base
    float vertMask = pow(vLocalY, 0.5); 
    
    // Add a very slight horizontal softening to each bar
    float horizMask = 1.0 - pow(abs(vNormIndex - 0.5) * 2.0, 4.0); // Optional: global screen fade
    
    float shimmerSpeed = 1.0 + uBass * 0.5;
    float scanline = 0.85 + 0.15 * sin(vLocalY * 20.0 - uTime * shimmerSpeed * 5.0);

    float alpha = vertMask * scanline;
    // Multiply by a factor to make it "pop" with additive blending
    fragColor = vec4(col * alpha * 1.5, alpha);
}