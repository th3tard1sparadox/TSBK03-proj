#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;
uniform sampler2D texUnit2;
out vec4 out_Color;

void main(void)
{
    mat3 lp_filter;
    lp_filter[0] = vec3(1.0/16, 2.0/16, 1.0/16);
    lp_filter[1] = vec3(2.0/16, 4.0/16, 2.0/16);
    lp_filter[2] = vec3(1.0/16, 2.0/16, 1.0/16);
    out_Color = vec4(0.0);
    for (int dx = -1; dx < 2; dx++) {
        for (int dy = -1; dy < 2; dy++) {
            out_Color += lp_filter[dy + 1][dx + 1] * texture(texUnit, vec2(outTexCoord.x + dx / 512.0, outTexCoord.y + dy / 512.0));
        }
    }
}
