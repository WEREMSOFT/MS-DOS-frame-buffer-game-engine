#version 300 es

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTextCoord;


out vec2 textCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    textCoord = vec2(aTextCoord.x, aTextCoord.y);
}