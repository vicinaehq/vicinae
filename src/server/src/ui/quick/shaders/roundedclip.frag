#version 440

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float cornerRadius;
    vec2 itemSize;
};
layout(binding = 1) uniform sampler2D source;

void main() {
    vec2 halfSize = itemSize * 0.5;
    float radius = min(cornerRadius, min(halfSize.x, halfSize.y));
    vec2 edge = abs(qt_TexCoord0 * itemSize - halfSize) - halfSize + radius;
    float distance = length(max(edge, 0.0)) + min(max(edge.x, edge.y), 0.0) - radius;
    float pixelWidth = max(fwidth(distance), 0.001);
    float coverage = 1.0 - smoothstep(-pixelWidth * 0.5, pixelWidth * 0.5, distance);
    fragColor = texture(source, qt_TexCoord0) * (qt_Opacity * coverage);
}
