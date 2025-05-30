#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec2 aTexID;
layout (location = 3) in vec4 aTint;

uniform mat4 ViewProjection;

out vec2 TexCoord;
out vec2 TexID;
out vec4 Tint;

void main() {
	TexCoord = aTexCoord;
	TexID = aTexID;
	Tint = aTint;
	gl_Position = ViewProjection * vec4(aPos.x, aPos.y, -1, 1.0);
}