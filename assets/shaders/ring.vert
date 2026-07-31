#version 330 core

// ─────────────────────────────────────────────────────────────────────────────
//  ring.vert
//
//  Takes unit-circle geometry and transforms it into a ring at the correct
//  radius with the correct rotation for this draw call.
//
//  Per-vertex:
//    location 0  vec2  aPos   – unit circle (cos θ, sin θ)
//    location 1  float aT     – arc parameter [0..1]
// ─────────────────────────────────────────────────────────────────────────────

layout(location = 0) in vec2  aPos;
layout(location = 1) in float aT;

uniform mat4  uProjection;
uniform float uRadius;      // world-space radius of this ring
uniform float uRotation;    // rotation offset in radians
uniform float uArcFrac;     // fraction of circle drawn (for edge fade)
uniform float uAmplitude;   // [0..1] audio amplitude of this ring
uniform float uNormT;       // [0..1] radial position (0=inner,1=outer)
uniform float uTime;

out float vT;          // arc parameter passed to frag
out float vAmplitude;
out float vNormT;

void main()
{
    // Rotate the unit circle vertex by uRotation
    float cosR = cos(uRotation);
    float sinR = sin(uRotation);

    vec2 rotated;
    rotated.x = aPos.x * cosR - aPos.y * sinR;
    rotated.y = aPos.x * sinR + aPos.y * cosR;

    // Scale to ring radius
    vec2 worldPos = rotated * uRadius;

    // Very subtle radius oscillation for life (like the original's wobble)
    float wobble = 1.0 + 0.012 * sin(uTime * 3.7 + uNormT * 15.0 + aT * 6.28);
    worldPos *= wobble;

    vT         = aT;
    vAmplitude = uAmplitude;
    vNormT     = uNormT;

    gl_Position = uProjection * vec4(worldPos, 0.0, 1.0);

    // Point size for GL_POINTS fallback (not used with GL_LINE_STRIP)
    gl_PointSize = 2.0;
}
