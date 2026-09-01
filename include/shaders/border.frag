/*
 * kwin_borders KWin plugin
 * SPDX-FileCopyrightText: 2026 Bartłomiej Ludew <bartekldw>
 * SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifdef GL_ES
precision highp float;
#endif

varying vec2 v_texcoord;

uniform vec2 u_size;
uniform vec2 u_inner_size;
uniform float u_radius;
uniform float u_border;
uniform vec4 u_color1;
uniform vec4 u_color2;

float sdfRoundedRect(vec2 p, vec2 halfSize, float radius)
{
    vec2 d = abs(p) - halfSize + vec2(radius);
    if (radius < 0.001) {
        return max(d.x, d.y);
    }
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - radius;
}

void main()
{
    vec2 uv = v_texcoord * u_size;
    vec2 center = u_size * 0.5;
    vec2 p = uv - center;

    vec2 halfSize = u_inner_size * 0.5;
    float dist = sdfRoundedRect(p, halfSize, u_radius);

    float aa = 1.0;

    float borderOuter = dist - u_border;
    float borderMask = (1.0 - smoothstep(-aa, aa, borderOuter)) * smoothstep(-aa, aa, dist) * step(0.001, u_border);

    float t = v_texcoord.y;
    vec4 gradColor = mix(u_color1, u_color2, t);

    gl_FragColor = vec4(gradColor.rgb, gradColor.a * borderMask);
}