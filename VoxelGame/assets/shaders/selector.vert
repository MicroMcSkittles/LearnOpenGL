#version 460 core
layout (location = 0) in vec3 aPos;

uniform mat4 ViewProjection;
uniform mat4 Model;

void main() {
    gl_Position = ViewProjection * Model * vec4(aPos, 1.0f);
}