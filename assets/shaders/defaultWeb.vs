
attribute vec3 aPos;
uniform vec2 aTextCoord;
varying vec2 textCoord;
void main()
{
    gl_Position = vec4(aPos, 1.0);
    textCoord = vec2(aTextCoord.x, aTextCoord.y);
}