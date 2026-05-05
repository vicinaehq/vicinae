#version 440

layout(location = 0) in vec4 qt_VertexPosition;
layout(location = 1) in vec2 qt_VertexTexCoord;

layout(location = 0) out vec2 v_uv;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    vec4 u_color;
    vec4 u_backgroundColor;
    vec4 u_secondaryColor;
    vec4 u_borderColor;
    vec2 u_itemSize;
    float u_radius;
    float u_borderWidth;
    float u_secondaryHeight;
    int u_mode;
};

void main() {
    v_uv = qt_VertexTexCoord;
    gl_Position = qt_Matrix * qt_VertexPosition;
}
