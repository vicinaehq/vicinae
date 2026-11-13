#version 440

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 fragColor;

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
    vec4 u_outerBorderColor;
    float u_outerBorderWidth;
    int u_gradientCount;
    vec2 u_gradientDir;
    vec4 u_gradientStops[8];
};

float roundedRectSDF(vec2 p, vec2 halfSize, float radius) {
    vec2 d = abs(p) - halfSize + radius;
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - radius;
}

// The loop bound is constant so that the shader stays valid for GLSL ES 1.00.
vec4 borderColorAt(vec2 pixelPos) {
    if (u_gradientCount < 2) return u_borderColor;

    vec2 center = u_itemSize * 0.5;
    float span = abs(u_itemSize.x * u_gradientDir.x) + abs(u_itemSize.y * u_gradientDir.y);
    float t = clamp(dot(pixelPos - center, u_gradientDir) / max(span, 1.0) + 0.5, 0.0, 1.0);
    float scaled = t * float(u_gradientCount - 1);
    vec4 color = u_gradientStops[0];

    for (int i = 1; i < 8; ++i) {
        if (i < u_gradientCount) {
            color = mix(color, u_gradientStops[i], clamp(scaled - float(i - 1), 0.0, 1.0));
        }
    }

    return color;
}

void main() {
    vec2 pixelPos = v_uv * u_itemSize;
    vec2 center = u_itemSize * 0.5;
    vec4 borderColor = borderColorAt(pixelPos);

    float dist = roundedRectSDF(pixelPos - center, center, u_radius);
    float shape = 1.0 - smoothstep(-0.5, 0.5, dist);

    if (u_mode == 1) {
        float outside = 1.0 - shape;
        if (outside < 0.004) discard;
        vec4 bg = u_backgroundColor;
        fragColor = vec4(bg.rgb * bg.a, bg.a) * qt_Opacity;
    } else if (u_mode == 2) {
        float ow = u_outerBorderColor.a > 0.0 ? u_outerBorderWidth : 0.0;
        float bw = borderColor.a > 0.0 ? u_borderWidth : 0.0;
        if (ow <= 0.0 && bw <= 0.0) {
            fragColor = vec4(0.0);
            return;
        }
        float outerShrunk = roundedRectSDF(pixelPos - center, center - ow, max(u_radius - ow, 0.0));
        float afterOuter = 1.0 - smoothstep(-0.5, 0.5, outerShrunk);
        float innerShrunk = roundedRectSDF(pixelPos - center, center - ow - bw, max(u_radius - ow - bw, 0.0));
        float innerShape = 1.0 - smoothstep(-0.5, 0.5, innerShrunk);
        vec4 oc = u_outerBorderColor;
        vec4 bc = borderColor;
        fragColor = (vec4(oc.rgb * oc.a, oc.a) * (shape - afterOuter)
                   + vec4(bc.rgb * bc.a, bc.a) * (afterOuter - innerShape)) * qt_Opacity;
    } else {
        vec4 fill = u_color;

        if (u_secondaryHeight > 0.0 && u_secondaryColor.a > 0.0) {
            float secondaryStart = u_itemSize.y - u_secondaryHeight;
            float inSecondary = step(secondaryStart, pixelPos.y);
            fill = mix(fill, u_secondaryColor, inSecondary);
        }

        if (u_backgroundColor.a > 0.0) {
            vec4 bg = u_backgroundColor;
            vec4 inner = fill;
            vec4 result;

            if (u_borderWidth > 0.0 && borderColor.a > 0.0) {
                float shrunk = roundedRectSDF(pixelPos - center, center - u_borderWidth, max(u_radius - u_borderWidth, 0.0));
                float innerShape = 1.0 - smoothstep(-0.5, 0.5, shrunk);
                float borderZone = clamp(shape - innerShape, 0.0, 1.0);
                vec4 bc = borderColor;
                vec4 shapeColor = mix(inner, bc, borderZone);
                result = mix(bg, shapeColor, shape);
            } else {
                result = mix(bg, inner, shape);
            }

            fragColor = vec4(result.rgb * result.a, result.a) * qt_Opacity;
        } else {
            vec4 sc = fill;

            if (u_borderWidth > 0.0 && borderColor.a > 0.0) {
                float shrunk = roundedRectSDF(pixelPos - center, center - u_borderWidth, max(u_radius - u_borderWidth, 0.0));
                float innerShape = 1.0 - smoothstep(-0.5, 0.5, shrunk);
                float borderZone = clamp(shape - innerShape, 0.0, 1.0);
                vec4 bc = borderColor;
                sc = mix(fill, bc, borderZone);
            }

            fragColor = vec4(sc.rgb * sc.a, sc.a) * shape * qt_Opacity;
        }
    }
}
