#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec2 TextureID;
uniform vec2 AtlasSize;
uniform sampler2D tex;

void main()
{
    vec3 tint = vec3(1,1,1);

    float unit_width = 1.0f / AtlasSize.x;
    float unit_height = 1.0f / AtlasSize.y;

    vec2 unit_coord = TexCoord;
    unit_coord.x = TexCoord.x * unit_width + (unit_width * TextureID.x);
    unit_coord.y = TexCoord.y * unit_height + (unit_height * ((AtlasSize.y - 1) - TextureID.y));

    vec4 color = texture(tex, unit_coord);

    if(color.a < 0.5) discard;

    FragColor = color * vec4(tint, 1);
} 