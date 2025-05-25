#version 460 core
out vec4 FragColor;

uniform vec3 Tint;
uniform sampler2D Texture;

in vec2 TexCoord;

void main() {
	vec4 tex = texture(Texture, TexCoord) ;
	if (tex.a < 0.5) discard;
	FragColor = tex * vec4(Tint, 1.0f);
}