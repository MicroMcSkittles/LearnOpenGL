#version 460 core
out vec4 FragColor;

uniform vec3 Tint;
uniform sampler2D Texture;
uniform vec2 AtlasSize;
uniform vec2 TextureID;

in vec2 TexCoord;

void main() {
    float unit_width = 1.0f / AtlasSize.x;
    float unit_height = 1.0f / AtlasSize.y;

    vec2 unit_coord = TexCoord;
    unit_coord.x = TexCoord.x * unit_width + (unit_width * TextureID.x);
    unit_coord.y = TexCoord.y * unit_height + (unit_height * TextureID.y);

	vec4 tex = texture(Texture, unit_coord);
	if (tex.a < 0.5) discard;
	FragColor = tex * vec4(Tint, 1.0f);
}