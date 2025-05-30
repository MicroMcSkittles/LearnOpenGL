#version 460 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 TranFragColor;

uniform sampler2D Texture;
uniform vec2 AtlasSize;

in vec2 TexCoord;
in vec2 TexID;
in vec4 Tint;

void main() {
	float unit_width = 1.0f / AtlasSize.x;
    float unit_height = 1.0f / AtlasSize.y;

    vec2 unit_coord = TexCoord;
    unit_coord.x = TexCoord.x * unit_width + (unit_width * TexID.x);
    unit_coord.y = TexCoord.y * unit_height + (unit_height * TexID.y);

	vec4 tex = texture(Texture, unit_coord);
	if (tex.a <= 0.5f) discard;

    FragColor = tex * vec4(Tint.rgb, 1.0f);
    TranFragColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
}