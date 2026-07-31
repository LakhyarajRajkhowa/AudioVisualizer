#version 330 core

// ─────────────────────────────────────────────────────────────────────────────
//  ring.frag
//
//  Colours each arc fragment using the classic heat palette:
//    green → yellow → orange → red
//  based on the ring's radial position + amplitude (uHeatT).
//
//  Additive blending is set by the renderer, so we output pre-multiplied alpha.
//
//  The arc fade at each end gives the clean gap between arcs.
// ─────────────────────────────────────────────────────────────────────────────

in float vT;           // [0..1] along the arc
in float vAmplitude;
in float vNormT;       // [0..1] ring position (inner=0, outer=1)

uniform float uHeatT;      // [0..1] colour blend position
uniform float uArcFrac;    // fraction of circle that is arc
uniform float uAmplitude;
uniform float uTime;
uniform float uNormT;

out vec4 fragColor;

// ── Heat palette: green → yellow → orange → red ───────────────────────────────
vec3 heatColor(float t)
{
    t = clamp(t, 0.0, 1.0);

    vec3 c0 = vec3(0.05, 1.00, 0.10);  // bright green   t=0.00
    vec3 c1 = vec3(0.70, 1.00, 0.00);  // yellow-green   t=0.25
    vec3 c2 = vec3(1.00, 0.55, 0.00);  // orange         t=0.60
    vec3 c3 = vec3(1.00, 0.10, 0.00);  // red            t=1.00

    if (t < 0.25) return mix(c0, c1, t / 0.25);
    if (t < 0.60) return mix(c1, c2, (t - 0.25) / 0.35);
                  return mix(c2, c3, (t - 0.60) / 0.40);
}

void main()
{
    // ── Arc end fade ──────────────────────────────────────────────────────────
    // Fade alpha to 0 near the start and end of the arc so there's no hard cut.
    // The arc spans vT in [0, uArcFrac].
    float fadeLen = 0.04;  // fraction of arc used for fade in/out

    float arcAlpha;
    if (vT > uArcFrac) {
        // Beyond the drawn arc – discard (shouldn't reach here due to drawCount,
        // but keeps GL_LINE_STRIP clean on the last segment)
        discard;
    } else {
        float fadeIn  = smoothstep(0.0,       fadeLen,              vT);
        float fadeOut = smoothstep(uArcFrac,  uArcFrac - fadeLen,   vT);
        arcAlpha      = fadeIn * fadeOut;
    }

    // ── Colour ────────────────────────────────────────────────────────────────
    vec3 col = heatColor(uHeatT);

    // Hot core: mix toward white-yellow at high amplitude
    vec3 hotCore = mix(col, vec3(1.0, 1.0, 0.6), uAmplitude * 0.45);

    // ── Intensity ─────────────────────────────────────────────────────────────
    // Minimum glow even at silence; brighter when loud
    float intensity = 0.25 + uAmplitude * 1.8;
    intensity = min(intensity, 2.5);

    // Subtle flicker along the arc (mimics the raster/scan artefact in the original)
    float flicker = 0.92 + 0.08 * sin(vT * 180.0 + uTime * 8.0 + uNormT * 40.0);

    float alpha = arcAlpha * intensity * flicker;
    alpha = clamp(alpha, 0.0, 1.0);

    // Pre-multiply for additive blend
    fragColor = vec4(hotCore * alpha, alpha);
}
