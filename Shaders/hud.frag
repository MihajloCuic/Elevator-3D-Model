#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform float uAlpha;
uniform bool uIsTexture;
uniform vec3 uColor;

void main()
{
    if (uIsTexture) {
        vec4 texColor = texture(uTexture, TexCoord);
        FragColor = vec4(texColor.rgb, texColor.a * uAlpha);
    } else {
        FragColor = vec4(uColor, uAlpha);
    }
}
