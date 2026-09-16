#version 440

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float topInset;
    float bottomInset;
    float contentHeight;
};
layout(binding = 1) uniform sampler2D source;

float fade(float from, float to, float position) {
    float t = clamp((position - from) / (to - from), 0.0, 1.0);
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

void main() {
    float y = qt_TexCoord0.y * contentHeight;
    float top = topInset > 0.0 ? fade(max(0.0, topInset - 40.0), topInset + 32.0, y) : 1.0;
    float bottom = bottomInset > 0.0
        ? 1.0 - fade(contentHeight - bottomInset - 36.0, contentHeight - bottomInset + 28.0, y)
        : 1.0;
    fragColor = texture(source, qt_TexCoord0) * (qt_Opacity * top * bottom);
}
