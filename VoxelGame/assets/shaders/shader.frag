#version 460 core
out vec4 FragColor;

in vec2 TexCoord;
in vec2 TextureID;
in float FaceID;

uniform sampler2D tex;

const float Epsilon = 0.00001;

bool pEqual(float f1, float f2) {
    if(f1 < (f2 + Epsilon) && f1 > (f2 - Epsilon)) {
        return true;
    }
    return false;
}

const float Front  = 0;
const float Back   = 1;
const float Left   = 2;
const float Right  = 3;
const float Top    = 4;
const float Bottom = 5;

const ivec2 atlasSize = ivec2(10, 16);

void main()
{
    vec3 tint = vec3(1,1,1);

    if (pEqual(FaceID, Back) || pEqual(FaceID, Right) || pEqual(FaceID, Bottom)) {
       tint = tint * 0.5;
    }

    float unit_width = 1.0 / atlasSize.x;
    float unit_height = 1.0 / atlasSize.y;

    vec2 unit_coord = TexCoord;
    unit_coord.x = TexCoord.x * unit_width + (unit_width * TextureID.x);
    unit_coord.y = TexCoord.y * unit_height + (unit_height * ((atlasSize.y - 1) - TextureID.y));

    vec4 color = texture(tex, unit_coord);
    if(color.a < 0.5) discard;

    FragColor = color * vec4(tint, 1);
} 