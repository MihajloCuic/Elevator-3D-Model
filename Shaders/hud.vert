#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(aPos.x, aPos.z, 0.0, 1.0);
    TexCoord = aTexCoord;
}
