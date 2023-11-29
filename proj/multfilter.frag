#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;
uniform sampler2D texUnit2;
out vec4 out_Color;

void main(void)
{
    out_Color = texture(texUnit, outTexCoord) * texture(texUnit2, outTexCoord);
}
