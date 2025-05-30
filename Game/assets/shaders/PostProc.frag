#version 460 core
out vec4 FragColor;

uniform sampler2D MainColorBuffer;
uniform sampler2D TranColorBuffer;

in vec2 TexCoord;

void main() {
    vec4 mainColor = texture(MainColorBuffer, TexCoord);
    vec4 tranColor = texture(TranColorBuffer, TexCoord);

    FragColor = vec4(mainColor.rgb, 1.0f);
}