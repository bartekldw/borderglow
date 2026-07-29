uniform vec2  u_size;
uniform vec2  u_inner_size;
uniform float u_border;
uniform vec4  u_color;

in vec2 v_texcoord;
out vec4 fragColor;

void main() {
    vec2 pos    = v_texcoord * u_size;
    vec2 margin = (u_size - u_inner_size) * 0.5;

    vec2 distFromInner = min(pos - margin, (u_size - margin) - pos);
    float inner = min(distFromInner.x, distFromInner.y);

    float alpha = (inner > -u_border && inner < u_border) ? 1.0 : 0.0;

    fragColor = vec4(u_color.rgb, u_color.a * alpha);
}