#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 ViewProjection;
uniform mat4 Model;

out vec2 TexCoord;

void main() {
    TexCoord = aTexCoord;
    gl_Position = ViewProjection * Model * vec4(aPos, 1.0);
}