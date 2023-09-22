#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;
uniform sampler2D texUnit2;
out vec4 out_Color;

void main(void)
{
    float value = texture(texUnit, outTexCoord).x;
    if (value >= 1.0) {
        out_Color = vec4(value, value, value, 1.0);
    } else {
        out_Color = vec4(0.0);
    }
}
