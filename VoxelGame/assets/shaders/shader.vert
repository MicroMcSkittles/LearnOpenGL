#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec2 aTextureID;
layout (location = 3) in float aFaceID;

uniform mat4 ViewProjection;
uniform mat4 Model;

out vec2 TexCoord;
out vec2 TextureID;
out float FaceID;

void main()
{
    TexCoord = aTexCoord;
    TextureID = aTextureID;
    FaceID = aFaceID;
    gl_Position = ViewProjection * Model * vec4(aPos, 1.0);
}